#pragma once
#include <simdjson.h>
#include <string>

namespace cdd_cpp::routes {
/// Parse routes from string
void parse(const std::string &input);
} // namespace cdd_cpp::routes
