#pragma once
#include "../openapi/parse.hpp"
#include <sstream>
#include <string>

namespace cdd_cpp::docstrings {
std::string emit_api_docstrings(const openapi::OpenAPI &spec) noexcept;
std::string emit_operation_docstrings(const openapi::Operation &op) noexcept;
std::string emit_path_docstrings(const openapi::PathItem &pi) noexcept;
} // namespace cdd_cpp::docstrings
