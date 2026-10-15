/// @file emit.hpp
/// @brief C++ HTTP server and JSON-RPC code generator declarations.

#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

/// @namespace cdd_cpp::server
/// @brief HTTP server route generation, reverse-parsing, and JSON-RPC serving.
namespace cdd_cpp::server {

/// @brief Emit C++ server routes to string (Legacy single-file format).
/// @param spec The OpenAPI specification model.
/// @return Generated C++ server header/source string.
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// @brief Emit modular C++ server files (Routes + Main + CMake + Tests).
/// @param spec The OpenAPI specification model.
/// @param no_installable_package If true, omits `CMakeLists.txt`.
/// @param tests If true, emits GoogleTest unit tests under `tests/`.
/// @param with_postgres If true, wires PostgreSQL libpq connection and DAOs.
/// @param with_faker If true, enables Faker data generation in stub DAOs.
/// @return Map of relative file paths to generated file contents.
std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool no_installable_package,
                                                bool tests, bool with_postgres,
                                                bool with_faker) noexcept;

/// @brief Serve a single JSON-RPC 2.0 request over `cdd-cpp` core generators.
/// @param request Raw JSON-RPC 2.0 request payload string.
/// @return Serialized JSON-RPC 2.0 response payload string.
std::string serve_json_rpc(const std::string &request) noexcept;

} // namespace cdd_cpp::server
