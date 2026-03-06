#pragma once
#include "../openapi/models.hpp"
#include "../utils/cpp_ast.hpp"

namespace cdd_cpp::utils {
    /// @brief Parses a CppAST and generates OpenAPI schemas.
/// @param ast The parsed C++ AST.
/// @param spec The OpenAPI specification object to mutate.
void parse_schemas(const CppAST& ast, openapi::OpenAPI& spec) noexcept;
}
