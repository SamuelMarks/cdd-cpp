#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::client_sdk_cli {
/// Emit a C++ CLI application based on the OpenAPI spec
std::string emit_cli(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::client_sdk_cli
