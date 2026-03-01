#pragma once
#include "../openapi/models.hpp"
#include <string>
#include <vector>

namespace cdd_cpp::google_discovery {
/// Parse Google Discovery JSON and emit one or more OpenAPI objects
std::vector<openapi::OpenAPI> parse(const std::string &input);
} // namespace cdd_cpp::google_discovery
