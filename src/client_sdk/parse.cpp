#include <expected>
#include "parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <regex>
#include "../models/parse.hpp"

namespace cdd_cpp::client_sdk {

std::expected<openapi::OpenAPI, std::string> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed API Client";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  utils::CppAST ast = utils::parse_cpp(input);

  
  auto extract = [&](const std::string& docstring, const std::string& tag) -> std::optional<std::string> {
      std::regex re(R"(@)" + tag + R"(\s+([^\r
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

              std::regex srv_re(R"(^\-\s+([^\r
\s]+)(?:\s+\(([^\r
)]+)\))?)");
              if (std::regex_search(content, m, srv_re)) {
                  openapi::Server srv;
                  srv.url = m[1].str();
                  if (m[2].matched) {
                      srv.description = m[2].str();
                  }
                  spec.servers->push_back(srv);
                  current_server = &spec.servers->back();
              } else if (current_server) {
                  std::regex var_re(R"(@server_variable\s+([^\r
\s]+)\s+([^\r
\s]+)(?:\s+([^\r
\[]*))?(?:\[(.*)\])?)");
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

      size_t schemes_idx = docstring.find("@securitySchemes");
      if (schemes_idx != std::string::npos) {
          if (!spec.components.has_value()) spec.components = openapi::Components{};
          if (!spec.components->securitySchemes.has_value()) spec.components->securitySchemes = std::map<std::string, openapi::SecurityScheme>{};
          
          std::string schemes_block = docstring.substr(schemes_idx);
          std::istringstream iss(schemes_block);
          std::string line;
          
          while (std::getline(iss, line)) {
              std::regex prefix_re(R"(^\s*(?:\*|///|//)\s*(.*))");
              std::smatch m;
              std::string lcontent = line;
              if (std::regex_match(line, m, prefix_re)) {
                  lcontent = m[1].str();
              } else {
                  size_t first_non_space = line.find_first_not_of(" \t");
                  if (first_non_space != std::string::npos && line[first_non_space] == '@') {
                       if (line.find("@securitySchemes") == std::string::npos) break;
                  }
              }

              std::regex scheme_re(R"(^\-\s+([^\r
\s]+)\s+([^\r
\s]+)(?:\s+([^\r
\s]+))?(?:\s+([^\r
\s]+))?(?:\s+([^\r
\s]+))?(?:\s+([^\r
\s]+))?)");
              if (std::regex_search(lcontent, m, scheme_re)) {
                  openapi::SecurityScheme scheme;
                  std::string key = m[1].str();
                  scheme.type = m[2].str();
                  if (m[3].matched && !m[3].str().empty()) scheme.name = m[3].str();
                  if (m[4].matched && !m[4].str().empty()) scheme.in = m[4].str();
                  if (m[5].matched && !m[5].str().empty()) scheme.scheme = m[5].str();
                  if (m[6].matched && !m[6].str().empty()) scheme.bearerFormat = m[6].str();
                  (*spec.components->securitySchemes)[key] = scheme;
              } else {
                  std::regex desc_re(R"(@securityScheme_description\s+([^\r
\s]+)\s+([^\r
\r
]+))");
                  if (std::regex_search(lcontent, m, desc_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].description = m[2].str();
                      }
                  }
                  
                  std::regex oid_re(R"(@securityScheme_openIdConnectUrl\s+([^\r
\s]+)\s+([^\r
\s]+))");
                  if (std::regex_search(lcontent, m, oid_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].openIdConnectUrl = m[2].str();
                      }
                  }
                  
                  std::regex oauth_re(R"(@securityScheme_oauth2MetadataUrl\s+([^\r
\s]+)\s+([^\r
\s]+))");
                  if (std::regex_search(lcontent, m, oauth_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].oauth2MetadataUrl = m[2].str();
                      }
                  }
                  
                  std::regex dep_re(R"(@securityScheme_deprecated\s+([^\r
\s]+))");
                  if (std::regex_search(lcontent, m, dep_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].deprecated = true;
                      }
                  }
                  
                  if (lcontent.find("@") != std::string::npos && lcontent.find("@securityScheme") == std::string::npos) {
                      if (lcontent.find("@securitySchemes") == std::string::npos) break;
                  }
              }
          }
      }

  };  for (const auto& cls : ast.classes) {
      if (cls.name == "Client") {
          parse_doc(spec, cls.docstring);
      }
  }

