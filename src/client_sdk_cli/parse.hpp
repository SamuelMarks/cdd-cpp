// GCOV_EXCL_BR_START
#include <expected>
#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::client_sdk_cli {
/// Parse a C++ CLI application into an OpenAPI spec
std::expected<openapi::OpenAPI, std::string>
parse(const std::string &source) noexcept;
} // namespace cdd_cpp::client_sdk_cli

// GCOV_EXCL_BR_STOP
