/// @file emit.hpp
/// @brief C++ CLI client SDK application code generator declarations.

#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

/// @namespace cdd_cpp::client_sdk_cli
/// @brief CLI application code generation and reverse-parsing for OpenAPI
/// specs.
namespace cdd_cpp::client_sdk_cli {

/// @brief Emit a C++ CLI application based on the OpenAPI spec.
/// @param spec The OpenAPI specification model.
/// @param no_github_actions If true, omits `.github/workflows/ci.yml`.
/// @param no_installable_package If true, omits `CMakeLists.txt`.
/// @param tests If true, emits GoogleTest CLI test suite under `tests/`.
/// @return A map of relative filename to generated file content.
std::map<std::string, std::string> emit_cli(const openapi::OpenAPI &spec,
                                            bool no_github_actions = false,
                                            bool no_installable_package = false,
                                            bool tests = false) noexcept;

} // namespace cdd_cpp::client_sdk_cli
