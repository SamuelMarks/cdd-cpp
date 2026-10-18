/// @file expressions.hpp
/// @brief Arazzo 1.1.0 ABNF Runtime Expression parser, RFC 6901 JSON Pointer
/// evaluator, payload replacement processor, and Criterion condition evaluator.

#pragma once
#include "../utils/error.hpp"
#include "models.hpp"
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace cdd_cpp::arazzo {

/// @brief Categorized Arazzo 1.1.0 runtime expression kinds.
enum class RuntimeExpressionKind {
  Url,            ///< $url expression.
  Method,         ///< $method expression.
  StatusCode,     ///< $statusCode expression.
  RequestHeader,  ///< $request.header.<name> expression.
  RequestQuery,   ///< $request.query.<name> expression.
  RequestPath,    ///< $request.path.<name> expression.
  RequestBody,    ///< $request.body[#<json-pointer>] expression.
  ResponseHeader, ///< $response.header.<name> expression.
  ResponseQuery,  ///< $response.query.<name> expression.
  ResponsePath,   ///< $response.path.<name> expression.
  ResponseBody,   ///< $response.body[#<json-pointer>] expression.
  Inputs,         ///< $inputs.<name>[#<json-pointer>] expression.
  Outputs,        ///< $outputs.<name>[#<json-pointer>] expression.
  Steps,     ///< $steps.<stepId>.outputs.<name>[#<json-pointer>] expression.
  Workflows, ///< $workflows.<workflowId>.outputs.<name>[#<json-pointer>]
             ///< expression.
  SourceDescriptions, ///< $sourceDescriptions.<name>.(url|workflows.<id>|<operationId>)
                      ///< expression.
  ComponentsParameters,     ///< $components.parameters.<name> expression.
  ComponentsSuccessActions, ///< $components.successActions.<name> expression.
  ComponentsFailureActions, ///< $components.failureActions.<name> expression.
  ComponentsInputs,         ///< $components.inputs.<name> expression.
  Self                      ///< $self expression.
};

/// @brief Parsed representation of an Arazzo 1.1.0 runtime expression.
struct ParsedRuntimeExpression {
  RuntimeExpressionKind kind =
      RuntimeExpressionKind::Self; ///< Categorized runtime expression kind.
  std::string primary_key =
      {}; ///< Primary identifier (stepId, workflowId, sourceDescription name,
          ///< input/output/header/query/path name).
  std::string secondary_key = {}; ///< Secondary identifier (e.g., output key
                                  ///< under $steps.<id>.outputs.<key>).
  std::string json_pointer =
      {}; ///< Optional RFC 6901 JSON Pointer or property path suffix.
  std::string raw = {}; ///< Original raw expression string.

  /// @brief Equality comparison operator.
  /// @param other The other ParsedRuntimeExpression to compare against.
  /// @return True if all fields are equal.
  bool
  operator==(const ParsedRuntimeExpression &other) const noexcept = default;
};

/// @brief Execution context storing workflow state for runtime expression
/// evaluation.
struct WorkflowExecutionContext {
  std::string url = {};      ///< Current request URL ($url).
  std::string method = {};   ///< Current HTTP method ($method).
  int64_t status_code = 200; ///< Last HTTP status code ($statusCode).
  std::map<std::string, std::string> request_headers =
      {}; ///< Current request headers ($request.header.*).
  std::map<std::string, std::string> request_query =
      {}; ///< Current request query parameters ($request.query.*).
  std::map<std::string, std::string> request_path =
      {}; ///< Current request path parameters ($request.path.*).
  openapi::JsonValue request_body =
      nullptr; ///< Current parsed request body ($request.body).
  std::map<std::string, std::string> response_headers =
      {}; ///< Last response headers ($response.header.*).
  std::map<std::string, std::string> response_query =
      {}; ///< Last response query parameters ($response.query.*).
  std::map<std::string, std::string> response_path =
      {}; ///< Last response path parameters ($response.path.*).
  openapi::JsonValue response_body =
      nullptr; ///< Last parsed response body ($response.body).
  std::map<std::string, openapi::JsonValue> inputs =
      {}; ///< Workflow input values ($inputs.*).
  std::map<std::string, openapi::JsonValue> outputs =
      {}; ///< Workflow output values ($outputs.*).
  std::map<std::string, std::map<std::string, openapi::JsonValue>>
      step_outputs = {}; ///< Step outputs indexed by stepId then output name
                         ///< ($steps.<id>.outputs.*).
  std::map<std::string, std::map<std::string, openapi::JsonValue>>
      workflow_outputs = {}; ///< Workflow outputs indexed by workflowId then
                             ///< output name ($workflows.<id>.outputs.*).
  std::map<std::string, std::string> source_description_urls =
      {}; ///< Source description URLs indexed by name
          ///< ($sourceDescriptions.<name>.url).
  std::map<std::string, openapi::JsonValue> component_parameters =
      {}; ///< Reusable component parameter values ($components.parameters.*).
  std::string self_uri = {}; ///< Document self URI ($self).
};

/// @brief Parse and validate a single Arazzo 1.1.0 runtime expression string.
/// @param expr The runtime expression string (must begin with '$').
/// @return Result containing the ParsedRuntimeExpression or an Error.
utils::Result<ParsedRuntimeExpression>
parse_runtime_expression(std::string_view expr) noexcept;

/// @brief Extract and parse all embedded `{$...}` runtime expressions from a
/// string template.
/// @param template_str The template string (or standalone `$...` expression).
/// @return Result containing the vector of parsed runtime expressions or an
/// Error.
utils::Result<std::vector<ParsedRuntimeExpression>>
extract_template_expressions(std::string_view template_str) noexcept;

/// @brief Evaluate an RFC 6901 JSON Pointer against a strongly typed JsonValue.
/// @param root The root JsonValue to navigate.
/// @param pointer The RFC 6901 pointer (e.g., `"/a/b/0"` or `"#/a/b/0"`).
/// @return Result containing the resolved JsonValue or an Error.
utils::Result<openapi::JsonValue>
evaluate_json_pointer(const openapi::JsonValue &root,
                      std::string_view pointer) noexcept;

/// @brief Evaluate a parsed runtime expression against a
/// WorkflowExecutionContext.
/// @param expr The parsed runtime expression.
/// @param ctx The active workflow execution context.
/// @return Result containing the evaluated JsonValue or an Error.
utils::Result<openapi::JsonValue>
evaluate_expression(const ParsedRuntimeExpression &expr,
                    const WorkflowExecutionContext &ctx) noexcept;

/// @brief Interpolate embedded `{$...}` or standalone `$...` expressions into a
/// string.
/// @param tmpl The input template string.
/// @param ctx The active workflow execution context.
/// @return Result containing the interpolated string or an Error.
utils::Result<std::string>
interpolate_template(std::string_view tmpl,
                     const WorkflowExecutionContext &ctx) noexcept;

/// @brief Apply a PayloadReplacement rule to a JSON payload.
/// @param payload The base JSON payload to transform.
/// @param replacement The PayloadReplacement specifying target pointer and
/// value.
/// @param ctx The active workflow execution context.
/// @return Result containing the updated JsonValue payload or an Error.
utils::Result<openapi::JsonValue>
apply_payload_replacement(const openapi::JsonValue &payload,
                          const PayloadReplacement &replacement,
                          const WorkflowExecutionContext &ctx) noexcept;

/// @brief Evaluate an Arazzo Criterion against a WorkflowExecutionContext.
/// @param criterion The Criterion object to evaluate.
/// @param ctx The active workflow execution context.
/// @return Result containing true if the criterion passes, false if it fails,
/// or an Error if malformed.
utils::Result<bool>
evaluate_criterion(const Criterion &criterion,
                   const WorkflowExecutionContext &ctx) noexcept;

} // namespace cdd_cpp::arazzo
