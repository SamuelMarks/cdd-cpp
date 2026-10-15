/// @file parse.cpp
/// @brief Implementation of GoogleMock C++ mock client parser into OpenAPI.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include <cctype>
#include <string_view>

namespace cdd_cpp::mocks {

/// @brief Internal helper or implementation for `parse`.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept {
  if (input.empty()) {
    return utils::make_error(utils::ErrorCode::CppAstParseError,
                             "Empty mock input string");
  }

  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Mock Client";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  docstrings::parse_api_docstrings(input, spec);

  size_t pos = 0;
  for (;;) {
    size_t idx = input.find("MOCK_METHOD(", pos);
    if (idx == std::string::npos) {
      break;
    }
    size_t after_open = idx + 12;
    size_t first_comma = input.find(',', after_open);
    size_t second_comma = input.find(',', first_comma + 1);

    std::string_view name_sv(input.data() + first_comma + 1,
                             second_comma - first_comma - 1);
    size_t ns = name_sv.find_first_not_of(" \t\r\n");
    size_t ne = name_sv.find_last_not_of(" \t\r\n");
    std::string func_name(name_sv.substr(ns, ne - ns + 1));
    std::string path = "/" + func_name;
    openapi::Operation op;
    op.operationId = func_name;

    if (func_name.starts_with("get_")) {
      path = "/" + func_name.substr(4);
      (*spec.paths)[path].get = op;
    } else if (func_name.starts_with("post_")) {
      path = "/" + func_name.substr(5);
      (*spec.paths)[path].post = op;
    } else if (func_name.starts_with("put_")) {
      path = "/" + func_name.substr(4);
      (*spec.paths)[path].put = op;
    } else if (func_name.starts_with("delete_")) {
      path = "/" + func_name.substr(7);
      (*spec.paths)[path].delete_op = op;
    } else if (func_name.starts_with("patch_")) {
      path = "/" + func_name.substr(6);
      (*spec.paths)[path].patch = op;
    } else {
      (*spec.paths)[path].get = op;
    }
    pos = second_comma + 1;
  }

  return spec;
}

} // namespace cdd_cpp::mocks
