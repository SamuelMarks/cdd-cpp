/// @file parse.hpp
/// @brief Doxygen docstring parsers for extracting OpenAPI metadata from C++
/// comments.

#pragma once
#include "../arazzo/models.hpp"
#include "../openapi/models.hpp"
#include <optional>
#include <string>
#include <string_view>

namespace cdd_cpp::docstrings {

/// @brief Strip leading whitespace and comment markers (`*`, `///`, `//`) from
/// a single docstring line.
/// @param line Raw docstring line.
/// @return Cleaned line content with trailing `\r`/whitespace trimmed.
std::string strip_comment_line_prefix(std::string_view line) noexcept;

/// @brief Extract the value of a `@<tag> <value>` annotation from a docstring.
/// @param docstring The Doxygen docstring block to search.
/// @param tag The tag name without the leading `@` (e.g., "title", "summary").
/// @return Optional extracted value with trailing whitespace trimmed.
std::optional<std::string> extract_tag(const std::string &docstring,
                                       const std::string &tag) noexcept;

/// @brief Parse API-level metadata (`@title`, `@version`, `@servers`,
/// `@securitySchemes`, etc.) into an OpenAPI object.
/// @param docstring The top-level or class-level Doxygen docstring.
/// @param spec The OpenAPI specification object to populate.
void parse_api_docstrings(const std::string &docstring,
                          openapi::OpenAPI &spec) noexcept;

/// @brief Parse operation-level metadata (`@summary`, `@description`, `@tags`,
/// `@deprecated`, `@security`, `@response_content`, `@response`, `@return`)
/// into an Operation object.
/// @param docstring The function/method Doxygen docstring.
/// @param op The OpenAPI Operation object to populate.
void parse_operation_docstrings(const std::string &docstring,
                                openapi::Operation &op) noexcept;

/// @brief Parse path-level metadata (`@summary`, `@description`) into a
/// PathItem object.
/// @param docstring The path-level Doxygen docstring.
/// @param pi The OpenAPI PathItem object to populate.
void parse_path_docstrings(const std::string &docstring,
                           openapi::PathItem &pi) noexcept;

/// @brief Parse schema/field-level metadata into an OpenAPI Schema object.
/// @param docstring The struct or field Doxygen docstring.
/// @param schema The OpenAPI Schema object to populate.
void parse_schema_docstrings(const std::string &docstring,
                             openapi::Schema &schema) noexcept;

/// @brief Parse Arazzo document-level metadata (`@arazzo`, `@arazzo_title`,
/// `@arazzo_version`, `@sourceDescription`, `@arazzo_spec_json`) into an Arazzo
/// object.
/// @param docstring The top-level or class-level Doxygen docstring.
/// @param spec The Arazzo specification object to populate.
void parse_arazzo_docstrings(const std::string &docstring,
                             arazzo::Arazzo &spec) noexcept;

/// @brief Parse Arazzo workflow-level metadata (`@workflowId`, `@summary`,
/// `@description`, `@dependsOn`, `@workflow_json`) into a Workflow object.
/// @param docstring The workflow method or function Doxygen docstring.
/// @param wf The Arazzo Workflow object to populate.
void parse_workflow_docstrings(const std::string &docstring,
                               arazzo::Workflow &wf) noexcept;

/// @brief Parse an arbitrary docstring and return an OpenAPI object populated
/// with any extracted metadata.
/// @param input The raw docstring or C++ comment block.
/// @return OpenAPI model containing the extracted metadata.
openapi::OpenAPI parse(const std::string &input) noexcept;

} // namespace cdd_cpp::docstrings
