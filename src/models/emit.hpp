/// @file emit.hpp
/// @brief C++ data model (struct) emitter declarations.

#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

/// @namespace cdd_cpp::models
/// @brief C++ struct generation and schema parsing for OpenAPI components.
namespace cdd_cpp::models {

/// @brief Emit C++ models (structs) to a single string from an OpenAPI spec.
/// @param spec The OpenAPI specification model.
/// @return Generated C++ struct definitions and JSON serialization helpers.
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// @brief Emit modular C++ models (structs) to a map of header files.
/// @param spec The OpenAPI specification model.
/// @return Map of `src/models/<SchemaName>.hpp` to generated C++ header
/// content.
std::map<std::string, std::string>
emit_modular(const openapi::OpenAPI &spec) noexcept;

} // namespace cdd_cpp::models
