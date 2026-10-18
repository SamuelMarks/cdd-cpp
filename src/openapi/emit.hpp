/// @file emit.hpp
/// @brief OpenAPI 3.2.0 JSON emitter declarations.

#pragma once
#include "../utils/json_writer.hpp"
#include "models.hpp"
#include <string>

namespace cdd_cpp::openapi {

/// @brief Emit an OpenAPI Schema model to a JsonWriter.
/// @param jw Destination JsonWriter.
/// @param obj The Schema object to serialize.
void emit_Schema(utils::JsonWriter &jw, const Schema &obj) noexcept;

/// @brief Emit an OpenAPI model to a serialized JSON string.
/// @param openapi The OpenAPI document model to serialize.
/// @return Compact RFC 8259 JSON string representing the OpenAPI document.
std::string emit(const OpenAPI &openapi) noexcept;

} // namespace cdd_cpp::openapi
