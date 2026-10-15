/// @file parse.cpp
/// @brief Implementation of Model Context Protocol (MCP) JSON-RPC message
/// parsers.

#include "parse.hpp"
#include <iostream>

namespace cdd_cpp::mcp {

/// @brief Extract an optional string field from an ondemand JSON object.
/// @param obj Source JSON object.
/// @param key Field key name.
/// @param out Target optional string to populate if present.
static void set_opt_string(simdjson::ondemand::object &obj,
                           std::string_view key,
                           std::optional<std::string> &out) noexcept {
  std::string_view sv;
  if (!obj[key].get_string().get(sv)) {
    out = std::string(sv);
  }
}

/// @brief Extract an optional bool field from an ondemand JSON object.
/// @param obj Source JSON object.
/// @param key Field key name.
/// @param out Target optional bool to populate if present.
static void set_opt_bool(simdjson::ondemand::object &obj, std::string_view key,
                         std::optional<bool> &out) noexcept {
  bool b = false;
  if (!obj[key].get_bool().get(b)) {
    out = b;
  }
}

/// @brief Extract an optional double field from an ondemand JSON object.
/// @param obj Source JSON object.
/// @param key Field key name.
/// @param out Target optional double to populate if present.
static void set_opt_double(simdjson::ondemand::object &obj,
                           std::string_view key,
                           std::optional<double> &out) noexcept {
  double d = 0.0;
  if (!obj[key].get_double().get(d)) {
    out = d;
  }
}

/// @brief Extract an optional raw JSON value or object field from an ondemand
/// JSON object.
/// @param obj Source JSON object.
/// @param key Field key name.
/// @param out Target optional string to populate with raw JSON if present.
static void set_opt_raw(simdjson::ondemand::object &obj, std::string_view key,
                        std::optional<std::string> &out) noexcept {
  simdjson::ondemand::value v;
  if (!obj[key].get(v)) {
    out = std::string(v.raw_json().value_unsafe());
  }
}

/// @brief Internal helper or implementation for `parse_annotations`.
std::optional<Annotations> parse_annotations(simdjson::ondemand::object &obj) {
  simdjson::ondemand::object annotations_obj;
  if (!obj["annotations"].get_object().get(annotations_obj)) {
    Annotations annotations;
    set_opt_string(annotations_obj, "audience", annotations.audience);
    set_opt_double(annotations_obj, "priority", annotations.priority);
    return annotations;
  }
  return std::nullopt;
}

/// @brief Internal helper or implementation for `parse_annotated`.
McpResult<Annotated> parse_annotated(simdjson::ondemand::value &val) noexcept {
  Annotated result;
  simdjson::ondemand::object obj;
  auto error = val.get_object().get(obj);
  if (error) {
    return std::unexpected("Expected object for Annotated");
  }
  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<BlobResourceContents>
parse_blob_resource_contents(simdjson::ondemand::value &val) noexcept {
  BlobResourceContents result;
  simdjson::ondemand::object obj;
  auto error = val.get_object().get(obj);
  if (error) {
    return std::unexpected("Expected object for BlobResourceContents");
  }

  std::string_view blob_sv;
  if (obj["blob"].get_string().get(blob_sv)) {
    return std::unexpected("Missing required field: blob");
  }
  result.blob = std::string(blob_sv);

  std::string_view uri_sv;
  if (obj["uri"].get_string().get(uri_sv)) {
    return std::unexpected("Missing required field: uri");
  }
  result.uri = std::string(uri_sv);

  set_opt_string(obj, "mimeType", result.mimeType);

  return result;
}

McpResult<CallToolRequest>
parse_call_tool_request(simdjson::ondemand::value &val) noexcept {
  CallToolRequest result;
  simdjson::ondemand::object obj;
  auto error = val.get_object().get(obj);
  if (error) {
    return std::unexpected("Expected object for CallToolRequest");
  }

  std::string_view method_sv;
  if (obj["method"].get_string().get(method_sv)) {
    return std::unexpected("Missing required field: method");
  }
  result.method = std::string(method_sv);

  simdjson::ondemand::object params_obj;
  if (obj["params"].get_object().get(params_obj)) {
    return std::unexpected("Missing required field: params");
  }

  std::string_view name_sv;
  if (params_obj["name"].get_string().get(name_sv)) {
    return std::unexpected("Missing required field: params.name");
  }
  result.params.name = std::string(name_sv);

  set_opt_raw(params_obj, "arguments", result.params.arguments);

  return result;
}

McpResult<TextContent>
parse_text_content(simdjson::ondemand::value &val) noexcept {
  TextContent result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view t_sv;
  if (obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing type");
  result.type = std::string(t_sv);
  std::string_view text_sv;
  if (obj["text"].get_string().get(text_sv))
    return std::unexpected("Missing text");
  result.text = std::string(text_sv);
  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<ImageContent>
parse_image_content(simdjson::ondemand::value &val) noexcept {
  ImageContent result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view t_sv;
  if (obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing type");
  result.type = std::string(t_sv);
  std::string_view d_sv;
  if (obj["data"].get_string().get(d_sv))
    return std::unexpected("Missing data");
  result.data = std::string(d_sv);
  std::string_view m_sv;
  if (obj["mimeType"].get_string().get(m_sv))
    return std::unexpected("Missing mimeType");
  result.mimeType = std::string(m_sv);
  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<TextResourceContents>
parse_text_resource_contents(simdjson::ondemand::value &val) noexcept {
  TextResourceContents result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  set_opt_string(obj, "mimeType", result.mimeType);
  std::string_view t_sv;
  if (obj["text"].get_string().get(t_sv))
    return std::unexpected("Missing text");
  result.text = std::string(t_sv);
  return result;
}

McpResult<EmbeddedResource>
parse_embedded_resource(simdjson::ondemand::value &val) noexcept {
  EmbeddedResource result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view t_sv;
  if (obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing type");
  result.type = std::string(t_sv);

  simdjson::ondemand::object res_obj;
  if (obj["resource"].get_object().get(res_obj))
    return std::unexpected("Missing resource");
  result.resource_json = std::string(res_obj.raw_json().value_unsafe());

  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<CallToolResult>
parse_call_tool_result(simdjson::ondemand::value &val) noexcept {
  CallToolResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);

  simdjson::ondemand::array content_arr;
  if (obj["content"].get_array().get(content_arr))
    return std::unexpected("Missing content");
  result.content_json = std::string(content_arr.raw_json().value_unsafe());

  set_opt_bool(obj, "isError", result.isError);

  return result;
}

McpResult<CancelledNotification>
parse_cancelled_notification(simdjson::ondemand::value &val) noexcept {
  CancelledNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  simdjson::ondemand::value req_id_val;
  if (p_obj["requestId"].get(req_id_val))
    return std::unexpected("Missing requestId");
  auto t = req_id_val.type().value_unsafe();
  if (t == simdjson::ondemand::json_type::string) {
    result.params.requestId =
        std::string(req_id_val.get_string().value_unsafe());
  } else {
    result.params.requestId =
        std::to_string(req_id_val.get_int64().value_unsafe());
  }

  set_opt_string(p_obj, "reason", result.params.reason);

  return result;
}

McpResult<ClientCapabilities>
parse_client_capabilities(simdjson::ondemand::value &val) noexcept {
  ClientCapabilities result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "experimental", result.experimental);

  simdjson::ondemand::object roots_obj;
  if (!obj["roots"].get_object().get(roots_obj)) {
    ClientCapabilitiesRoots roots;
    set_opt_bool(roots_obj, "listChanged", roots.listChanged);
    result.roots = roots;
  }

  set_opt_raw(obj, "sampling", result.sampling);

  return result;
}

McpResult<CompleteRequest>
parse_complete_request(simdjson::ondemand::value &val) noexcept {
  CompleteRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  simdjson::ondemand::object arg_obj;
  if (p_obj["argument"].get_object().get(arg_obj))
    return std::unexpected("Missing argument");

  std::string_view name_sv;
  if (arg_obj["name"].get_string().get(name_sv))
    return std::unexpected("Missing argument.name");
  result.params.argument.name = std::string(name_sv);

  std::string_view val_sv;
  if (arg_obj["value"].get_string().get(val_sv))
    return std::unexpected("Missing argument.value");
  result.params.argument.value = std::string(val_sv);

  simdjson::ondemand::object ref_obj;
  if (p_obj["ref"].get_object().get(ref_obj))
    return std::unexpected("Missing ref");
  result.params.ref_json = std::string(ref_obj.raw_json().value_unsafe());

  return result;
}

McpResult<CompleteResult>
parse_complete_result(simdjson::ondemand::value &val) noexcept {
  CompleteResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);

  simdjson::ondemand::object c_obj;
  if (obj["completion"].get_object().get(c_obj))
    return std::unexpected("Missing completion");

  simdjson::ondemand::array v_arr;
  if (c_obj["values"].get_array().get(v_arr))
    return std::unexpected("Missing completion.values");
  for (auto v : v_arr) {
    result.completion.values.push_back(
        std::string(v.get_string().value_unsafe()));
  }

  int64_t total_val = 0;
  if (!c_obj["total"].get_int64().get(total_val))
    result.completion.total = static_cast<int>(total_val);

  set_opt_bool(c_obj, "hasMore", result.completion.hasMore);

  return result;
}

/// @brief Internal helper or implementation for `parse_model_hint`.
McpResult<ModelHint> parse_model_hint(simdjson::ondemand::value &val) noexcept {
  ModelHint result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_string(obj, "name", result.name);
  return result;
}

McpResult<ModelPreferences>
parse_model_preferences(simdjson::ondemand::value &val) noexcept {
  ModelPreferences result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_double(obj, "costPriority", result.costPriority);
  set_opt_double(obj, "intelligencePriority", result.intelligencePriority);
  set_opt_double(obj, "speedPriority", result.speedPriority);

  simdjson::ondemand::array hints_arr;
  if (!obj["hints"].get_array().get(hints_arr)) {
    std::vector<ModelHint> hints;
    for (auto h : hints_arr) {
      auto h_val = h.value_unsafe();
      auto ph = parse_model_hint(h_val);
      if (ph.has_value())
        hints.push_back(ph.operator*());
      else
        return std::unexpected(ph.error());
    }
    result.hints = hints;
  }

  return result;
}

McpResult<CreateMessageRequest>
parse_create_message_request(simdjson::ondemand::value &val) noexcept {
  CreateMessageRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  int64_t mt;
  if (p_obj["maxTokens"].get_int64().get(mt))
    return std::unexpected("Missing maxTokens");
  result.params.maxTokens = static_cast<int>(mt);

  simdjson::ondemand::array msg_arr;
  if (p_obj["messages"].get_array().get(msg_arr))
    return std::unexpected("Missing messages");
  result.params.messages_json = std::string(msg_arr.raw_json().value_unsafe());

  set_opt_raw(p_obj, "includeContext", result.params.includeContext);
  set_opt_raw(p_obj, "metadata", result.params.metadata);

  simdjson::ondemand::value mp_val;
  if (!p_obj["modelPreferences"].get(mp_val)) {
    result.params.modelPreferences =
        parse_model_preferences(mp_val).operator*();
  }

  simdjson::ondemand::array ss_arr;
  if (!p_obj["stopSequences"].get_array().get(ss_arr)) {
    std::vector<std::string> ss;
    for (auto s : ss_arr) {
      ss.push_back(std::string(s.get_string().value_unsafe()));
    }
    result.params.stopSequences = ss;
  }

  set_opt_string(p_obj, "systemPrompt", result.params.systemPrompt);
  set_opt_double(p_obj, "temperature", result.params.temperature);

  return result;
}

McpResult<CreateMessageResult>
parse_create_message_result(simdjson::ondemand::value &val) noexcept {
  CreateMessageResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);

  simdjson::ondemand::value c_val;
  if (obj["content"].get(c_val))
    return std::unexpected("Missing content");
  result.content_json = std::string(c_val.raw_json().value_unsafe());

  std::string_view mod_sv;
  if (obj["model"].get_string().get(mod_sv))
    return std::unexpected("Missing model");
  result.model = std::string(mod_sv);

  std::string_view role_sv;
  if (obj["role"].get_string().get(role_sv))
    return std::unexpected("Missing role");
  result.role = std::string(role_sv);

  set_opt_string(obj, "stopReason", result.stopReason);

  return result;
}

/// @brief Internal helper or implementation for `parse_tool`.
McpResult<Tool> parse_tool(simdjson::ondemand::value &val) noexcept {
  Tool result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  set_opt_string(obj, "description", result.description);

  simdjson::ondemand::object is_obj;
  if (obj["inputSchema"].get_object().get(is_obj))
    return std::unexpected("Missing inputSchema");

  std::string_view t_sv;
  if (is_obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing inputSchema.type");
  result.inputSchema.type = std::string(t_sv);

  set_opt_raw(is_obj, "properties", result.inputSchema.properties_json);

  simdjson::ondemand::array req_arr;
  if (!is_obj["required"].get_array().get(req_arr)) {
    std::vector<std::string> req;
    for (auto r : req_arr) {
      req.push_back(std::string(r.get_string().value_unsafe()));
    }
    result.inputSchema.required = req;
  }

  return result;
}

McpResult<ToolListChangedNotification>
parse_tool_list_changed_notification(simdjson::ondemand::value &val) noexcept {
  ToolListChangedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ToolListChangedNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }

  return result;
}

McpResult<PaginatedRequest>
parse_paginated_request(simdjson::ondemand::value &val) noexcept {
  PaginatedRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    set_opt_string(p_obj, "cursor", result.params.cursor);
  }

  return result;
}

McpResult<PaginatedResult>
parse_paginated_result(simdjson::ondemand::value &val) noexcept {
  PaginatedResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "nextCursor", result.nextCursor);

  return result;
}

McpResult<Implementation>
parse_implementation(simdjson::ondemand::value &val) noexcept {
  Implementation result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);
  std::string_view v_sv;
  if (obj["version"].get_string().get(v_sv))
    return std::unexpected("Missing version");
  result.version = std::string(v_sv);
  return result;
}

McpResult<InitializeRequest>
parse_initialize_request(simdjson::ondemand::value &val) noexcept {
  InitializeRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  std::string_view pv_sv;
  if (p_obj["protocolVersion"].get_string().get(pv_sv))
    return std::unexpected("Missing protocolVersion");
  result.params.protocolVersion = std::string(pv_sv);

  simdjson::ondemand::value cap_val;
  if (!p_obj["capabilities"].get(cap_val)) {
    result.params.capabilities = parse_client_capabilities(cap_val).operator*();
  }

  simdjson::ondemand::value ci_val;
  if (p_obj["clientInfo"].get(ci_val))
    return std::unexpected("Missing clientInfo");
  auto ci_res = parse_implementation(ci_val);
  if (!ci_res.has_value())
    return std::unexpected("Invalid clientInfo");
  result.params.clientInfo = ci_res.operator*();

  return result;
}

McpResult<ServerCapabilities>
parse_server_capabilities(simdjson::ondemand::value &val) noexcept {
  ServerCapabilities result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "experimental", result.experimental);

