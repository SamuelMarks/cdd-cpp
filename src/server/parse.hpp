#include <expected>
#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::server {
/// Parse routes from C++ server code
std::expected<openapi::OpenAPI, std::string> parse(const std::string &input) noexcept;
} // namespace cdd_cpp::server