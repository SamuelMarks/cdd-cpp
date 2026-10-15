/// @file emit.hpp
/// @brief C++ client SDK code generator declarations.

#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

/// @namespace cdd_cpp::client_sdk
/// @brief Client SDK code generation and reverse-parsing from C++ to OpenAPI.
namespace cdd_cpp::client_sdk {

/// @brief Emit a C++ client SDK containing libcurl and simdjson wrappers based
/// on the OpenAPI spec.
/// @param spec The OpenAPI specification model.
/// @param no_github_actions If true, omits `.github/workflows/ci.yml` from
/// generated output.
/// @param no_installable_package If true, omits `CMakeLists.txt` package
/// configuration.
/// @param tests If true, emits GoogleTest unit test suite under `tests/`.
/// @return A map of relative filename to generated file content.
std::map<std::string, std::string>
emit_client(const openapi::OpenAPI &spec, bool no_github_actions = false,
            bool no_installable_package = false, bool tests = false) noexcept;

} // namespace cdd_cpp::client_sdk
