#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::client_sdk {
/// Emit a C++ client SDK containing libcurl and simdjson wrappers based on the
/// OpenAPI spec
std::string emit_client(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::client_sdk
