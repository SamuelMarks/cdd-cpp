#pragma once
#include <simdjson.h>
#include <string>

namespace cdd_cpp::mocks {
/// Parse mocks from string
void parse(const std::string &input) noexcept;
} // namespace cdd_cpp::mocks
