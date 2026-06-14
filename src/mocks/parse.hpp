// GCOV_EXCL_BR_START
#pragma once
#include <simdjson.h>
#include <string>

namespace cdd_cpp::mocks {
/// Parse mocks from string
void parse(const std::string &input) noexcept;
} // namespace cdd_cpp::mocks

// GCOV_EXCL_BR_STOP
