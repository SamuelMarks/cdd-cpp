/// @file parse.hpp
/// @brief Standalone C++ function declarations parser into OpenAPI.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>

namespace cdd_cpp::functions {

/// @brief Standard Result alias for functions operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse standalone C++ function declarations into an OpenAPI
/// specification.
/// @param input The C++ function declarations source code.
/// @return Result containing the extracted OpenAPI specification or a typed
/// Error.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept;

/// @brief Parse standalone C++ workflow function declarations into an Arazzo
/// specification.
/// @param input The C++ workflow function declarations source code.
/// @return Result containing the extracted Arazzo specification or a typed
/// Error.
Result<arazzo::Arazzo> parse_arazzo(const std::string &input) noexcept;

} // namespace cdd_cpp::functions
