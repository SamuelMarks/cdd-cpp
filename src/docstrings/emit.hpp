/// @file emit.hpp
/// @brief Doxygen docstring emitters and multi-line comment formatting
/// utilities.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/parse.hpp"
#include <sstream>
#include <string>

/// @namespace cdd_cpp::docstrings
/// @brief Docstring emitters and parsers for round-tripping OpenAPI and Arazzo
/// metadata in C++ comments.
namespace cdd_cpp::docstrings {

/// @brief Format a potentially multi-line string with a consistent indentation
/// prefix on subsequent lines.
/// @param indent The prefix string to insert after each newline (e.g., "/// ").
/// @param text The raw text to format.
/// @return Formatted multi-line string with carriage returns removed.
inline std::string format_multi(const std::string &indent,
                                const std::string &text) noexcept {
  std::string res;
  for (char c : text) {
    if (c == '\r')
      continue;
    res += c;
    if (c == '\n')
      res += indent;
  }
  return res;
}

/// @brief Emit API-level docstrings (`@title`, `@version`, `@summary`,
/// `@description`, `@servers`, `@securitySchemes`).
/// @param spec The OpenAPI specification model.
/// @return Formatted C++ Doxygen block for the top-level API class or file.
std::string emit_api_docstrings(const openapi::OpenAPI &spec) noexcept;

/// @brief Emit operation-level docstrings (`@summary`, `@description`, `@tags`,
/// `@deprecated`, `@security`, `@param`, `@return`).
/// @param op The OpenAPI Operation model.
/// @return Formatted C++ Doxygen block for an operation method or handler.
std::string emit_operation_docstrings(const openapi::Operation &op) noexcept;

/// @brief Emit path-level docstrings (`@summary`, `@description`).
/// @param pi The OpenAPI PathItem model.
/// @return Formatted C++ Doxygen block for a PathItem.
std::string emit_path_docstrings(const openapi::PathItem &pi) noexcept;

/// @brief Emit Arazzo document-level docstrings (`@arazzo`, `@arazzo_title`,
/// `@arazzo_version`, `@sourceDescription`, `@arazzo_spec_json`).
/// @param spec The Arazzo 1.1.0 specification model.
/// @return Formatted C++ Doxygen block for the `Workflows` class or file.
std::string emit_arazzo_docstrings(const arazzo::Arazzo &spec) noexcept;

/// @brief Emit Arazzo workflow-level docstrings (`@workflowId`, `@summary`,
/// `@description`, `@dependsOn`, `@workflow_json`).
/// @param wf The Arazzo Workflow model.
/// @return Formatted C++ Doxygen block for a workflow method or handler.
std::string emit_workflow_docstrings(const arazzo::Workflow &wf) noexcept;

} // namespace cdd_cpp::docstrings