  simdjson::ondemand::object log_obj;
  if (!obj["logging"].get_object().get(log_obj)) {
    result.logging = ServerCapabilitiesLogging{};
  }

  simdjson::ondemand::object pr_obj;
  if (!obj["prompts"].get_object().get(pr_obj)) {
    ServerCapabilitiesPrompts prompts;
    set_opt_bool(pr_obj, "listChanged", prompts.listChanged);
    result.prompts = prompts;
  }

  simdjson::ondemand::object res_obj;
  if (!obj["resources"].get_object().get(res_obj)) {
    ServerCapabilitiesResources resources;
    set_opt_bool(res_obj, "listChanged", resources.listChanged);
    set_opt_bool(res_obj, "subscribe", resources.subscribe);
    result.resources = resources;
  }

  simdjson::ondemand::object tools_obj;
  if (!obj["tools"].get_object().get(tools_obj)) {
    ServerCapabilitiesTools tools;
    set_opt_bool(tools_obj, "listChanged", tools.listChanged);
    result.tools = tools;
  }

  return result;
}

McpResult<InitializeResult>
parse_initialize_result(simdjson::ondemand::value &val) noexcept {
  InitializeResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);

  std::string_view pv_sv;
  if (obj["protocolVersion"].get_string().get(pv_sv))
    return std::unexpected("Missing protocolVersion");
  result.protocolVersion = std::string(pv_sv);

  set_opt_string(obj, "instructions", result.instructions);

  simdjson::ondemand::value cap_val;
  if (obj["capabilities"].get(cap_val))
    return std::unexpected("Missing capabilities");
  auto cap_res = parse_server_capabilities(cap_val);
  if (!cap_res.has_value())
    return std::unexpected("Invalid capabilities");
  result.capabilities = cap_res.operator*();

  simdjson::ondemand::value si_val;
  if (obj["serverInfo"].get(si_val))
    return std::unexpected("Missing serverInfo");
  auto si_res = parse_implementation(si_val);
  if (!si_res.has_value())
    return std::unexpected("Invalid serverInfo");
  result.serverInfo = si_res.operator*();

  return result;
}

