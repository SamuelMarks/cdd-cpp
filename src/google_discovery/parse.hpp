#include <expected>
#pragma once
#include "../openapi/models.hpp"
#include <string>
#include <vector>

namespace cdd_cpp::google_discovery {
/// Parse Google Discovery JSON and emit one or more OpenAPI objects
std::expected<std::vector<openapi::OpenAPI>, std::string> parse(const std::string &input) noexcept;
} // namespace cdd_cpp::google_discovery
