/// @file expressions.cpp
/// @brief Implementation of Arazzo 1.1.0 ABNF Runtime Expression parser and
/// evaluator without exceptions (-fno-exceptions -fno-rtti).

#include "expressions.hpp"
#include <cctype>
#include <charconv>
#include <sstream>

namespace cdd_cpp::arazzo {

/// @brief Validate RFC 6901 JSON Pointer syntax (`~` must be followed by `0` or
/// `1`).
/// @param ptr The JSON Pointer string view.
/// @return True if valid, false if malformed escape sequence found.
static bool is_valid_json_pointer(std::string_view ptr) noexcept {
  if (ptr.empty())
    return true;
  if (ptr.front() != '/')
    return false;
  for (size_t i = 0; i < ptr.size(); ++i) {
    if (ptr[i] == '~') {
      if (i + 1 >= ptr.size() || (ptr[i + 1] != '0' && ptr[i + 1] != '1')) {
        return false;
      }
      ++i;
    }
  }
  return true;
}

/// @brief Unescape a single RFC 6901 JSON Pointer reference token (`~1` -> `/`,
/// `~0` -> `~`).
/// @param token The raw reference token.
/// @return Unescaped token string.
static std::string unescape_pointer_token(std::string_view token) noexcept {
  std::string out;
  out.reserve(token.size());
  for (size_t i = 0; i < token.size(); ++i) {
    if (token[i] == '~') {
      out.push_back(token[i + 1] == '1' ? '/' : '~');
      ++i;
      continue;
    }
    out.push_back(token[i]);
  }
  return out;
}

/// @brief Split a suffix into primary identifier and optional JSON Pointer
/// (`#...` or `.prop`).
/// @param suffix The raw suffix after a prefix like `$inputs.`.
/// @param out_key Destination primary identifier.
/// @param out_ptr Destination normalized JSON Pointer (starting with `/`).
/// @return True if valid, false if empty key or invalid pointer.
static bool split_key_and_pointer(std::string_view suffix, std::string &out_key,
                                  std::string &out_ptr) noexcept {
  if (suffix.empty())
    return false;
  auto hash_pos = suffix.find('#');
  if (hash_pos != std::string_view::npos) {
    out_key = std::string(suffix.substr(0, hash_pos));
    out_ptr = std::string(suffix.substr(hash_pos + 1));
    return !out_key.empty() && is_valid_json_pointer(out_ptr);
  }
  auto dot_pos = suffix.find('.');
  if (dot_pos != std::string_view::npos) {
    out_key = std::string(suffix.substr(0, dot_pos));
    std::string_view rest = suffix.substr(dot_pos + 1);
    if (rest.empty())
      return false;
    out_ptr = "/";
    for (char c : rest) {
      out_ptr.push_back(c == '.' ? '/' : c);
    }
    return true;
  }
  out_key = std::string(suffix);
  out_ptr.clear();
  return true;
}

utils::Result<ParsedRuntimeExpression>
parse_runtime_expression(std::string_view expr) noexcept {
  if (expr.empty() || expr.front() != '$') {
    return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                             "Runtime expression must start with '$'",
                             std::string(expr));
  }

  ParsedRuntimeExpression res;
  res.raw = std::string(expr);

  if (expr == "$url") {
    res.kind = RuntimeExpressionKind::Url;
    return res;
  }
  if (expr == "$method") {
    res.kind = RuntimeExpressionKind::Method;
    return res;
  }
  if (expr == "$statusCode") {
    res.kind = RuntimeExpressionKind::StatusCode;
    return res;
  }
  if (expr == "$self") {
    res.kind = RuntimeExpressionKind::Self;
    return res;
  }

