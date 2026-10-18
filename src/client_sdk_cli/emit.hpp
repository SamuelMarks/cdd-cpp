/// @file emit.hpp
/// @brief C++ CLI client SDK application code generator declarations.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include <map>
#include <optional>
#include <string>

/// @namespace cdd_cpp::client_sdk_cli
/// @brief CLI application code generation and reverse-parsing for OpenAPI and
/// Arazzo specs.
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

/// @brief Emit a C++ CLI application reusing the Client SDK `Workflows`
/// orchestrator from an Arazzo 1.1.0 specification.
/// @param spec The Arazzo 1.1.0 specification model.
/// @param openapi_spec Optional OpenAPI specification model to co-emit.
/// @param no_github_actions If true, omits `.github/workflows/ci.yml`.
/// @param no_installable_package If true, omits `CMakeLists.txt`.
/// @param tests If true, emits GoogleTest CLI workflow test suite under
/// `tests/`.
/// @return A map of relative filename to generated file content.
std::map<std::string, std::string> emit_arazzo_cli(
    const arazzo::Arazzo &spec,
    const std::optional<openapi::OpenAPI> &openapi_spec = std::nullopt,
    bool no_github_actions = false, bool no_installable_package = false,
    bool tests = false) noexcept;

} // namespace cdd_cpp::client_sdk_cli
