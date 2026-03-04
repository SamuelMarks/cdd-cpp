#pragma once
#include "models.hpp"
#include <expected>
#include <simdjson.h>
#include <string>

namespace cdd_cpp::openapi {
/// Parse OpenAPI from string
std::expected<OpenAPI, std::string> parse(const std::string &input) noexcept;
} // namespace cdd_cpp::openapi