  if (expr.starts_with("$request.header.")) {
    res.kind = RuntimeExpressionKind::RequestHeader;
    res.primary_key = std::string(expr.substr(16));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty header name in $request.header.",
                               std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$request.query.")) {
    res.kind = RuntimeExpressionKind::RequestQuery;
    res.primary_key = std::string(expr.substr(15));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty query name in $request.query.",
                               std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$request.path.")) {
    res.kind = RuntimeExpressionKind::RequestPath;
    res.primary_key = std::string(expr.substr(14));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty path name in $request.path.",
                               std::string(expr));
    }
    return res;
  }
  if (expr == "$request.body" || expr.starts_with("$request.body#")) {
    res.kind = RuntimeExpressionKind::RequestBody;
    if (expr.size() > 14) {
      res.json_pointer = std::string(expr.substr(14));
      if (!is_valid_json_pointer(res.json_pointer)) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "Invalid JSON Pointer in $request.body",
                                 std::string(expr));
      }
    }
    return res;
  }

  if (expr.starts_with("$response.header.")) {
    res.kind = RuntimeExpressionKind::ResponseHeader;
    res.primary_key = std::string(expr.substr(17));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty header name in $response.header.",
                               std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$response.query.")) {
    res.kind = RuntimeExpressionKind::ResponseQuery;
    res.primary_key = std::string(expr.substr(16));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty query name in $response.query.",
                               std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$response.path.")) {
    res.kind = RuntimeExpressionKind::ResponsePath;
    res.primary_key = std::string(expr.substr(15));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty path name in $response.path.",
                               std::string(expr));
    }
    return res;
  }
  if (expr == "$response.body" || expr.starts_with("$response.body#")) {
    res.kind = RuntimeExpressionKind::ResponseBody;
    if (expr.size() > 15) {
      res.json_pointer = std::string(expr.substr(15));
      if (!is_valid_json_pointer(res.json_pointer)) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "Invalid JSON Pointer in $response.body",
                                 std::string(expr));
      }
    }
    return res;
  }

  if (expr.starts_with("$inputs.")) {
    res.kind = RuntimeExpressionKind::Inputs;
    if (!split_key_and_pointer(expr.substr(8), res.primary_key,
                               res.json_pointer)) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Malformed $inputs runtime expression",
                               std::string(expr));
    }
    return res;
  }

  if (expr.starts_with("$outputs.")) {
    res.kind = RuntimeExpressionKind::Outputs;
    if (!split_key_and_pointer(expr.substr(9), res.primary_key,
                               res.json_pointer)) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Malformed $outputs runtime expression",
                               std::string(expr));
    }
    return res;
  }

  if (expr.starts_with("$steps.")) {
    res.kind = RuntimeExpressionKind::Steps;
    std::string_view rest = expr.substr(7);
    auto dot_pos = rest.find(".outputs.");
    if (dot_pos == std::string_view::npos || dot_pos == 0) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "$steps expression must follow $steps.<stepId>.outputs.<name>",
          std::string(expr));
    }
    res.primary_key = std::string(rest.substr(0, dot_pos));
    if (!split_key_and_pointer(rest.substr(dot_pos + 9), res.secondary_key,
                               res.json_pointer)) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Malformed output key in $steps expression",
                               std::string(expr));
    }
    return res;
  }

  if (expr.starts_with("$workflows.")) {
    res.kind = RuntimeExpressionKind::Workflows;
    std::string_view rest = expr.substr(11);
    auto dot_pos = rest.find(".outputs.");
    if (dot_pos == std::string_view::npos) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "$workflows expression must follow $workflows.<id>.outputs.<name>",
          std::string(expr));
    }
    res.primary_key = std::string(rest.substr(0, dot_pos));
    if (!split_key_and_pointer(rest.substr(dot_pos + 9), res.secondary_key,
                               res.json_pointer)) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Malformed output key in $workflows expression",
                               std::string(expr));
    }
    return res;
  }

  if (expr.starts_with("$sourceDescriptions.")) {
    res.kind = RuntimeExpressionKind::SourceDescriptions;
    std::string_view rest = expr.substr(20);
    auto dot_pos = rest.find('.');
    if (dot_pos == std::string_view::npos || dot_pos + 1 >= rest.size()) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "Malformed $sourceDescriptions runtime expression",
          std::string(expr));
    }
    res.primary_key = std::string(rest.substr(0, dot_pos));
    res.secondary_key = std::string(rest.substr(dot_pos + 1));
    return res;
  }

  if (expr.starts_with("$components.parameters.")) {
    res.kind = RuntimeExpressionKind::ComponentsParameters;
    res.primary_key = std::string(expr.substr(23));
    if (res.primary_key.empty()) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "Empty parameter name in $components.parameters.", std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$components.successActions.")) {
    res.kind = RuntimeExpressionKind::ComponentsSuccessActions;
    res.primary_key = std::string(expr.substr(27));
    if (res.primary_key.empty()) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "Empty action name in $components.successActions.",
          std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$components.failureActions.")) {
    res.kind = RuntimeExpressionKind::ComponentsFailureActions;
    res.primary_key = std::string(expr.substr(27));
    if (res.primary_key.empty()) {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "Empty action name in $components.failureActions.",
          std::string(expr));
    }
    return res;
  }
  if (expr.starts_with("$components.inputs.")) {
    res.kind = RuntimeExpressionKind::ComponentsInputs;
    res.primary_key = std::string(expr.substr(19));
    if (res.primary_key.empty()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Empty input name in $components.inputs.",
                               std::string(expr));
    }
    return res;
  }

  return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                           "Unrecognized Arazzo runtime expression",
                           std::string(expr));
}