McpResult<InitializedNotification>
parse_initialized_notification(simdjson::ondemand::value &val) noexcept {
  InitializedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    InitializedNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }

  return result;
}

/// @brief Internal helper or implementation for `parse_request_id`.
std::string parse_request_id(simdjson::ondemand::value &val) noexcept {
  std::string out = "null";
  auto t = val.type().value_unsafe();
  if (t == simdjson::ondemand::json_type::string) {
    out = "\"" + std::string(val.get_string().value_unsafe()) + "\"";
  } else if (t == simdjson::ondemand::json_type::number) {
    out = std::to_string(val.get_int64().value_unsafe());
  }
  return out;
}

McpResult<JSONRPCError>
parse_jsonrpc_error(simdjson::ondemand::value &val) noexcept {
  JSONRPCError result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view jr_sv;
  if (obj["jsonrpc"].get_string().get(jr_sv))
    return std::unexpected("Missing jsonrpc");
  result.jsonrpc = std::string(jr_sv);

  simdjson::ondemand::value id_val;
  if (obj["id"].get(id_val))
    return std::unexpected("Missing id");
  result.id = parse_request_id(id_val);

  simdjson::ondemand::object err_obj;
  if (obj["error"].get_object().get(err_obj))
    return std::unexpected("Missing error");

  int64_t code;
  if (err_obj["code"].get_int64().get(code))
    return std::unexpected("Missing error.code");
  result.error.code = static_cast<int>(code);

  std::string_view msg_sv;
  if (err_obj["message"].get_string().get(msg_sv))
    return std::unexpected("Missing error.message");
  result.error.message = std::string(msg_sv);

  set_opt_raw(err_obj, "data", result.error.data);

  return result;
}

