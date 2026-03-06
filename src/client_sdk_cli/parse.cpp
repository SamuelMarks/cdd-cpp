#include <expected>
#include "parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <regex>

namespace cdd_cpp::client_sdk_cli {

std::expected<openapi::OpenAPI, std::string> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed CLI";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  
  auto extract = [&](const std::string& docstring, const std::string& tag) -> std::optional<std::string> {
      std::regex re(R"(@)" + tag + R"(\s+([^

]+))");
      std::smatch match;
      if (std::regex_search(docstring, match, re)) {
          std::string res = match[1].str();
          res.erase(res.find_last_not_of(" \t\r") + 1);
          return res;
      }
      return std::nullopt;
  };

  auto parse_doc = [&](openapi::OpenAPI& spec, const std::string& docstring) {
      if (auto v = extract(docstring, "title")) spec.info.title = *v;
      if (auto v = extract(docstring, "summary")) spec.info.summary = *v;
      if (auto v = extract(docstring, "version")) spec.info.version = *v;
      if (auto v = extract(docstring, "description")) spec.info.description = *v;
      if (auto v = extract(docstring, "termsOfService")) spec.info.termsOfService = *v;
      
      if (auto v = extract(docstring, "contact_name")) {
          if (!spec.info.contact.has_value()) spec.info.contact = openapi::Contact{};
          spec.info.contact->name = *v;
      }
      if (auto v = extract(docstring, "contact_url")) {
          if (!spec.info.contact.has_value()) spec.info.contact = openapi::Contact{};
          spec.info.contact->url = *v;
      }
      if (auto v = extract(docstring, "contact_email")) {
          if (!spec.info.contact.has_value()) spec.info.contact = openapi::Contact{};
          spec.info.contact->email = *v;
      }
      
      if (auto v = extract(docstring, "license_name")) {
          if (!spec.info.license.has_value()) spec.info.license = openapi::License{};
          spec.info.license->name = *v;
      }
      if (auto v = extract(docstring, "license_identifier")) {
          if (!spec.info.license.has_value()) spec.info.license = openapi::License{};
          spec.info.license->identifier = *v;
      }
      if (auto v = extract(docstring, "license_url")) {
          if (!spec.info.license.has_value()) spec.info.license = openapi::License{};
          spec.info.license->url = *v;
      }

      size_t servers_idx = docstring.find("@servers");
      if (servers_idx != std::string::npos) {
          spec.servers = std::vector<openapi::Server>{};
          std::string servers_block = docstring.substr(servers_idx);
          std::istringstream iss(servers_block);
          std::string line;
          
          openapi::Server* current_server = nullptr;

          while (std::getline(iss, line)) {
              std::regex prefix_re(R"(^\s*(?:\*|///)\s*(.*))");
              std::smatch m;
              std::string content = line;
              if (std::regex_match(line, m, prefix_re)) {
                  content = m[1].str();
              } else {
                  size_t first_non_space = line.find_first_not_of(" \t");
                  if (first_non_space != std::string::npos && line[first_non_space] == '@') {
                       if (line.find("@servers") == std::string::npos) break;
                  }
              }

              std::regex srv_re(R"(^\-\s+([^\s]+)(?:\s+\(([^)]+)\))?)");
              if (std::regex_search(content, m, srv_re)) {
                  openapi::Server srv;
                  srv.url = m[1].str();
                  if (m[2].matched) {
                      srv.description = m[2].str();
                  }
                  spec.servers->push_back(srv);
                  current_server = &spec.servers->back();
              } else if (current_server) {
                  std::regex var_re(R"(@server_variable\s+([^\s]+)\s+([^\s]+)(?:\s+([^\[]*))?(?:\[(.*)\])?)");
                  if (std::regex_search(content, m, var_re)) {
                      openapi::ServerVariable var;
                      std::string var_name = m[1].str();
                      var.default_value = m[2].str();
                      
                      if (m[3].matched) {
                          std::string desc = m[3].str();
                          desc.erase(desc.find_last_not_of(" \t\r") + 1);
                          if (!desc.empty()) {
                              var.description = desc;
                          }
                      }
                      if (m[4].matched) {
                          std::string enums_str = m[4].str();
                          if (!enums_str.empty()) {
                              var.enum_values = std::vector<std::string>{};
                              std::stringstream enum_ss(enums_str);
                              std::string enum_val;
                              while (std::getline(enum_ss, enum_val, ',')) {
                                  var.enum_values->push_back(enum_val);
                              }
                          }
                      }
                      if (!current_server->variables.has_value()) {
                          current_server->variables = std::map<std::string, openapi::ServerVariable>{};
                      }
                      (*current_server->variables)[var_name] = var;
                  } else if (content.find("@") != std::string::npos && content.find("@server_variable") == std::string::npos) {
                      if (content.find("@servers") == std::string::npos) break;
                  }
              }
          }
      }
  };

  parse_doc(spec, input);

  // Find subcommands
  std::regex cmd_re(R"(if\s*\(\s*command\s*==\s*\"([^\"]+)\"\s*\)\s*\{([^}]+)\})");
  std::sregex_iterator cmd_it(input.begin(), input.end(), cmd_re);
  std::sregex_iterator end;

  while (cmd_it != end) {
      std::string cmd = (*cmd_it)[1].str();
      std::string body = (*cmd_it)[2].str();

      std::string path = "/" + cmd;
      openapi::PathItem pi;
      openapi::Operation op;
      op.operationId = cmd;

      std::regex tags_re(R"(//\s*@tags\s+([^\r
]+))");
      std::smatch tags_match;
      if (std::regex_search(body, tags_match, tags_re)) {
          std::string tags_str = tags_match[1].str();
          op.tags = std::vector<std::string>{};
          std::stringstream ts(tags_str);
          std::string tag;
          while (std::getline(ts, tag, ',')) {
              op.tags->push_back(tag);
          }
      }
      
      std::regex dep_re(R"(//\s*@deprecated)");
      if (std::regex_search(body, tags_match, dep_re)) {
          op.deprecated = true;
      }
      
      std::regex sec_re(R"(//\s*@security\s+([^\s]+)\s+\[([^\]]*)\])");
      auto sec_begin = std::sregex_iterator(body.begin(), body.end(), sec_re);
      auto sec_end = std::sregex_iterator();
      if (sec_begin != sec_end) {
          op.security = std::vector<openapi::SecurityRequirement>{};
          for (std::sregex_iterator s = sec_begin; s != sec_end; ++s) {
              openapi::SecurityRequirement req;
              std::string scheme = (*s)[1].str();
              std::string scopes_str = (*s)[2].str();
              std::vector<std::string> scopes;
              std::stringstream ss(scopes_str);
              std::string scope;
              while (std::getline(ss, scope, ',')) {
                  scopes.push_back(scope);
              }
              req[scheme] = scopes;
              op.security->push_back(req);
          }
      }
      
      std::regex resp_content_re(R"(//\s*@response_content\s+([0-9]+)\s+([^\r
]+))");
      auto rc_begin = std::sregex_iterator(body.begin(), body.end(), resp_content_re);
      auto rc_end = std::sregex_iterator();
      if (rc_begin != rc_end) {
          if (!op.responses.has_value()) op.responses = std::map<std::string, openapi::Response>{};
          for (std::sregex_iterator rc = rc_begin; rc != rc_end; ++rc) {
              std::string code = (*rc)[1].str();
              std::string mtype = (*rc)[2].str();
              if (op.responses->find(code) == op.responses->end()) {
                  op.responses->insert({code, openapi::Response{}});
              }
              if (!op.responses->at(code).content.has_value()) {
                  op.responses->at(code).content = std::map<std::string, openapi::MediaType>{};
              }
              op.responses->at(code).content->insert({mtype, openapi::MediaType{}});
          }
      }


      std::regex op_desc_re(R"(//\s*([^
]+))");
      std::smatch match;
      if (std::regex_search(body, match, op_desc_re)) {
          op.summary = match[1].str();
          op.description = match[1].str();
      }

      std::vector<openapi::Parameter> params;
      std::regex param_re(R"(if\s*\(\s*arg\s*==\s*\"--([^\"]+)\"\s*\))");
      std::sregex_iterator p_it(body.begin(), body.end(), param_re);
      while (p_it != end) {
          std::string p_name = (*p_it)[1].str();
          if (p_name == "body") {
              openapi::RequestBody reqBody;
              reqBody.description = "CLI body parameter";
              op.requestBody = reqBody;
          } else {
              openapi::Parameter p;
              p.name = p_name;
              p.in = "query"; // default to query

              std::string pdesc_str = R"(//\s*@param\s+)" + p_name + R"(\s+([^
]+))";
              std::regex pdesc_re(pdesc_str);
              std::smatch pdesc_match;
              if (std::regex_search(body, pdesc_match, pdesc_re)) {
                  p.description = pdesc_match[1].str();
              }
              std::string pex_str = R"(//\s*@param_example\s+)" + p_name + R"(\s+([^
]+))";
              std::regex pex_re(pex_str);
              if (std::regex_search(body, pdesc_match, pex_re)) {
                  p.example = pdesc_match[1].str();
              }
              std::string pdep_str = R"(//\s*@param_deprecated\s+)" + p_name;
              std::regex pdep_re(pdep_str);
              if (std::regex_search(body, pdesc_match, pdep_re)) {
                  p.deprecated = true;
              }

              params.push_back(p);
          }
          ++p_it;
      }

      if (!params.empty()) {
          op.parameters = params;
      }

      pi.post = op; // default to post
      spec.paths->insert({path, pi});
      ++cmd_it;
  }

  return spec;
}

} // namespace cdd_cpp::client_sdk_cli