utils::Result<std::vector<ParsedRuntimeExpression>>
extract_template_expressions(std::string_view template_str) noexcept {
  std::vector<ParsedRuntimeExpression> out;
  if (template_str.starts_with('$')) {
    auto single = parse_runtime_expression(template_str);
    if (!single.has_value()) {
      return std::unexpected(single.error());
    }
    out.push_back(*single);
    return out;
  }

  size_t pos = 0;
  while (pos < template_str.size()) {
    auto open_brace = template_str.find('{', pos);
    if (open_brace == std::string_view::npos) {
      break;
    }
    auto close_brace = template_str.find('}', open_brace + 1);
    if (close_brace == std::string_view::npos) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Unclosed '{' in runtime expression template",
                               std::string(template_str));
    }
    std::string_view inner =
        template_str.substr(open_brace + 1, close_brace - open_brace - 1);
    if (inner.starts_with('$')) {
      auto parsed = parse_runtime_expression(inner);
      if (!parsed.has_value()) {
        return std::unexpected(parsed.error());
      }
      out.push_back(*parsed);
    }
    pos = close_brace + 1;
  }
  return out;
}

utils::Result<openapi::JsonValue>
evaluate_json_pointer(const openapi::JsonValue &root,
                      std::string_view pointer) noexcept {
  if (pointer.starts_with('#')) {
    pointer.remove_prefix(1);
  }
  if (pointer.empty()) {
    return root;
  }
  if (!is_valid_json_pointer(pointer)) {
    return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                             "Invalid RFC 6901 JSON Pointer syntax",
                             std::string(pointer));
  }

  const openapi::JsonValue *curr = &root;
  size_t pos = 1; // skip leading '/'
  for (;;) {
    auto next_slash = pointer.find('/', pos);
    std::string_view raw_token = (next_slash == std::string_view::npos)
                                     ? pointer.substr(pos)
                                     : pointer.substr(pos, next_slash - pos);
    std::string token = unescape_pointer_token(raw_token);

    if (const auto *obj =
            std::get_if<std::map<std::string, openapi::JsonValue>>(
                &curr->data)) {
      if (!obj->contains(token)) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "JSON Pointer key not found: " + token,
                                 std::string(pointer));
      }
      curr = &obj->find(token)->second;
    } else if (const auto *arr =
                   std::get_if<std::vector<openapi::JsonValue>>(&curr->data)) {
      size_t idx = 0;
      (void)std::from_chars(token.data(), token.data() + token.size(), idx);
      if (idx >= arr->size()) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "JSON Pointer array index out of bounds: " +
                                     token,
                                 std::string(pointer));
      }
      curr = &(*arr)[idx];
    } else {
      return utils::make_error(
          utils::ErrorCode::InvalidRuntimeExpression,
          "Cannot dereference scalar JsonValue with JSON Pointer",
          std::string(pointer));
    }

    if (next_slash == std::string_view::npos) {
      break;
    }
    pos = next_slash + 1;
  }
  return *curr;
}

/// @brief Convert a scalar JsonValue to string for template interpolation.
/// @param val The JsonValue to stringify.
/// @return String representation of the JsonValue.
static std::string
json_value_to_string(const openapi::JsonValue &val) noexcept {
  if (const auto *s = std::get_if<std::string>(&val.data)) {
    return *s;
  }
  if (const auto *i = std::get_if<int64_t>(&val.data)) {
    return std::to_string(*i);
  }
  if (const auto *b = std::get_if<bool>(&val.data)) {
    return *b ? "true" : "false";
  }
  if (const auto *d = std::get_if<double>(&val.data)) {
    std::ostringstream ss;
    ss << *d;
    return ss.str();
  }
  return "null";
}

