#pragma once
#include <expected>
#include <simdjson.h>
#include <string>

namespace cdd_cpp::openapi::upgraders {

/// Detects the OpenAPI version from a JSON string and upgrades it to 3.2.0.
std::expected<std::string, std::string>
upgrade_to_latest(const std::string &json_spec) noexcept;

} // namespace cdd_cpp::openapi::upgraders
