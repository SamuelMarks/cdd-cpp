/// @file upgrader.hpp
/// @brief OpenAPI / Swagger specification version detector and upgrader
/// to 3.2.0.

#pragma once
#include "../../utils/error.hpp"
#include <simdjson.h>
#include <string>

/// @namespace cdd_cpp::openapi::upgraders
/// @brief Version upgraders transforming Swagger 1.2, 2.0, and OpenAPI 3.0/3.1
/// into 3.2.0.
namespace cdd_cpp::openapi::upgraders {

/// @brief Standard Result alias for upgrader operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Detects the OpenAPI/Swagger version from a JSON string and upgrades
/// it to OpenAPI 3.2.0.
/// @param json_spec The raw input JSON specification string.
/// @return Result containing the upgraded OpenAPI 3.2.0 JSON string or a typed
/// Error.
Result<std::string> upgrade_to_latest(const std::string &json_spec) noexcept;

} // namespace cdd_cpp::openapi::upgraders