utils::Result<openapi::JsonValue>
evaluate_expression(const ParsedRuntimeExpression &expr,
                    const WorkflowExecutionContext &ctx) noexcept {
  openapi::JsonValue base = nullptr;
  if (expr.kind == RuntimeExpressionKind::Url) {
    return openapi::JsonValue(ctx.url);
  }
  if (expr.kind == RuntimeExpressionKind::Method) {
    return openapi::JsonValue(ctx.method);
  }
  if (expr.kind == RuntimeExpressionKind::StatusCode) {
    return openapi::JsonValue(ctx.status_code);
  }
  if (expr.kind == RuntimeExpressionKind::Self) {
    return openapi::JsonValue(ctx.self_uri);
  }
  if (expr.kind == RuntimeExpressionKind::RequestHeader) {
    auto it = ctx.request_headers.find(expr.primary_key);
    if (it == ctx.request_headers.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing request header: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  }
  if (expr.kind == RuntimeExpressionKind::RequestQuery) {
    auto it = ctx.request_query.find(expr.primary_key);
    if (it == ctx.request_query.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing request query: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  }
  if (expr.kind == RuntimeExpressionKind::RequestPath) {
    auto it = ctx.request_path.find(expr.primary_key);
    if (it == ctx.request_path.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing request path: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  }
  if (expr.kind == RuntimeExpressionKind::RequestBody) {
    base = ctx.request_body;
  } else if (expr.kind == RuntimeExpressionKind::ResponseHeader) {
    auto it = ctx.response_headers.find(expr.primary_key);
    if (it == ctx.response_headers.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing response header: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  } else if (expr.kind == RuntimeExpressionKind::ResponseQuery) {
    auto it = ctx.response_query.find(expr.primary_key);
    if (it == ctx.response_query.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing response query: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  } else if (expr.kind == RuntimeExpressionKind::ResponsePath) {
    auto it = ctx.response_path.find(expr.primary_key);
    if (it == ctx.response_path.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing response path: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(it->second);
  } else if (expr.kind == RuntimeExpressionKind::ResponseBody) {
    base = ctx.response_body;
  } else if (expr.kind == RuntimeExpressionKind::Inputs) {
    auto it = ctx.inputs.find(expr.primary_key);
    if (it == ctx.inputs.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing workflow input: " + expr.primary_key,
                               expr.raw);
    }
    base = it->second;
  } else if (expr.kind == RuntimeExpressionKind::Outputs) {
    auto it = ctx.outputs.find(expr.primary_key);
    if (it == ctx.outputs.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing workflow output: " + expr.primary_key,
                               expr.raw);
    }
    base = it->second;
  } else if (expr.kind == RuntimeExpressionKind::Steps) {
    auto s_it = ctx.step_outputs.find(expr.primary_key);
    if (s_it == ctx.step_outputs.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing step outputs for: " + expr.primary_key,
                               expr.raw);
    }
    auto o_it = s_it->second.find(expr.secondary_key);
    if (o_it == s_it->second.end()) {
      return utils::make_error(utils::ErrorCode::WorkflowExecutionFailed,
                               "Missing step output key: " + expr.secondary_key,
                               expr.raw);
    }
    base = o_it->second;
  } else if (expr.kind == RuntimeExpressionKind::Workflows) {
    auto w_it = ctx.workflow_outputs.find(expr.primary_key);
    if (w_it == ctx.workflow_outputs.end()) {
      return utils::make_error(
          utils::ErrorCode::WorkflowExecutionFailed,
          "Missing workflow outputs for: " + expr.primary_key, expr.raw);
    }
    auto o_it = w_it->second.find(expr.secondary_key);
    if (o_it == w_it->second.end()) {
      return utils::make_error(
          utils::ErrorCode::WorkflowExecutionFailed,
          "Missing workflow output key: " + expr.secondary_key, expr.raw);
    }
    base = o_it->second;
  } else if (expr.kind == RuntimeExpressionKind::SourceDescriptions) {
    auto sd_it = ctx.source_description_urls.find(expr.primary_key);
    if (sd_it == ctx.source_description_urls.end()) {
      return utils::make_error(utils::ErrorCode::UnresolvedSourceDescription,
                               "Missing sourceDescription: " + expr.primary_key,
                               expr.raw);
    }
    return openapi::JsonValue(sd_it->second);
  } else if (expr.kind == RuntimeExpressionKind::ComponentsParameters) {
    auto cp_it = ctx.component_parameters.find(expr.primary_key);
    if (cp_it == ctx.component_parameters.end()) {
      return utils::make_error(
          utils::ErrorCode::UnresolvedComponentReference,
          "Missing component parameter: " + expr.primary_key, expr.raw);
    }
    return cp_it->second;
  } else {
    return openapi::JsonValue(expr.primary_key);
  }

  if (!expr.json_pointer.empty()) {
    return evaluate_json_pointer(base, expr.json_pointer);
  }
  return base;
}

utils::Result<std::string>
interpolate_template(std::string_view tmpl,
                     const WorkflowExecutionContext &ctx) noexcept {
  if (tmpl.starts_with('$')) {
    auto parsed = parse_runtime_expression(tmpl);
    if (!parsed.has_value()) {
      return std::unexpected(parsed.error());
    }
    auto val = evaluate_expression(*parsed, ctx);
    if (!val.has_value()) {
      return std::unexpected(val.error());
    }
    return json_value_to_string(*val);
  }

  std::string out;
  size_t pos = 0;
  while (pos < tmpl.size()) {
    auto open_brace = tmpl.find('{', pos);
    if (open_brace == std::string_view::npos) {
      out.append(tmpl.substr(pos));
      break;
    }
    out.append(tmpl.substr(pos, open_brace - pos));
    auto close_brace = tmpl.find('}', open_brace + 1);
    if (close_brace == std::string_view::npos) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Unclosed '{' in template string",
                               std::string(tmpl));
    }
    std::string_view inner =
        tmpl.substr(open_brace + 1, close_brace - open_brace - 1);
    if (inner.starts_with('$')) {
      auto parsed = parse_runtime_expression(inner);
      if (!parsed.has_value()) {
        return std::unexpected(parsed.error());
      }
      auto val = evaluate_expression(*parsed, ctx);
      if (!val.has_value()) {
        return std::unexpected(val.error());
      }
      out.append(json_value_to_string(*val));
    } else {
      out.push_back('{');
      out.append(inner);
      out.push_back('}');
    }
    pos = close_brace + 1;
  }
  return out;
}

/// @brief Recursively mutate a JsonValue at a given RFC 6901 JSON Pointer path.
/// @param node The current JsonValue node to mutate.
/// @param pointer The remaining RFC 6901 pointer starting with `/`.
/// @param new_val The replacement JsonValue to set.
/// @return Result containing true on success or an Error.
static utils::Result<bool>
set_json_pointer_value(openapi::JsonValue &node, std::string_view pointer,
                       const openapi::JsonValue &new_val) noexcept {
  if (pointer.empty()) {
    node = new_val;
    return true;
  }
  auto next_slash = pointer.find('/', 1);
  std::string_view raw_token = (next_slash == std::string_view::npos)
                                   ? pointer.substr(1)
                                   : pointer.substr(1, next_slash - 1);
  std::string token = unescape_pointer_token(raw_token);
  std::string_view rest = (next_slash == std::string_view::npos)
                              ? std::string_view{}
                              : pointer.substr(next_slash);

  if (auto *obj =
          std::get_if<std::map<std::string, openapi::JsonValue>>(&node.data)) {
    return set_json_pointer_value((*obj)[token], rest, new_val);
  }
  if (auto *arr = std::get_if<std::vector<openapi::JsonValue>>(&node.data)) {
    size_t idx = 0;
    (void)std::from_chars(token.data(), token.data() + token.size(), idx);
    if (idx >= arr->size()) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "Replacement array index out of bounds: " +
                                   token,
                               std::string(pointer));
    }
    return set_json_pointer_value((*arr)[idx], rest, new_val);
  }
  return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                           "Replacement target is not an object or array",
                           std::string(pointer));
}

