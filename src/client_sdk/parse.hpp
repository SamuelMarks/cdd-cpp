/// @file parse.hpp
/// @brief C++ client SDK parser declarations.

#pragma once
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>

namespace cdd_cpp::client_sdk {

/// @brief Standard Result alias for client_sdk operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse a C++ Client SDK source string and return an OpenAPI
/// specification.
/// @param input The C++ client SDK header/source code to parse.
/// @return Result containing the extracted OpenAPI specification or a typed
/// Error.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept;

} // namespace cdd_cpp::client_sdk
