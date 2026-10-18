/// @file emit.hpp
/// @brief Arazzo Specification 1.1.0 JSON emitter declarations.

#pragma once
#include "../utils/json_writer.hpp"
#include "models.hpp"
#include <string>

namespace cdd_cpp::arazzo {

/// @brief Emit a strongly typed JsonValue to a JsonWriter.
/// @param jw Destination JsonWriter.
/// @param val The JsonValue to serialize.
void emit_json_value(utils::JsonWriter &jw,
                     const openapi::JsonValue &val) noexcept;

/// @brief Emit an Arazzo 1.1.0 model to a serialized JSON string.
/// @param spec The Arazzo document model to serialize.
/// @return Compact RFC 8259 JSON string representing the Arazzo document.
std::string emit(const Arazzo &spec) noexcept;

} // namespace cdd_cpp::arazzo