utils::Result<openapi::JsonValue>
apply_payload_replacement(const openapi::JsonValue &payload,
                          const PayloadReplacement &replacement,
                          const WorkflowExecutionContext &ctx) noexcept {
  std::string_view target = replacement.target;
  if (target.starts_with('#')) {
    target.remove_prefix(1);
  }
  if (!is_valid_json_pointer(target)) {
    return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                             "Invalid PayloadReplacement target pointer",
                             replacement.target);
  }

  openapi::JsonValue resolved_val = replacement.value;
  if (const auto *str_val = std::get_if<std::string>(&replacement.value.data)) {
    if (str_val->starts_with('$') || str_val->find("{$") != std::string::npos) {
      auto interp = interpolate_template(*str_val, ctx);
      if (!interp.has_value()) {
        return std::unexpected(interp.error());
      }
      resolved_val = openapi::JsonValue(*interp);
    }
  }

  openapi::JsonValue copy = payload;
  auto res = set_json_pointer_value(copy, target, resolved_val);
  if (!res.has_value()) {
    return std::unexpected(res.error());
  }
  return copy;
}

/// @brief Trim leading and trailing ASCII whitespace from a string view.
/// @param sv The string view to trim.
/// @return Trimmed string view.
static std::string_view trim_ws(std::string_view sv) noexcept {
  while (!sv.empty() &&
         std::isspace(static_cast<unsigned char>(sv.front())) != 0) {
    sv.remove_prefix(1);
  }
  while (!sv.empty() &&
         std::isspace(static_cast<unsigned char>(sv.back())) != 0) {
    sv.remove_suffix(1);
  }
  return sv;
}

/// @brief Match an escaped character code (`d`, `w`, `s`, or literal) against
/// `c`.
/// @param esc The character immediately following `\`.
/// @param c The character to test.
/// @return True if `c` matches the escape sequence.
static bool match_escaped_char(char esc, char c) noexcept {
  if (esc == 'd')
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  if (esc == 'w')
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
  if (esc == 's')
    return std::isspace(static_cast<unsigned char>(c)) != 0;
  return c == esc;
}