McpResult<JSONRPCNotification>
parse_jsonrpc_notification(simdjson::ondemand::value &val) noexcept {
  JSONRPCNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view jr_sv;
  if (obj["jsonrpc"].get_string().get(jr_sv))
    return std::unexpected("Missing jsonrpc");
  result.jsonrpc = std::string(jr_sv);

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    JSONRPCNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    params._raw = std::string(p_obj.raw_json().value_unsafe());
    result.params = params;
  }

  return result;
}

McpResult<JSONRPCRequest>
parse_jsonrpc_request(simdjson::ondemand::value &val) noexcept {
  JSONRPCRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view jr_sv;
  if (obj["jsonrpc"].get_string().get(jr_sv))
    return std::unexpected("Missing jsonrpc");
  result.jsonrpc = std::string(jr_sv);

  simdjson::ondemand::value id_val;
  if (obj["id"].get(id_val))
    return std::unexpected("Missing id");
  result.id = parse_request_id(id_val);

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    JSONRPCRequestParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    params._raw = std::string(p_obj.raw_json().value_unsafe());
    result.params = params;
  }

  return result;
}

McpResult<JSONRPCResponse>
parse_jsonrpc_response(simdjson::ondemand::value &val) noexcept {
  JSONRPCResponse result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view jr_sv;
  if (obj["jsonrpc"].get_string().get(jr_sv))
    return std::unexpected("Missing jsonrpc");
  result.jsonrpc = std::string(jr_sv);

  simdjson::ondemand::value id_val;
  if (obj["id"].get(id_val))
    return std::unexpected("Missing id");
  result.id = parse_request_id(id_val);

  simdjson::ondemand::value res_val;
  if (obj["result"].get(res_val))
    return std::unexpected("Missing result");
  result.result_json = std::string(res_val.raw_json().value_unsafe());

  return result;
}

