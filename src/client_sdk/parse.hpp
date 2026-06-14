// GCOV_EXCL_BR_START
#include <expected>
#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::client_sdk {
/// Parse a C++ Client SDK and return an OpenAPI specification
std::expected<openapi::OpenAPI, std::string>
parse(const std::string &input) noexcept;
} // namespace cdd_cpp::client_sdk

// GCOV_EXCL_BR_STOP
