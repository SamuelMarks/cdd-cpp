/// @file emit.hpp
/// @brief GoogleMock C++ mock client emitter declarations.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include <string>

/// @namespace cdd_cpp::mocks
/// @brief GoogleMock interface and mock client generation and reverse-parsing.
namespace cdd_cpp::mocks {

/// @brief Emit a GoogleMock `IClient` and `MockClient` C++ header from an
/// OpenAPI spec.
/// @param spec The OpenAPI specification model.
/// @return Generated C++ header containing `IClient` and `MockClient`.
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// @brief Emit a GoogleMock `IWorkflows` and `MockWorkflows` C++ header from an
/// Arazzo spec.
/// @param spec The Arazzo 1.1.0 specification model.
/// @return Generated C++ header containing `IWorkflows` and `MockWorkflows`.
std::string emit_arazzo(const arazzo::Arazzo &spec) noexcept;

} // namespace cdd_cpp::mocks
