#include "parse.hpp"
#include <regex>

namespace cdd_cpp::routes {
openapi::OpenAPI parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Server Routes";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  std::regex route_regex(R"(routes\[\"([A-Z]+)\s+([^\"]+)\"\])");
  auto words_begin =
      std::sregex_iterator(input.begin(), input.end(), route_regex);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    std::string method = match[1].str();
    std::string path = match[2].str();

    if (!spec.paths->contains(path)) {
      spec.paths->insert({path, openapi::PathItem{}});
    }

    openapi::Operation op;
    op.operationId = "handler";

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
  return spec;
}
} // namespace cdd_cpp::routes