/// @file emit.hpp
/// @brief GoogleMock C++ mock client emitter declarations.

#pragma once
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

} // namespace cdd_cpp::mocks
