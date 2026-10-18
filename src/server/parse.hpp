/// @file parse.hpp
/// @brief C++ HTTP server route parser declarations.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>

namespace cdd_cpp::server {

/// @brief Standard Result alias for server operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse routes and schemas from C++ server source code into an OpenAPI
/// spec.
/// @param input The C++ server source code to parse.
/// @return Result containing the parsed OpenAPI specification or a typed Error.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept;

/// @brief Parse workflow orchestration endpoints from C++ server source code
/// into an Arazzo 1.1.0 spec.
/// @param input The C++ server workflow source code to parse.
/// @return Result containing the parsed Arazzo specification or a typed Error.
Result<arazzo::Arazzo> parse_arazzo(const std::string &input) noexcept;

} // namespace cdd_cpp::server
