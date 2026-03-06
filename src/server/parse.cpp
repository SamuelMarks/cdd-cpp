#include <expected>
#include "parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <regex>
#include "../models/parse.hpp"
#include "../models/parse.hpp"

namespace cdd_cpp::server {
std::expected<openapi::OpenAPI, std::string> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Server Routes";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  auto extract = [&](const std::string& docstring, const std::string& tag) -> std::optional<std::string> {
      std::regex re(R"(@)" + tag + R"(\s+([^
\r]+))");
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

              std::regex scheme_re(R"(^\-\s+([^\s]+)\s+([^\s]+)(?:\s+([^\s]+))?(?:\s+([^\s]+))?(?:\s+([^\s]+))?(?:\s+([^\s]+))?)");
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
                  std::regex desc_re(R"(@securityScheme_description\s+([^\s]+)\s+([^\r
]+))");
                  if (std::regex_search(lcontent, m, desc_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].description = m[2].str();
                      }
                  }
                  
                  std::regex oid_re(R"(@securityScheme_openIdConnectUrl\s+([^\s]+)\s+([^\s]+))");
                  if (std::regex_search(lcontent, m, oid_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].openIdConnectUrl = m[2].str();
                      }
                  }
                  
                  std::regex oauth_re(R"(@securityScheme_oauth2MetadataUrl\s+([^\s]+)\s+([^\s]+))");
                  if (std::regex_search(lcontent, m, oauth_re)) {
                      std::string key = m[1].str();
                      if (spec.components->securitySchemes->find(key) != spec.components->securitySchemes->end()) {
                          (*spec.components->securitySchemes)[key].oauth2MetadataUrl = m[2].str();
                      }
                  }
                  
                  std::regex dep_re(R"(@securityScheme_deprecated\s+([^\s]+))");
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

  };

  
  size_t class_start = input.find('{');
  std::string class_header = (class_start != std::string::npos) ? input.substr(0, class_start) : input;
  parse_doc(spec, class_header);

  std::regex method_regex(R"(void\s+on_([A-Z]+)_([a-zA-Z0-9_]+)\s*\(\s*std::function<std::string\(([^)]*)\)>\s+handler\s*\)\s*\{\s*routes\[\"([A-Z]+)\s+([^\"]+)\"\]\s*=\s*handler;\s*\})");

  auto words_begin = std::sregex_iterator(input.begin(), input.end(), method_regex);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    std::string method = match[1].str();
    std::string func_name = match[2].str();
    std::string args_str = match[3].str();
    std::string route_method = match[4].str();
    std::string path = match[5].str();

    if (!spec.paths->contains(path)) {
      spec.paths->insert({path, openapi::PathItem{}});
    }

    openapi::Operation op;
    op.operationId = func_name;

    // Parse arguments
    if (!args_str.empty() && args_str != "const std::string&") {
        op.parameters = std::vector<openapi::Parameter>{};
        
        std::regex arg_regex(R"((int|double|bool|std::string)\s+([a-zA-Z0-9_]+))");
        auto arg_begin = std::sregex_iterator(args_str.begin(), args_str.end(), arg_regex);
        auto arg_end = std::sregex_iterator();

        for (std::sregex_iterator j = arg_begin; j != arg_end; ++j) {
            std::string type_str = (*j)[1].str();
            std::string name_str = (*j)[2].str();
            
            if (name_str == "requestBody" && type_str == "std::string") {
                op.requestBody = openapi::RequestBody{};
            } else {
                openapi::Parameter param;
                param.name = name_str;
                param.in = "query"; 
                
                if (path.find("{" + name_str + "}") != std::string::npos) {
                    param.in = "path";
                    param.required = true;
                }
                
                param.schema = openapi::Schema{};
                if (type_str == "int") param.schema->type = "integer";
                else if (type_str == "double") param.schema->type = "number";
                else if (type_str == "bool") param.schema->type = "boolean";
                else if (type_str == "std::string") param.schema->type = "string";
                
                op.parameters->push_back(param);
            }
        }
        
        if (op.parameters->empty()) {
            op.parameters.reset();
        }
    }

    // Extract comments block
    std::string before = input.substr(0, match.position());
    size_t last_end = before.find_last_of("{}");
    if (last_end == std::string::npos) last_end = 0;
    std::string comments_block = before.substr(last_end);

    std::smatch summary_match;
    if (std::regex_search(comments_block, summary_match, std::regex(R"(///\s*@summary\s+([^\r
]+))"))) {
        op.summary = summary_match[1].str();
    }
    
    std::smatch op_desc_match;
    if (std::regex_search(comments_block, op_desc_match, std::regex(R"(///\s*@description\s+([^\r
]+))"))) {
        op.description = op_desc_match[1].str();
    }
    
    std::regex response_regex(R"(///\s*@response\s+([0-9]+)\s+-\s+([^\r
]+))");
    auto res_begin = std::sregex_iterator(comments_block.begin(), comments_block.end(), response_regex);
    auto res_end = std::sregex_iterator();
    if (res_begin != res_end) {
        op.responses = std::map<std::string, openapi::Response>{};
        for (std::sregex_iterator r = res_begin; r != res_end; ++r) {
            openapi::Response resp;
            resp.description = (*r)[2].str();
            op.responses->insert({(*r)[1].str(), resp});
        }
    }

    std::regex tags_re(R"(///\s*@tags\s+([^\r
]+))");
    std::smatch tags_match;
    if (std::regex_search(comments_block, tags_match, tags_re)) {
        std::string tags_str = tags_match[1].str();
        op.tags = std::vector<std::string>{};
        std::stringstream ts(tags_str);
        std::string tag;
        while (std::getline(ts, tag, ',')) {
            op.tags->push_back(tag);
        }
    }
    
    std::regex dep_re(R"(///\s*@deprecated)");
    std::smatch dep_match;
    if (std::regex_search(comments_block, dep_match, dep_re)) {
        op.deprecated = true;
    }
    
    std::regex sec_re(R"(///\s*@security\s+([^\s]+)\s+\[([^\]]*)\])");
    auto sec_begin = std::sregex_iterator(comments_block.begin(), comments_block.end(), sec_re);
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
    
    std::regex resp_content_re(R"(///\s*@response_content\s+([0-9]+)\s+([^\r
]+))");
    auto rc_begin = std::sregex_iterator(comments_block.begin(), comments_block.end(), resp_content_re);
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
  utils::CppAST ast = utils::parse_cpp(input);
  utils::parse_schemas(ast, spec);
  return spec;
}
} // namespace cdd_cpp::server
