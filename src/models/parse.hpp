/// @file parse.hpp
/// @brief C++ AST to OpenAPI schema parser declarations.

#pragma once
#include "../openapi/models.hpp"
#include "../utils/cpp_ast.hpp"

namespace cdd_cpp::models {

/// @brief Parses a CppAST and populates OpenAPI component schemas.
/// @param ast The parsed C++ AST containing struct/class definitions.
/// @param spec The OpenAPI specification object to mutate with extracted
/// schemas.
void parse_schemas(const utils::CppAST &ast, openapi::OpenAPI &spec) noexcept;

} // namespace cdd_cpp::models

namespace cdd_cpp::utils {
using models::parse_schemas;
} // namespace cdd_cpp::utils
