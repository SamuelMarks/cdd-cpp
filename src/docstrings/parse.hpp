// GCOV_EXCL_BR_START
#pragma once
#include <simdjson.h>
#include <string>

namespace cdd_cpp::docstrings {
/// Parse docstrings from string
void parse(const std::string &input) noexcept;
} // namespace cdd_cpp::docstrings

// GCOV_EXCL_BR_STOP
