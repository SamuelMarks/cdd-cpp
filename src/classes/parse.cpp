#include "parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <regex>

namespace cdd_cpp::classes {

openapi::OpenAPI parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed API Client";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  utils::CppAST ast = utils::parse_cpp(input);

  std::regex url_regex(R"(base_url\s*\+\s*\"([^\"]+)\")");
  std::regex method_regex(R"(CURLOPT_CUSTOMREQUEST,\s*\"([^\"]+)\")");

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

} // namespace cdd_cpp::classes