/// @brief Match a single regex atom (`[a-z]`, `\d`, `\w`, `\s`, `.`, or
/// literal) against `c`.
/// @param atom The regex atom slice.
/// @param c The character to test.
/// @return True if `c` matches `atom`.
static bool match_regex_atom(std::string_view atom, char c) noexcept {
  if (atom.front() == '.') {
    return true;
  }
  if (atom.front() == '\\') {
    return match_escaped_char(atom[1], c);
  }
  if (atom.front() == '[') {
    std::string_view cls = atom.substr(1, atom.size() - 2);
    bool negate = cls.starts_with('^');
    if (negate)
      cls.remove_prefix(1);
    bool matched = false;
    for (size_t k = 0; k < cls.size(); ++k) {
      if (cls[k] == '\\') {
        if (match_escaped_char(cls[k + 1], c))
          matched = true;
        ++k;
      } else if (k + 2 < cls.size() && cls[k + 1] == '-') {
        if (c >= cls[k] && c <= cls[k + 2])
          matched = true;
        k += 2;
      } else if (c == cls[k]) {
        matched = true;
      }
    }
    return negate ? !matched : matched;
  }
  return c == atom.front();
}

/// @brief Match a regex pattern anchored at the beginning of `text`.
/// @param pat Remaining regex pattern.
/// @param text Remaining input text.
/// @return True if `pat` matches a prefix of `text`.
static bool match_regex_here(std::string_view pat,
                             std::string_view text) noexcept {
  if (pat.empty())
    return true;
  if (pat == "$")
    return text.empty();

  if (pat.front() == '(') {
    auto close_paren = pat.find(')');
    std::string_view group = pat.substr(1, close_paren - 1);
    std::string_view after_group = pat.substr(close_paren + 1);
    size_t start = 0;
    for (;;) {
      auto pipe = group.find('|', start);
      std::string_view alt = (pipe == std::string_view::npos)
                                 ? group.substr(start)
                                 : group.substr(start, pipe - start);
      if (text.starts_with(alt) &&
          match_regex_here(after_group, text.substr(alt.size())))
        return true;
      if (pipe == std::string_view::npos)
        break;
      start = pipe + 1;
    }
    return false;
  }

  size_t atom_len = 1;
  if (pat.front() == '\\') {
    atom_len = 2;
  } else if (pat.front() == '[') {
    atom_len = pat.find(']') + 1;
  }

  std::string_view atom = pat.substr(0, atom_len);
  char quant = (atom_len < pat.size()) ? pat[atom_len] : '\0';
  if (quant == '*' || quant == '+' || quant == '?' || quant == '{') {
    size_t min_rep = 0;
    size_t max_rep = text.size();
    std::string_view rest_pat = pat.substr(atom_len + 1);
    if (quant == '+') {
      min_rep = 1;
    } else if (quant == '?') {
      max_rep = 1;
    } else if (quant == '{') {
      auto close_brace = pat.find('}', atom_len + 1);
      std::string_view bounds =
          pat.substr(atom_len + 1, close_brace - atom_len - 1);
      rest_pat = pat.substr(close_brace + 1);
      auto comma = bounds.find(',');
      if (comma == std::string_view::npos) {
        (void)std::from_chars(bounds.data(), bounds.data() + bounds.size(),
                              min_rep);
        max_rep = min_rep;
      } else {
        (void)std::from_chars(bounds.data(), bounds.data() + comma, min_rep);
        std::string_view max_sv = bounds.substr(comma + 1);
        if (!max_sv.empty()) {
          (void)std::from_chars(max_sv.data(), max_sv.data() + max_sv.size(),
                                max_rep);
        }
      }
    }
    for (size_t i = 0; i <= max_rep; ++i) {
      if (i >= min_rep && match_regex_here(rest_pat, text.substr(i)))
        return true;
      if (i >= text.size() || !match_regex_atom(atom, text[i]))
        break;
    }
    return false;
  }

  return !text.empty() && match_regex_atom(atom, text.front()) &&
         match_regex_here(pat.substr(atom_len), text.substr(1));
}

/// @brief Find a top-level `|` alternation operator outside `(...)` and
/// `[...]`.
/// @param pattern The regex pattern string view.
/// @return Index of top-level `|` or `std::string_view::npos`.
static size_t find_top_level_pipe(std::string_view pattern) noexcept {
  int paren_depth = 0;
  bool in_brackets = false;
  for (size_t i = 0; i < pattern.size(); ++i) {
    if (pattern[i] == '\\') {
      ++i;
      continue;
    }
    if (pattern[i] == '[') {
      in_brackets = true;
    } else if (pattern[i] == ']') {
      in_brackets = false;
    } else if (!in_brackets && pattern[i] == '(') {
      ++paren_depth;
    } else if (!in_brackets && pattern[i] == ')') {
      --paren_depth;
    } else if (!in_brackets && paren_depth == 0 && pattern[i] == '|') {
      return i;
    }
  }
  return std::string_view::npos;
}

