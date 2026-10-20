/// @file type_mapping.hpp
/// @brief Unified C++, OpenAPI 3.2.0, and PostgreSQL SQL type mapping
/// utilities.

#pragma once
#include <string>
#include <string_view>

namespace cdd_cpp::utils {

/// @brief Convert an OpenAPI primitive type string to its corresponding C++
/// type name.
/// @param oas_type The OpenAPI type string ("integer", "number", "boolean",
/// "string").
/// @return C++ type string ("int", "double", "bool", or "std::string").
inline std::string openapi_type_to_cpp(std::string_view oas_type) noexcept {
  if (oas_type == "integer")
    return "int";
  if (oas_type == "number")
    return "double";
  if (oas_type == "boolean")
    return "bool";
  return "std::string";
}

/// @brief Convert a C++ primitive type string to its corresponding OpenAPI
/// primitive type name.
/// @param cpp_type The C++ type string ("int", "int32_t", "int64_t", "long",
/// "double", "float", "bool", "std::string").
/// @return OpenAPI type string ("integer", "number", "boolean", or "string").
inline std::string cpp_type_to_openapi(std::string_view cpp_type) noexcept {
  while (!cpp_type.empty() &&
         (cpp_type.front() == ' ' || cpp_type.front() == '\t'))
    cpp_type.remove_prefix(1);
  while (!cpp_type.empty() &&
         (cpp_type.back() == ' ' || cpp_type.back() == '\t' ||
          cpp_type.back() == '&'))
    cpp_type.remove_suffix(1);
  if (cpp_type.starts_with("const "))
    cpp_type.remove_prefix(6);
  while (!cpp_type.empty() && cpp_type.front() == ' ')
    cpp_type.remove_prefix(1);

  if (cpp_type == "int" || cpp_type == "int32_t" || cpp_type == "int64_t" ||
      cpp_type == "long" || cpp_type == "short")
    return "integer";
  if (cpp_type == "double" || cpp_type == "float")
    return "number";
  if (cpp_type == "bool" || cpp_type == "boolean")
    return "boolean";
  return "string";
}

/// @brief Map an OpenAPI schema type to its PostgreSQL DDL column type.
/// @param oas_type The OpenAPI primitive type string ("integer", "number",
/// "boolean", "string").
/// @return SQL column type string ("INTEGER", "DOUBLE PRECISION", "BOOLEAN",
/// "TEXT").
inline std::string openapi_type_to_sql(std::string_view oas_type) noexcept {
  if (oas_type == "integer")
    return "INTEGER";
  if (oas_type == "number")
    return "DOUBLE PRECISION";
  if (oas_type == "boolean")
    return "BOOLEAN";
  return "TEXT";
}

} // namespace cdd_cpp::utils
