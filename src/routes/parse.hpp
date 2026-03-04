#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::routes {
/// Parse routes from C++ server code
openapi::OpenAPI parse(const std::string &input) noexcept;
} // namespace cdd_cpp::routes