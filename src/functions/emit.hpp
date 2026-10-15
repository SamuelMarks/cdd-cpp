/// @file emit.hpp
/// @brief Standalone C++ function declarations emitter from OpenAPI operations.

#pragma once
#include "../openapi/models.hpp"
#include <string>

/// @namespace cdd_cpp::functions
/// @brief C++ standalone function signature generation and parsing for OpenAPI
/// specs.
namespace cdd_cpp::functions {

/// @brief Emit standalone C++ function declarations from an OpenAPI
/// specification.
/// @param spec Optional OpenAPI specification model (defaults to empty spec).
/// @return Generated C++ function declarations string (or "{}" when empty for
/// legacy compatibility).
std::string emit(const openapi::OpenAPI &spec = openapi::OpenAPI{}) noexcept;

} // namespace cdd_cpp::functions