McpResult<ListPromptsRequest>
parse_list_prompts_request(simdjson::ondemand::value &val) noexcept {
  ListPromptsRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListPromptsRequestParams params;
    set_opt_string(p_obj, "cursor", params.cursor);
    result.params = params;
  }

  return result;
}

McpResult<PromptArgument>
parse_prompt_argument(simdjson::ondemand::value &val) noexcept {
  PromptArgument result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  set_opt_string(obj, "description", result.description);
  set_opt_bool(obj, "required", result.required);

  return result;
}

/// @brief Internal helper or implementation for `parse_prompt`.
McpResult<Prompt> parse_prompt(simdjson::ondemand::value &val) noexcept {
  Prompt result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  set_opt_string(obj, "description", result.description);

  simdjson::ondemand::array arg_arr;
  if (!obj["arguments"].get_array().get(arg_arr)) {
    std::vector<PromptArgument> args;
    for (auto a : arg_arr) {
      auto a_val = a.value_unsafe();
      auto pa = parse_prompt_argument(a_val);
      if (pa.has_value())
        args.push_back(pa.operator*());
      else
        return std::unexpected(pa.error());
    }
    result.arguments = args;
  }

  return result;
}

McpResult<ListPromptsResult>
parse_list_prompts_result(simdjson::ondemand::value &val) noexcept {
  ListPromptsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "nextCursor", result.nextCursor);

  simdjson::ondemand::array p_arr;
  if (obj["prompts"].get_array().get(p_arr))
    return std::unexpected("Missing prompts");

  for (auto p : p_arr) {
    auto p_val = p.value_unsafe();
    auto pt = parse_prompt(p_val);
    if (pt.has_value())
      result.prompts.push_back(pt.operator*());
    else
      return std::unexpected(pt.error());
  }

  return result;
}

