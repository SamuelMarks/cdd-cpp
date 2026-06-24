#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

namespace cdd_cpp::server {
/// Emit C++ server routes to string (Legacy)
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// Emit modular C++ server files (Routes + Main + CMake)
std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool no_installable_package,
                                                bool tests, bool with_postgres,
                                                bool with_faker) noexcept;

/// Serve JSON-RPC request
std::string serve_json_rpc(const std::string &request) noexcept;
} // namespace cdd_cpp::server
