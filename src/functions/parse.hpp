#pragma once
#include <simdjson.h>
#include <string>

namespace cdd_cpp::functions {
/// Parse functions from string
void parse(const std::string &input) noexcept;
} // namespace cdd_cpp::functions