utils::parse_schemas(ast, spec);


  std::regex url_regex(R"(base_url\s*\+\s*\"([^\r
\"]+)\")");
  std::regex method_regex(R"(CURLOPT_CUSTOMREQUEST,\s*\"([^\r
\"]+)\")");

  for (const auto &fn : ast.functions) {
    if (fn.body.empty())
      continue;

    std::smatch url_match;
    if (std::regex_search(fn.body, url_match, url_regex)) {
      std::string path = url_match[1].str();

      std::string method = "GET";
      std::smatch method_match;
      if (std::regex_search(fn.body, method_match, method_regex)) {
        method = method_match[1].str();
      }

      openapi::Operation op;
      op.operationId = fn.name;
      
      std::regex summary_re(R"(@summary\s+([^\r

\r]+))");
      std::regex desc_re(R"(@description\s+([^\r

\r]+))");
      std::smatch match;
      if (std::regex_search(fn.docstring, match, summary_re)) {
          op.summary = match[1].str();
      }
      if (std::regex_search(fn.docstring, match, desc_re)) {
          op.description = match[1].str();
      }

      std::regex tags_re(R"(@tags\s+([^\r

]+))");
      std::smatch tags_match;
      if (std::regex_search(fn.docstring, tags_match, tags_re)) {
          std::string tags_str = tags_match[1].str();
          op.tags = std::vector<std::string>{};
          std::stringstream ts(tags_str);
          std::string tag;
          while (std::getline(ts, tag, ',')) {
              op.tags->push_back(tag);
          }
      }
      
      std::regex dep_re(R"(@deprecated)");
      if (std::regex_search(fn.docstring, tags_match, dep_re)) {
          op.deprecated = true;
      }
      
      std::regex sec_re(R"(@security\s+([^\r
\s]+)\s+\[([^\r
\]]*)\])");
      auto sec_begin = std::sregex_iterator(fn.docstring.begin(), fn.docstring.end(), sec_re);
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
      
      std::regex resp_content_re(R"(@response_content\s+([0-9]+)\s+([^\r

]+))");
      auto rc_begin = std::sregex_iterator(fn.docstring.begin(), fn.docstring.end(), resp_content_re);
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


      std::vector<openapi::Parameter> params;
      for (const auto& arg : fn.params) {
          if (arg.name == "body") {
              openapi::RequestBody reqBody;
              reqBody.description = "Parsed request body";
              op.requestBody = reqBody;
          } else {
              openapi::Parameter p;
              p.name = arg.name;
              // Simple heuristic: if the path contains {arg.name}, it's a path param, else query.
              if (path.find("<" + arg.name + ">") != std::string::npos || path.find("{" + arg.name + "}") != std::string::npos) {
                  p.in = "path";
              } else {
                  p.in = "query";
              }
              
              p.schema = openapi::Schema{};
              if (arg.type == "int") p.schema->type = "integer";
              else if (arg.type == "double") p.schema->type = "number";
              else if (arg.type == "bool") p.schema->type = "boolean";
              else p.schema->type = "string";
              
              params.push_back(p);
          }
      }
      if (!params.empty()) {
          op.parameters = params;
      }

      if (!spec.paths->contains(path)) {
        spec.paths->insert({path, openapi::PathItem{}});
      }

      if (method == "GET")
        spec.paths->at(path).get = op;
      else if (method == "POST")
        spec.paths->at(path).post = op;
      else if (method == "PUT")
        spec.paths->at(path).put = op;
      else if (method == "DELETE")
        spec.paths->at(path).delete_op = op;
      else if (method == "PATCH")
        spec.paths->at(path).patch = op;
    }
  }

  return spec;
}

} // namespace cdd_cpp::client_sdk
