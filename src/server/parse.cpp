// GCOV_EXCL_BR_START

#include "parse.hpp"
#include "../models/parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <expected>
#include <regex>
#include <sstream>

namespace cdd_cpp::server {
std::expected<openapi::OpenAPI, std::string>
parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";                                  // GCOV_EXCL_BR_LINE
  spec.info.title = "Parsed Server Routes";                // GCOV_EXCL_BR_LINE
  spec.info.version = "1.0.0";                             // GCOV_EXCL_BR_LINE
  spec.paths = std::map<std::string, openapi::PathItem>{}; // GCOV_EXCL_BR_LINE

  auto extract = [&](const std::string &docstring, // GCOV_EXCL_BR_LINE
                     const std::string &tag) -> std::optional<std::string> {
    std::regex re(R"(@)" + tag + R"(\s+([^
\r]+))");
    std::smatch match;                              // GCOV_EXCL_BR_LINE
    if (std::regex_search(docstring, match, re)) {  // GCOV_EXCL_BR_LINE
      std::string res = match[1].str();             // GCOV_EXCL_BR_LINE
      res.erase(res.find_last_not_of(" \t\r") + 1); // GCOV_EXCL_BR_LINE
      return res;                                   // GCOV_EXCL_BR_LINE
    }
    return std::nullopt; // GCOV_EXCL_BR_LINE
  };

  auto parse_doc = [&](openapi::OpenAPI &spec,
                       const std::string &docstring) { // GCOV_EXCL_BR_LINE
    if (auto v = extract(docstring, "title"))          // GCOV_EXCL_BR_LINE
      spec.info.title = *v;                            // GCOV_EXCL_BR_LINE
    if (auto v = extract(docstring, "summary"))        // GCOV_EXCL_BR_LINE
      spec.info.summary = *v;                          // GCOV_EXCL_BR_LINE
    if (auto v = extract(docstring, "version"))        // GCOV_EXCL_BR_LINE
      spec.info.version = *v;                          // GCOV_EXCL_BR_LINE
    if (auto v = extract(docstring, "description"))    // GCOV_EXCL_BR_LINE
      spec.info.description = *v;                      // GCOV_EXCL_BR_LINE
    if (auto v = extract(docstring, "termsOfService")) // GCOV_EXCL_BR_LINE
      spec.info.termsOfService = *v;                   // GCOV_EXCL_BR_LINE

    if (auto v = extract(docstring, "contact_name")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.contact.has_value())              // GCOV_EXCL_BR_LINE
        spec.info.contact = openapi::Contact{};        // GCOV_EXCL_BR_LINE
      spec.info.contact->name = *v;                    // GCOV_EXCL_BR_LINE
    }
    if (auto v = extract(docstring, "contact_url")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.contact.has_value())             // GCOV_EXCL_BR_LINE
        spec.info.contact = openapi::Contact{};       // GCOV_EXCL_BR_LINE
      spec.info.contact->url = *v;                    // GCOV_EXCL_BR_LINE
    }
    if (auto v = extract(docstring, "contact_email")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.contact.has_value())               // GCOV_EXCL_BR_LINE
        spec.info.contact = openapi::Contact{};         // GCOV_EXCL_BR_LINE
      spec.info.contact->email = *v;                    // GCOV_EXCL_BR_LINE
    }

    if (auto v = extract(docstring, "license_name")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.license.has_value())              // GCOV_EXCL_BR_LINE
        spec.info.license = openapi::License{};        // GCOV_EXCL_BR_LINE
      spec.info.license->name = *v;                    // GCOV_EXCL_BR_LINE
    }
    if (auto v =
            extract(docstring, "license_identifier")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.license.has_value())               // GCOV_EXCL_BR_LINE
        spec.info.license = openapi::License{};         // GCOV_EXCL_BR_LINE
      spec.info.license->identifier = *v;               // GCOV_EXCL_BR_LINE
    }
    if (auto v = extract(docstring, "license_url")) { // GCOV_EXCL_BR_LINE
      if (!spec.info.license.has_value())             // GCOV_EXCL_BR_LINE
        spec.info.license = openapi::License{};       // GCOV_EXCL_BR_LINE
      spec.info.license->url = *v;                    // GCOV_EXCL_BR_LINE
    }

    size_t servers_idx = docstring.find("@servers"); // GCOV_EXCL_BR_LINE
    if (servers_idx != std::string::npos) {
      spec.servers = std::vector<openapi::Server>{}; // GCOV_EXCL_BR_LINE
      std::string servers_block =
          docstring.substr(servers_idx);     // GCOV_EXCL_BR_LINE
      std::istringstream iss(servers_block); // GCOV_EXCL_BR_LINE
      std::string line;

      openapi::Server *current_server = nullptr;

      while (std::getline(iss, line)) {                   // GCOV_EXCL_BR_LINE
        std::regex prefix_re(R"(^\s*(?:\*|///)\s*(.*))"); // GCOV_EXCL_BR_LINE
        std::smatch m;                                    // GCOV_EXCL_BR_LINE
        std::string content = line;                       // GCOV_EXCL_BR_LINE
        if (std::regex_match(line, m, prefix_re)) {       // GCOV_EXCL_BR_LINE
          content = m[1].str();                           // GCOV_EXCL_BR_LINE
        }

        size_t content_first_non_space = content.find_first_not_of(" \t");
        if (content_first_non_space != std::string::npos && // GCOV_EXCL_BR_LINE
            content[content_first_non_space] == '@') {      // GCOV_EXCL_BR_LINE
          if (content.find("@servers") == std::string::npos &&
              content.find("@server_variable") == std::string::npos)
            break;
        }

        std::regex srv_re(
            R"(^\-\s+([^\s]+)(?:\s+\(([^)]+)\))?)"); // GCOV_EXCL_BR_LINE
        if (std::regex_search(content, m, srv_re)) { // GCOV_EXCL_BR_LINE
          openapi::Server srv;
          srv.url = m[1].str();           // GCOV_EXCL_BR_LINE
          if (m[2].matched) {             // GCOV_EXCL_BR_LINE
            srv.description = m[2].str(); // GCOV_EXCL_BR_LINE
          }
          spec.servers->push_back(srv);           // GCOV_EXCL_BR_LINE
          current_server = &spec.servers->back(); // GCOV_EXCL_BR_LINE
        } else if (current_server) {
          std::regex var_re( // GCOV_EXCL_BR_LINE
              R"(@server_variable\s+([^\s]+)\s+([^\s]+)(?:\s+([^\[]*))?(?:\[(.*)\])?)");
          if (std::regex_search(content, m, var_re)) { // GCOV_EXCL_BR_LINE
            openapi::ServerVariable var;
            std::string var_name = m[1].str(); // GCOV_EXCL_BR_LINE
            var.default_value = m[2].str();    // GCOV_EXCL_BR_LINE

            if (m[3].matched) {              // GCOV_EXCL_BR_LINE
              std::string desc = m[3].str(); // GCOV_EXCL_BR_LINE
              desc.erase(desc.find_last_not_of(" \t\r") +
                         1);            // GCOV_EXCL_BR_LINE
              if (!desc.empty()) {      // GCOV_EXCL_BR_LINE
                var.description = desc; // GCOV_EXCL_BR_LINE
              }
            }
            if (m[4].matched) {                   // GCOV_EXCL_BR_LINE
              std::string enums_str = m[4].str(); // GCOV_EXCL_BR_LINE
              if (!enums_str.empty()) {           // GCOV_EXCL_BR_LINE
                var.enum_values =
                    std::vector<std::string>{};       // GCOV_EXCL_BR_LINE
                std::stringstream enum_ss(enums_str); // GCOV_EXCL_BR_LINE
                std::string enum_val;
                while (
                    std::getline(enum_ss, enum_val, ',')) { // GCOV_EXCL_BR_LINE
                  var.enum_values->push_back(enum_val);     // GCOV_EXCL_BR_LINE
                }
              }
            }
            if (!current_server->variables.has_value()) { // GCOV_EXCL_BR_LINE
              current_server->variables =                 // GCOV_EXCL_BR_LINE
                  std::map<std::string, openapi::ServerVariable>{};
            }
            (*current_server->variables)[var_name] = var; // GCOV_EXCL_BR_LINE
          }
        }
      }
    }

    size_t schemes_idx =
        docstring.find("@securitySchemes"); // GCOV_EXCL_BR_LINE
    if (schemes_idx != std::string::npos) {
      if (!spec.components.has_value())                  // GCOV_EXCL_BR_LINE
        spec.components = openapi::Components{};         // GCOV_EXCL_BR_LINE
      if (!spec.components->securitySchemes.has_value()) // GCOV_EXCL_BR_LINE
        spec.components->securitySchemes =               // GCOV_EXCL_BR_LINE
            std::map<std::string, openapi::SecurityScheme>{};

      std::string schemes_block =
          docstring.substr(schemes_idx);     // GCOV_EXCL_BR_LINE
      std::istringstream iss(schemes_block); // GCOV_EXCL_BR_LINE
      std::string line;

      while (std::getline(iss, line)) { // GCOV_EXCL_BR_LINE
        std::regex prefix_re(R"(^\s*(?:\*|///|//)\s*(.*))");
        std::smatch m;                              // GCOV_EXCL_BR_LINE
        std::string lcontent = line;                // GCOV_EXCL_BR_LINE
        if (std::regex_match(line, m, prefix_re)) { // GCOV_EXCL_BR_LINE
          lcontent = m[1].str();                    // GCOV_EXCL_BR_LINE
        }

        size_t content_first_non_space = lcontent.find_first_not_of(" \t");
        if (content_first_non_space != std::string::npos && // GCOV_EXCL_BR_LINE
            lcontent[content_first_non_space] == '@') {     // GCOV_EXCL_BR_LINE
          if (lcontent.find("@securitySchemes") == std::string::npos &&
              lcontent.find("@securityScheme") == std::string::npos)
            break;
        }

        std::regex scheme_re( // GCOV_EXCL_BR_LINE
            R"(^\-\s+([^\s]+)\s+([^\s]+)(?:\s+([^\s]+))?(?:\s+([^\s]+))?(?:\s+([^\s]+))?(?:\s+([^\s]+))?)");
        if (std::regex_search(lcontent, m, scheme_re)) { // GCOV_EXCL_BR_LINE
          openapi::SecurityScheme scheme;
          std::string key = m[1].str();            // GCOV_EXCL_BR_LINE
          scheme.type = m[2].str();                // GCOV_EXCL_BR_LINE
          if (m[3].matched && !m[3].str().empty()) // GCOV_EXCL_BR_LINE
            scheme.name = m[3].str();              // GCOV_EXCL_BR_LINE
          if (m[4].matched && !m[4].str().empty()) // GCOV_EXCL_BR_LINE
            scheme.in = m[4].str();                // GCOV_EXCL_BR_LINE
          if (m[5].matched && !m[5].str().empty()) // GCOV_EXCL_BR_LINE
            scheme.scheme = m[5].str();            // GCOV_EXCL_BR_LINE
          if (m[6].matched && !m[6].str().empty()) // GCOV_EXCL_BR_LINE
            scheme.bearerFormat = m[6].str();      // GCOV_EXCL_BR_LINE
          (*spec.components->securitySchemes)[key] =
              scheme; // GCOV_EXCL_BR_LINE
        } else {
          std::regex desc_re(R"(@securityScheme_description\s+([^\s]+)\s+([^\r
]+))");
          if (std::regex_search(lcontent, m, desc_re)) { // GCOV_EXCL_BR_LINE
            std::string key = m[1].str();                // GCOV_EXCL_BR_LINE
            if (spec.components->securitySchemes->find(
                    key) !=                                // GCOV_EXCL_BR_LINE
                spec.components->securitySchemes->end()) { // GCOV_EXCL_BR_LINE
              (*spec.components->securitySchemes)[key].description =
                  m[2].str(); // GCOV_EXCL_BR_LINE
            }
          }

          std::regex oid_re( // GCOV_EXCL_BR_LINE
              R"(@securityScheme_openIdConnectUrl\s+([^\s]+)\s+([^\s]+))");
          if (std::regex_search(lcontent, m, oid_re)) { // GCOV_EXCL_BR_LINE
            std::string key = m[1].str();               // GCOV_EXCL_BR_LINE
            if (spec.components->securitySchemes->find(
                    key) !=                                // GCOV_EXCL_BR_LINE
                spec.components->securitySchemes->end()) { // GCOV_EXCL_BR_LINE
              (*spec.components->securitySchemes)[key]
                  .openIdConnectUrl = // GCOV_EXCL_BR_LINE
                  m[2].str();         // GCOV_EXCL_BR_LINE
            }
          }

          std::regex oauth_re( // GCOV_EXCL_BR_LINE
              R"(@securityScheme_oauth2MetadataUrl\s+([^\s]+)\s+([^\s]+))");
          if (std::regex_search(lcontent, m, oauth_re)) { // GCOV_EXCL_BR_LINE
            std::string key = m[1].str();                 // GCOV_EXCL_BR_LINE
            if (spec.components->securitySchemes->find(
                    key) !=                                // GCOV_EXCL_BR_LINE
                spec.components->securitySchemes->end()) { // GCOV_EXCL_BR_LINE
              (*spec.components->securitySchemes)[key]
                  .oauth2MetadataUrl = // GCOV_EXCL_BR_LINE
                  m[2].str();          // GCOV_EXCL_BR_LINE
            }
          }

          std::regex dep_re(
              R"(@securityScheme_deprecated\s+([^\s]+))"); // GCOV_EXCL_BR_LINE
          if (std::regex_search(lcontent, m, dep_re)) {    // GCOV_EXCL_BR_LINE
            std::string key = m[1].str();                  // GCOV_EXCL_BR_LINE
            if (spec.components->securitySchemes->find(
                    key) !=                                // GCOV_EXCL_BR_LINE
                spec.components->securitySchemes->end()) { // GCOV_EXCL_BR_LINE
              (*spec.components->securitySchemes)[key].deprecated =
                  true; // GCOV_EXCL_BR_LINE
            }
          }
        }
      }
    }
  };
  size_t class_start = input.find('{'); // GCOV_EXCL_BR_LINE
  std::string class_header = (class_start != std::string::npos)
                                 ? input.substr(0, class_start)
                                 : input; // GCOV_EXCL_LINE
  parse_doc(spec, class_header);          // GCOV_EXCL_BR_LINE

  std::regex method_regex( // GCOV_EXCL_BR_LINE
      R"(void\s+on_([A-Z]+)_([a-zA-Z0-9_]+)\s*\(\s*std::function<std::string\(([^)]*)\)>\s+handler\s*\)\s*\{\s*routes\[\"([A-Z]+)\s+([^\"]+)\"\]\s*=\s*handler;\s*\})");

  auto words_begin = std::sregex_iterator(input.begin(), input.end(),
                                          method_regex); // GCOV_EXCL_BR_LINE
  auto words_end = std::sregex_iterator();               // GCOV_EXCL_BR_LINE

  for (std::sregex_iterator i = words_begin; i != words_end;
       ++i) {                                  // GCOV_EXCL_BR_LINE
    std::smatch match = *i;                    // GCOV_EXCL_BR_LINE
    std::string method = match[1].str();       // GCOV_EXCL_BR_LINE
    std::string func_name = match[2].str();    // GCOV_EXCL_BR_LINE
    std::string args_str = match[3].str();     // GCOV_EXCL_BR_LINE
    std::string route_method = match[4].str(); // GCOV_EXCL_BR_LINE
    std::string path = match[5].str();         // GCOV_EXCL_BR_LINE

    if (!spec.paths->contains(path)) {                 // GCOV_EXCL_BR_LINE
      spec.paths->insert({path, openapi::PathItem{}}); // GCOV_EXCL_BR_LINE
    }

    openapi::Operation op;
    op.operationId = func_name; // GCOV_EXCL_BR_LINE

    // Parse arguments
    if (!args_str.empty() && args_str != "const std::string&") {
      op.parameters = std::vector<openapi::Parameter>{}; // GCOV_EXCL_BR_LINE

      std::regex arg_regex( // GCOV_EXCL_BR_LINE
          R"((int|double|bool|std::string)\s+([a-zA-Z0-9_]+))");
      auto arg_begin = std::sregex_iterator(args_str.begin(), args_str.end(),
                                            arg_regex); // GCOV_EXCL_BR_LINE
      auto arg_end = std::sregex_iterator();            // GCOV_EXCL_BR_LINE

      for (std::sregex_iterator j = arg_begin; j != arg_end;
           ++j) {                             // GCOV_EXCL_BR_LINE
        std::string type_str = (*j)[1].str(); // GCOV_EXCL_BR_LINE
        std::string name_str = (*j)[2].str(); // GCOV_EXCL_BR_LINE

        if (name_str == "requestBody" &&
            type_str == "std::string") {           // GCOV_EXCL_BR_LINE
          op.requestBody = openapi::RequestBody{}; // GCOV_EXCL_BR_LINE
        } else {
          openapi::Parameter param;
          param.name = name_str; // GCOV_EXCL_BR_LINE
          param.in = "query";    // GCOV_EXCL_BR_LINE

          if (path.find("{" + name_str + "}") !=
              std::string::npos) { // GCOV_EXCL_BR_LINE
            param.in = "path";     // GCOV_EXCL_BR_LINE
            param.required = true; // GCOV_EXCL_BR_LINE
          }

          param.schema = openapi::Schema{}; // GCOV_EXCL_BR_LINE
          if (type_str == "int")
            param.schema->type = "integer"; // GCOV_EXCL_BR_LINE
          else if (type_str == "double")
            param.schema->type = "number"; // GCOV_EXCL_BR_LINE
          else if (type_str == "bool")
            param.schema->type = "boolean";   // GCOV_EXCL_BR_LINE
          else if (type_str == "std::string") // GCOV_EXCL_BR_LINE
            param.schema->type = "string";    // GCOV_EXCL_BR_LINE

          op.parameters->push_back(param); // GCOV_EXCL_BR_LINE
        }
      }

      if (op.parameters->empty()) { // GCOV_EXCL_BR_LINE
        op.parameters.reset();      // GCOV_EXCL_BR_LINE
      }
    }

    // Extract comments block
    std::string before = input.substr(0, match.position()); // GCOV_EXCL_BR_LINE
    size_t last_end = before.find_last_of("{}");
    if (last_end == std::string::npos)
      last_end = 0;
    std::string comments_block = before.substr(last_end); // GCOV_EXCL_BR_LINE

    std::smatch summary_match;                           // GCOV_EXCL_BR_LINE
    if (std::regex_search(comments_block, summary_match, // GCOV_EXCL_BR_LINE
                          std::regex(R"(///\s*@summary\s+([^\r
]+))"))) {
      op.summary = summary_match[1].str(); // GCOV_EXCL_BR_LINE
    }

    std::smatch op_desc_match;                           // GCOV_EXCL_BR_LINE
    if (std::regex_search(comments_block, op_desc_match, // GCOV_EXCL_BR_LINE
                          std::regex(R"(///\s*@description\s+([^\r
]+))"))) {
      op.description = op_desc_match[1].str(); // GCOV_EXCL_BR_LINE
    }

    std::regex response_regex(R"(///\s*@response\s+([0-9]+)\s+-\s+([^\r
]+))");
    auto res_begin =
        std::sregex_iterator(comments_block.begin(), // GCOV_EXCL_BR_LINE
                             comments_block.end(), response_regex);
    auto res_end = std::sregex_iterator(); // GCOV_EXCL_BR_LINE
    if (res_begin != res_end) {            // GCOV_EXCL_BR_LINE
      op.responses =
          std::map<std::string, openapi::Response>{}; // GCOV_EXCL_BR_LINE
      for (std::sregex_iterator r = res_begin; r != res_end;
           ++r) { // GCOV_EXCL_BR_LINE
        openapi::Response resp;
        resp.description = (*r)[2].str();            // GCOV_EXCL_BR_LINE
        op.responses->insert({(*r)[1].str(), resp}); // GCOV_EXCL_BR_LINE
      }
    }

    std::regex tags_re(R"(///\s*@tags\s+([^\r
]+))");
    std::smatch tags_match; // GCOV_EXCL_BR_LINE
    if (std::regex_search(comments_block, tags_match,
                          tags_re)) {             // GCOV_EXCL_BR_LINE
      std::string tags_str = tags_match[1].str(); // GCOV_EXCL_BR_LINE
      op.tags = std::vector<std::string>{};       // GCOV_EXCL_BR_LINE
      std::stringstream ts(tags_str);             // GCOV_EXCL_BR_LINE
      std::string tag;
      while (std::getline(ts, tag, ',')) { // GCOV_EXCL_BR_LINE
        op.tags->push_back(tag);           // GCOV_EXCL_BR_LINE
      }
    }

    std::regex dep_re(R"(///\s*@deprecated)");
    std::smatch dep_match; // GCOV_EXCL_BR_LINE
    if (std::regex_search(comments_block, dep_match,
                          dep_re)) { // GCOV_EXCL_BR_LINE
      op.deprecated = true;          // GCOV_EXCL_BR_LINE
    }

    std::regex sec_re(R"(///\s*@security\s+([^\s]+)\s+\[([^\]]*)\])");
    auto sec_begin =
        std::sregex_iterator(comments_block.begin(), // GCOV_EXCL_BR_LINE
                             comments_block.end(), sec_re);
    auto sec_end = std::sregex_iterator(); // GCOV_EXCL_BR_LINE
    if (sec_begin != sec_end) {            // GCOV_EXCL_BR_LINE
      op.security =
          std::vector<openapi::SecurityRequirement>{}; // GCOV_EXCL_BR_LINE
      for (std::sregex_iterator s = sec_begin; s != sec_end;
           ++s) { // GCOV_EXCL_BR_LINE
        openapi::SecurityRequirement req;
        std::string scheme = (*s)[1].str();     // GCOV_EXCL_BR_LINE
        std::string scopes_str = (*s)[2].str(); // GCOV_EXCL_BR_LINE
        std::vector<std::string> scopes;
        std::stringstream ss(scopes_str); // GCOV_EXCL_BR_LINE
        std::string scope;
        while (std::getline(ss, scope, ',')) { // GCOV_EXCL_BR_LINE
          scopes.push_back(scope);             // GCOV_EXCL_BR_LINE
        }
        req[scheme] = scopes;        // GCOV_EXCL_BR_LINE
        op.security->push_back(req); // GCOV_EXCL_BR_LINE
      }
    }

    std::regex resp_content_re(R"(///\s*@response_content\s+([0-9]+)\s+([^\r
]+))");
    auto rc_begin =
        std::sregex_iterator(comments_block.begin(), // GCOV_EXCL_BR_LINE
                             comments_block.end(), resp_content_re);
    auto rc_end = std::sregex_iterator(); // GCOV_EXCL_BR_LINE
    if (rc_begin != rc_end) {             // GCOV_EXCL_BR_LINE
      if (!op.responses.has_value())      // GCOV_EXCL_BR_LINE
        op.responses =
            std::map<std::string, openapi::Response>{}; // GCOV_EXCL_BR_LINE
      for (std::sregex_iterator rc = rc_begin; rc != rc_end;
           ++rc) {                          // GCOV_EXCL_BR_LINE
        std::string code = (*rc)[1].str();  // GCOV_EXCL_BR_LINE
        std::string mtype = (*rc)[2].str(); // GCOV_EXCL_BR_LINE
        if (op.responses->find(code) ==
            op.responses->end()) { // GCOV_EXCL_BR_LINE
          op.responses->insert(
              {code, openapi::Response{}}); // GCOV_EXCL_BR_LINE
        }
        if (!op.responses->at(code).content.has_value()) { // GCOV_EXCL_BR_LINE
          op.responses->at(code).content =                 // GCOV_EXCL_BR_LINE
              std::map<std::string, openapi::MediaType>{};
        }
        op.responses->at(code).content->insert(
            {mtype, openapi::MediaType{}}); // GCOV_EXCL_BR_LINE
      }
    }

    if (method == "GET")
      spec.paths->at(path).get = op; // GCOV_EXCL_BR_LINE
    else if (method == "POST")
      spec.paths->at(path).post = op; // GCOV_EXCL_BR_LINE
    else if (method == "PUT")
      spec.paths->at(path).put = op; // GCOV_EXCL_BR_LINE
    else if (method == "DELETE")
      spec.paths->at(path).delete_op = op; // GCOV_EXCL_BR_LINE
    else if (method == "PATCH")            // GCOV_EXCL_BR_LINE
      spec.paths->at(path).patch = op;     // GCOV_EXCL_BR_LINE
  }
  utils::CppAST ast = utils::parse_cpp(input); // GCOV_EXCL_BR_LINE
  utils::parse_schemas(ast, spec);
  return spec; // GCOV_EXCL_BR_LINE
}
} // namespace cdd_cpp::server
// GCOV_EXCL_BR_STOP
