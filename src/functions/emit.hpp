/// @file emit.hpp
/// @brief Standalone C++ function declarations emitter from OpenAPI operations.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include <string>

/// @namespace cdd_cpp::functions
/// @brief C++ standalone function signature generation and parsing for OpenAPI
/// and Arazzo specs.
namespace cdd_cpp::functions {

/// @brief Emit standalone C++ function declarations from an OpenAPI
/// specification.
/// @param spec Optional OpenAPI specification model (defaults to empty spec).
/// @return Generated C++ function declarations string (or "{}" when empty for
/// legacy compatibility).
std::string emit(const openapi::OpenAPI &spec = openapi::OpenAPI{}) noexcept;

/// @brief Emit standalone C++ workflow function declarations from an Arazzo
/// specification.
/// @param spec The Arazzo 1.1.0 specification model.
/// @return Generated C++ workflow function declarations string.
std::string emit_arazzo(const arazzo::Arazzo &spec) noexcept;

} // namespace cdd_cpp::functions