McpResult<GetPromptRequest>
parse_get_prompt_request(simdjson::ondemand::value &val) noexcept {
  GetPromptRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  std::string_view n_sv;
  if (p_obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing params.name");
  result.params.name = std::string(n_sv);

  set_opt_raw(p_obj, "arguments", result.params.arguments);

  return result;
}

McpResult<GetPromptResult>
parse_get_prompt_result(simdjson::ondemand::value &val) noexcept {
  GetPromptResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "description", result.description);

  simdjson::ondemand::array msg_arr;
  if (obj["messages"].get_array().get(msg_arr))
    return std::unexpected("Missing messages");

  for (auto m : msg_arr) {
    simdjson::ondemand::object m_obj;
    if (m.get_object().get(m_obj))
      return std::unexpected("Expected object in messages array");

    PromptMessage pm;
    std::string_view mr_sv;
    if (m_obj["role"].get_string().get(mr_sv))
      return std::unexpected("Missing role in PromptMessage");
    pm.role = std::string(mr_sv);

    simdjson::ondemand::value c_val;
    if (m_obj["content"].get(c_val))
      return std::unexpected("Missing content in PromptMessage");

    pm.content_json = std::string(c_val.raw_json().value_unsafe());

    result.messages.push_back(pm);
  }

  return result;
}

/// @brief Internal helper or implementation for
/// `parse_prompt_list_changed_notification`.
McpResult<PromptListChangedNotification> parse_prompt_list_changed_notification(
    simdjson::ondemand::value &val) noexcept {
  PromptListChangedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);

  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    PromptListChangedNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }

  return result;
}

McpResult<PromptReference>
parse_prompt_reference(simdjson::ondemand::value &val) noexcept {
  PromptReference result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view t_sv;
  if (obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing type");
  result.type = std::string(t_sv);

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  return result;
}

McpResult<ListResourcesRequest>
parse_list_resources_request(simdjson::ondemand::value &val) noexcept {
  ListResourcesRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListResourcesRequestParams params;
    set_opt_string(p_obj, "cursor", params.cursor);
    result.params = params;
  }
  return result;
}

/// @brief Internal helper or implementation for `parse_resource`.
McpResult<Resource> parse_resource(simdjson::ondemand::value &val) noexcept {
  Resource result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);
  set_opt_string(obj, "description", result.description);
  set_opt_string(obj, "mimeType", result.mimeType);
  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<ListResourcesResult>