/// @brief Evaluate a regular expression pattern against `text` without
/// exceptions.
/// @param pattern The regular expression string.
/// @param text The target string to test.
/// @return True if `pattern` matches `text`.
static bool match_regex(std::string_view pattern,
                        std::string_view text) noexcept {
  size_t pipe = find_top_level_pipe(pattern);
  if (pipe != std::string_view::npos) {
    return match_regex(pattern.substr(0, pipe), text) ||
           match_regex(pattern.substr(pipe + 1), text);
  }
  if (pattern.starts_with('^')) {
    return match_regex_here(pattern.substr(1), text);
  }
  for (size_t i = 0; i <= text.size(); ++i) {
    if (match_regex_here(pattern, text.substr(i)))
      return true;
  }
  return false;
}

/// @brief Resolve a token (either `$...` runtime expression or literal) to
/// string for comparison.
/// @param token The token string view.
/// @param ctx The active workflow execution context.
/// @param default_ctx_val Optional value resolved from `Criterion::context`.
/// @return Result containing resolved string representation.
static utils::Result<std::string>
resolve_operand(std::string_view token, const WorkflowExecutionContext &ctx,
                const std::optional<std::string> &default_ctx_val) noexcept {
  token = trim_ws(token);
  if (token == "$") {
    return default_ctx_val.value_or("");
  }
  if (token.starts_with('$')) {
    return interpolate_template(token, ctx);
  }
  if (token.size() >= 2 &&
      (token.front() == static_cast<char>(39) || token.front() == '"') &&
      token.back() == token.front()) {
    return std::string(token.substr(1, token.size() - 2));
  }
  return std::string(token);
}

/// @brief Compare two resolved string/numeric operands using a relational
/// operator.
/// @param lhs Left-hand side string value.
/// @param op Relational operator (`==`, `!=`, `>=`, `<=`, `>`, `<`).
/// @param rhs Right-hand side string value.
/// @return True if the relational condition holds.
static bool compare_values(std::string_view lhs, std::string_view op,
                           std::string_view rhs) noexcept {
  int64_t l_num = 0;
  int64_t r_num = 0;
  auto [lp, lec] = std::from_chars(lhs.data(), lhs.data() + lhs.size(), l_num);
  auto [rp, rec] = std::from_chars(rhs.data(), rhs.data() + rhs.size(), r_num);
  bool both_numeric = (lec == std::errc{} && rec == std::errc{});

  if (op == "==")
    return both_numeric ? (l_num == r_num) : (lhs == rhs);
  if (op == "!=")
    return both_numeric ? (l_num != r_num) : (lhs != rhs);
  if (op == ">=")
    return l_num >= r_num;
  if (op == "<=")
    return l_num <= r_num;
  if (op == ">")
    return l_num > r_num;
  return l_num < r_num;
}

/// @brief Convert a JSONPath selector (`$.a.b` or `$['a'][0]`) to an RFC 6901
/// JSON Pointer.
/// @param jp The JSONPath selector starting after `$`.
/// @return Converted RFC 6901 JSON Pointer string.
static std::string jsonpath_to_pointer(std::string_view jp) noexcept {
  jp = trim_ws(jp);
  std::string ptr;
  size_t i = 0;
  while (i < jp.size()) {
    if (jp[i] == '.') {
      ++i;
      ptr.push_back('/');
      while (i < jp.size() && jp[i] != '.' && jp[i] != '[') {
        ptr.push_back(jp[i]);
        ++i;
      }
    } else if (jp[i] == '[') {
      ++i;
      if (jp[i] == '\'' || jp[i] == '"') {
        char quote = jp[i++];
        ptr.push_back('/');
        while (jp[i] != quote) {
          if (jp[i] == '~')
            ptr.append("~0");
          else if (jp[i] == '/')
            ptr.append("~1");
          else
            ptr.push_back(jp[i]);
          ++i;
        }
        i = jp.find(']', i) + 1;
      } else {
        ptr.push_back('/');
        while (jp[i] != ']') {
          ptr.push_back(jp[i]);
          ++i;
        }
        ++i;
      }
    } else {
      ptr.push_back(jp[i]);
      ++i;
    }
  }
  return ptr;
}

