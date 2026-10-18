/// @file cpp_parser.hpp
/// @brief Directory-level C++ project scanner and OpenAPI extractor
/// declarations.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include "error.hpp"
#include <string>

namespace cdd_cpp::utils {

/// @brief Recursively parse a C++ project directory and return a Result
/// containing a combined OpenAPI specification or a typed Error.
/// @param folder_path Path to the root C++ directory containing
/// `.hpp`/`.cpp`/`.h`/`.c` files.
/// @return Result containing the extracted OpenAPI 3.2.0 specification or an
/// Error.
Result<openapi::OpenAPI>
try_parse_cpp_project(const std::string &folder_path) noexcept;

/// @brief Recursively parse a C++ project directory and generate a combined
/// OpenAPI specification.
/// @param folder_path Path to the root C++ directory containing
/// `.hpp`/`.cpp`/`.h`/`.c` files.
/// @return Extracted OpenAPI 3.2.0 specification containing schemas and paths.
openapi::OpenAPI parse_cpp_project(const std::string &folder_path) noexcept;

/// @brief Recursively parse a C++ project directory and return a Result
/// containing a combined Arazzo 1.1.0 specification or a typed Error.
/// @param folder_path Path to the root C++ directory containing
/// `.hpp`/`.cpp`/`.h`/`.c` files.
/// @return Result containing the extracted Arazzo 1.1.0 specification or an
/// Error.
Result<arazzo::Arazzo>
try_parse_cpp_project_arazzo(const std::string &folder_path) noexcept;

/// @brief Recursively parse a C++ project directory and generate a combined
/// Arazzo 1.1.0 specification.
/// @param folder_path Path to the root C++ directory containing
/// `.hpp`/`.cpp`/`.h`/`.c` files.
/// @return Extracted Arazzo 1.1.0 specification containing workflows.
arazzo::Arazzo
parse_cpp_project_arazzo(const std::string &folder_path) noexcept;

} // namespace cdd_cpp::utils