parse_list_resources_result(simdjson::ondemand::value &val) noexcept {
  ListResourcesResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "nextCursor", result.nextCursor);
  simdjson::ondemand::array r_arr;
  if (obj["resources"].get_array().get(r_arr))
    return std::unexpected("Missing resources");
  for (auto r : r_arr) {
    auto r_val = r.value_unsafe();
    auto res = parse_resource(r_val);
    if (res.has_value())
      result.resources.push_back(res.operator*());
    else
      return std::unexpected(res.error());
  }
  return result;
}

McpResult<ListResourceTemplatesRequest>
parse_list_resource_templates_request(simdjson::ondemand::value &val) noexcept {
  ListResourceTemplatesRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListResourceTemplatesRequestParams params;
    set_opt_string(p_obj, "cursor", params.cursor);
    result.params = params;
  }
  return result;
}

McpResult<ResourceTemplate>
parse_resource_template(simdjson::ondemand::value &val) noexcept {
  ResourceTemplate result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uriTemplate"].get_string().get(u_sv))
    return std::unexpected("Missing uriTemplate");
  result.uriTemplate = std::string(u_sv);
  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);
  set_opt_string(obj, "description", result.description);
  set_opt_string(obj, "mimeType", result.mimeType);
  result.annotations = parse_annotations(obj);
  return result;
}

McpResult<ListResourceTemplatesResult>
parse_list_resource_templates_result(simdjson::ondemand::value &val) noexcept {
  ListResourceTemplatesResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "nextCursor", result.nextCursor);
  simdjson::ondemand::array r_arr;
  if (obj["resourceTemplates"].get_array().get(r_arr))
    return std::unexpected("Missing resourceTemplates");
  for (auto r : r_arr) {
    auto r_val = r.value_unsafe();
    auto res = parse_resource_template(r_val);
    if (res.has_value())
      result.resourceTemplates.push_back(res.operator*());
    else
      return std::unexpected(res.error());
  }
  return result;
}

McpResult<ReadResourceRequest>
parse_read_resource_request(simdjson::ondemand::value &val) noexcept {
  ReadResourceRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view u_sv;
  if (p_obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.params.uri = std::string(u_sv);
  return result;
}

McpResult<ReadResourceResult>
parse_read_resource_result(simdjson::ondemand::value &val) noexcept {
  ReadResourceResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  simdjson::ondemand::array c_arr;
  if (obj["contents"].get_array().get(c_arr))
    return std::unexpected("Missing contents");
  for (auto c : c_arr) {
    result.contents_json.push_back(std::string(c.raw_json().value_unsafe()));
  }
  return result;
}

McpResult<ResourceUpdatedNotification>
parse_resource_updated_notification(simdjson::ondemand::value &val) noexcept {
  ResourceUpdatedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view u_sv;
  if (p_obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.params.uri = std::string(u_sv);
  return result;
}

McpResult<ResourceListChangedNotification>
parse_resource_list_changed_notification(
    simdjson::ondemand::value &val) noexcept {
  ResourceListChangedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ResourceListChangedNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }
  return result;
}

McpResult<ResourceReference>
parse_resource_reference(simdjson::ondemand::value &val) noexcept {
  ResourceReference result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view t_sv;
  if (obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing type");
  result.type = std::string(t_sv);
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  return result;
}

/// @brief Internal helper or implementation for `parse_result`.
McpResult<Result> parse_result(simdjson::ondemand::value &val) noexcept {
  Result result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  return result;
}

/// @brief Internal helper or implementation for `parse_root`.
McpResult<Root> parse_root(simdjson::ondemand::value &val) noexcept {
  Root result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  set_opt_string(obj, "name", result.name);
  return result;
}

McpResult<RootsListChangedNotification>
parse_roots_list_changed_notification(simdjson::ondemand::value &val) noexcept {
  RootsListChangedNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    RootsListChangedNotificationParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }
  return result;
}

McpResult<ListRootsRequest>
parse_list_roots_request(simdjson::ondemand::value &val) noexcept {
  ListRootsRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListRootsRequestParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }
  return result;
}

McpResult<ListRootsResult>
parse_list_roots_result(simdjson::ondemand::value &val) noexcept {
  ListRootsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  simdjson::ondemand::array r_arr;
  if (obj["roots"].get_array().get(r_arr))
    return std::unexpected("Missing roots");
  for (auto r : r_arr) {
    auto r_val = r.value_unsafe();
    auto pt = parse_root(r_val);
    if (pt.has_value())
      result.roots.push_back(pt.operator*());
    else
      return std::unexpected(pt.error());
  }
  return result;
}