utils::Result<bool>
evaluate_criterion(const Criterion &criterion,
                   const WorkflowExecutionContext &ctx) noexcept {
  std::optional<std::string> ctx_str = std::nullopt;
  openapi::JsonValue ctx_json = ctx.response_body;
  if (criterion.context.has_value()) {
    auto parsed_ctx = parse_runtime_expression(*criterion.context);
    if (!parsed_ctx.has_value()) {
      return std::unexpected(parsed_ctx.error());
    }
    auto eval_ctx = evaluate_expression(*parsed_ctx, ctx);
    if (!eval_ctx.has_value()) {
      return std::unexpected(eval_ctx.error());
    }
    ctx_json = *eval_ctx;
    ctx_str = json_value_to_string(*eval_ctx);
  }

  CriterionType eff_type = CriterionType::Simple;
  if (criterion.type.has_value()) {
    if (const auto *ct = std::get_if<CriterionType>(&*criterion.type)) {
      eff_type = *ct;
    } else {
      eff_type = std::get<CriterionExpressionType>(*criterion.type).type;
    }
  }

  std::string_view cond = trim_ws(criterion.condition);
  if (cond.empty()) {
    return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                             "Criterion condition cannot be empty");
  }

  if (eff_type == CriterionType::JsonPath) {
    if (!cond.starts_with('$')) {
      return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                               "JSONPath condition must start with '$'",
                               criterion.condition);
    }
    std::string_view jp_expr = cond.substr(1);
    std::string_view found_op;
    size_t op_pos = std::string_view::npos;
    for (std::string_view op : {"==", "!=", ">=", "<=", ">", "<"}) {
      auto pos = jp_expr.find(op);
      if (pos != std::string_view::npos) {
        found_op = op;
        op_pos = pos;
        break;
      }
    }
    std::string_view selector = (op_pos == std::string_view::npos)
                                    ? jp_expr
                                    : jp_expr.substr(0, op_pos);
    std::string ptr = jsonpath_to_pointer(selector);
    auto resolved = evaluate_json_pointer(ctx_json, ptr);
    if (!resolved.has_value()) {
      return false;
    }
    if (op_pos != std::string_view::npos) {
      auto rhs = resolve_operand(jp_expr.substr(op_pos + found_op.size()), ctx,
                                 ctx_str);
      if (!rhs.has_value()) {
        return std::unexpected(rhs.error());
      }
      return compare_values(json_value_to_string(*resolved), found_op, *rhs);
    }
    std::string val_str = json_value_to_string(*resolved);
    return val_str != "false" && val_str != "null";
  }

  if (eff_type == CriterionType::Regex) {
    std::string target = ctx_str.value_or("");
    return match_regex(cond, target);
  }

  if (eff_type == CriterionType::XPath) {
    std::string target = ctx_str.value_or("");
    if (target.empty() || !cond.starts_with('/')) {
      return false;
    }
    if (target.starts_with('<')) {
      auto last_slash = cond.rfind('/');
      std::string_view tag = cond.substr(last_slash + 1);
      return target.find("<" + std::string(tag)) != std::string::npos;
    }
    return true;
  }

  // Compound logical conditions (`||` and `&&`)
  auto or_pos = cond.find(" || ");
  if (or_pos != std::string_view::npos) {
    Criterion left_c = criterion;
    left_c.condition = std::string(cond.substr(0, or_pos));
    auto l_res = evaluate_criterion(left_c, ctx);
    if (!l_res.has_value())
      return std::unexpected(l_res.error());
    if (*l_res)
      return true;
    Criterion right_c = criterion;
    right_c.condition = std::string(cond.substr(or_pos + 4));
    return evaluate_criterion(right_c, ctx);
  }

  auto and_pos = cond.find(" && ");
  if (and_pos != std::string_view::npos) {
    Criterion left_c = criterion;
    left_c.condition = std::string(cond.substr(0, and_pos));
    auto l_res = evaluate_criterion(left_c, ctx);
    if (!l_res.has_value())
      return std::unexpected(l_res.error());
    if (!*l_res)
      return false;
    Criterion right_c = criterion;
    right_c.condition = std::string(cond.substr(and_pos + 4));
    return evaluate_criterion(right_c, ctx);
  }

  // Simple condition evaluation: support ==, !=, >=, <=, >, <
  for (std::string_view op : {"==", "!=", ">=", "<=", ">", "<"}) {
    auto op_pos = cond.find(op);
    if (op_pos != std::string_view::npos) {
      auto lhs = resolve_operand(cond.substr(0, op_pos), ctx, ctx_str);
      if (!lhs.has_value())
        return std::unexpected(lhs.error());
      auto rhs = resolve_operand(cond.substr(op_pos + op.size()), ctx, ctx_str);
      if (!rhs.has_value())
        return std::unexpected(rhs.error());
      return compare_values(*lhs, op, *rhs);
    }
  }

  auto single_val = resolve_operand(cond, ctx, ctx_str);
  if (!single_val.has_value()) {
    return std::unexpected(single_val.error());
  }
  return *single_val != "false";
}

} // namespace cdd_cpp::arazzo
