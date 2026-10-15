/// @file parse.hpp
/// @brief C++ CLI client SDK application parser declarations.

#pragma once
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>

namespace cdd_cpp::client_sdk_cli {

/// @brief Standard Result alias for client_sdk_cli operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse a C++ CLI application source into an OpenAPI spec.
/// @param source The C++ CLI application source code.
/// @return Result containing the extracted OpenAPI specification or a typed
/// Error.
Result<openapi::OpenAPI> parse(const std::string &source) noexcept;

} // namespace cdd_cpp::client_sdk_cli
