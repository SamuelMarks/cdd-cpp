#pragma once
#include <string>
#include <sstream>
#include "../openapi/parse.hpp"

namespace cdd_cpp::docstrings {
std::string emit_api_docstrings(const openapi::OpenAPI &spec) noexcept;
std::string emit_operation_docstrings(const openapi::Operation &op) noexcept;
std::string emit_path_docstrings(const openapi::PathItem &pi) noexcept;
} // namespace cdd_cpp::docstrings
