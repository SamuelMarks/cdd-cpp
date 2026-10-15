/// @file parse.hpp
/// @brief OpenAPI 3.2.0 JSON parser declarations.

#pragma once
#include "../utils/error.hpp"
#include "models.hpp"
#include <simdjson.h>
#include <string>

namespace cdd_cpp::openapi {

/// @brief Standard Result alias for OpenAPI operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse an OpenAPI specification from a JSON string.
/// @param input The raw JSON string representing an OpenAPI document.
/// @return Result containing the parsed OpenAPI model or a typed Error.
Result<OpenAPI> parse(const std::string &input) noexcept;

} // namespace cdd_cpp::openapi