McpResult<SetLevelRequest>
parse_set_level_request(simdjson::ondemand::value &val) noexcept {
  SetLevelRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view l_sv;
  if (p_obj["level"].get_string().get(l_sv))
    return std::unexpected("Missing level");
  result.params.level = std::string(l_sv);
  return result;
}

McpResult<LoggingMessageNotification>
parse_logging_message_notification(simdjson::ondemand::value &val) noexcept {
  LoggingMessageNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view l_sv;
  if (p_obj["level"].get_string().get(l_sv))
    return std::unexpected("Missing level");
  result.params.level = std::string(l_sv);
  set_opt_string(p_obj, "logger", result.params.logger);
  simdjson::ondemand::value d_val;
  if (p_obj["data"].get(d_val))
    return std::unexpected("Missing data");
  result.params.data = std::string(d_val.raw_json().value_unsafe());
  return result;
}

McpResult<ProgressNotification>
parse_progress_notification(simdjson::ondemand::value &val) noexcept {
  ProgressNotification result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");

  simdjson::ondemand::value pt_val;
  if (p_obj["progressToken"].get(pt_val))
    return std::unexpected("Missing progressToken");
  result.params.progressToken =
      parse_request_id(pt_val); // reusing parse_request_id as it perfectly
                                // extracts string or number
  if (result.params.progressToken == "null")
    return std::unexpected("Invalid progressToken");

  double prg;
  if (p_obj["progress"].get_double().get(prg))
    return std::unexpected("Missing progress");
  result.params.progress = prg;

  set_opt_double(p_obj, "total", result.params.total);

  return result;
}

McpResult<EmptyResult>
parse_empty_result(simdjson::ondemand::value &val) noexcept {
  EmptyResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  return result;
}

McpResult<SamplingMessage>
parse_sampling_message(simdjson::ondemand::value &val) noexcept {
  SamplingMessage result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view r_sv;
  if (obj["role"].get_string().get(r_sv))
    return std::unexpected("Missing role");
  result.role = std::string(r_sv);
  simdjson::ondemand::value c_val;
  if (obj["content"].get(c_val))
    return std::unexpected("Missing content");
  result.content_json = std::string(c_val.raw_json().value_unsafe());
  return result;
}

McpResult<PingRequest>
parse_ping_request(simdjson::ondemand::value &val) noexcept {
  PingRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    PingRequestParams params;
    set_opt_raw(p_obj, "_meta", params._meta);
    result.params = params;
  }
  return result;
}

McpResult<SubscribeRequest>
parse_subscribe_request(simdjson::ondemand::value &val) noexcept {
  SubscribeRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view u_sv;
  if (p_obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.params.uri = std::string(u_sv);
  return result;
}

McpResult<UnsubscribeRequest>
parse_unsubscribe_request(simdjson::ondemand::value &val) noexcept {
  UnsubscribeRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (obj["params"].get_object().get(p_obj))
    return std::unexpected("Missing params");
  std::string_view u_sv;
  if (p_obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.params.uri = std::string(u_sv);
  return result;
}

McpResult<ListToolsRequest>
parse_list_tools_request(simdjson::ondemand::value &val) noexcept {
  ListToolsRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Missing method");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListToolsRequestParams params;
    set_opt_string(p_obj, "cursor", params.cursor);
    result.params = params;
  }
  return result;
}

McpResult<ListToolsResult>
parse_list_tools_result(simdjson::ondemand::value &val) noexcept {
  ListToolsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  set_opt_raw(obj, "_meta", result._meta);
  set_opt_string(obj, "nextCursor", result.nextCursor);
  simdjson::ondemand::array t_arr;
  if (obj["tools"].get_array().get(t_arr))
    return std::unexpected("Missing tools");
  for (auto t : t_arr) {
    auto t_val = t.value_unsafe();
    auto pt = parse_tool(t_val);
    if (pt.has_value())
      result.tools.push_back(pt.operator*());
    else
      return std::unexpected(pt.error());
  }
  return result;
}

} // namespace cdd_cpp::mcp
