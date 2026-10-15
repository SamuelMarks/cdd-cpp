/// @file emit.hpp
/// @brief OpenAPI 3.2.0 JSON emitter declarations.

#pragma once
#include "models.hpp"
#include <string>

namespace cdd_cpp::openapi {

/// @brief Emit an OpenAPI model to a serialized JSON string.
/// @param openapi The OpenAPI document model to serialize.
/// @return Compact RFC 8259 JSON string representing the OpenAPI document.
std::string emit(const OpenAPI &openapi) noexcept;

} // namespace cdd_cpp::openapi
