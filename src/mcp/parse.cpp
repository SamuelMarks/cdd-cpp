#include "parse.hpp"
#include <iostream>

namespace cdd_cpp::mcp {

std::expected<Annotated, std::string>
parse_annotated(simdjson::ondemand::value &val) noexcept {
  Annotated result;
  simdjson::ondemand::object obj;
  auto error = val.get_object().get(obj);
  if (error) {
    return std::unexpected("Expected object for Annotated");
  }

  simdjson::ondemand::object annotations_obj;
  if (!obj["annotations"].get_object().get(annotations_obj)) {
    Annotations annotations;
    std::string_view audience_sv;
    if (!annotations_obj["audience"].get_string().get(audience_sv)) {
      annotations.audience = std::string(audience_sv);
    }
    double priority_val;
    if (!annotations_obj["priority"].get_double().get(priority_val)) {
      annotations.priority = priority_val;
    }
    result.annotations = annotations;
  }

  return result;
}

std::expected<BlobResourceContents, std::string>
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

  std::string_view mime_sv;
  if (!obj["mimeType"].get_string().get(mime_sv)) {
    result.mimeType = std::string(mime_sv);
  }

  return result;
}

std::expected<CallToolRequest, std::string>
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

  simdjson::ondemand::object args_obj;
  if (!params_obj["arguments"].get_object().get(args_obj)) {
    std::string_view args_str;
    if (!args_obj.raw_json().get(args_str)) {
      result.params.arguments = std::string(args_str);
    }
  }

  return result;
}

std::optional<Annotations> parse_annotations(simdjson::ondemand::object &obj) {
  simdjson::ondemand::object annotations_obj;
  if (!obj["annotations"].get_object().get(annotations_obj)) {
    Annotations annotations;
    std::string_view audience_sv;
    if (!annotations_obj["audience"].get_string().get(audience_sv)) {
      annotations.audience = std::string(audience_sv);
    }
    double priority_val;
    if (!annotations_obj["priority"].get_double().get(priority_val)) {
      annotations.priority = priority_val;
    }
    return annotations;
  }
  return std::nullopt;
}

std::expected<TextContent, std::string>
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

std::expected<ImageContent, std::string>
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

std::expected<TextResourceContents, std::string>
parse_text_resource_contents(simdjson::ondemand::value &val) noexcept {
  TextResourceContents result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  std::string_view m_sv;
  if (!obj["mimeType"].get_string().get(m_sv))
    result.mimeType = std::string(m_sv);
  std::string_view t_sv;
  if (obj["text"].get_string().get(t_sv))
    return std::unexpected("Missing text");
  result.text = std::string(t_sv);
  return result;
}

std::expected<EmbeddedResource, std::string>
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
  std::string_view r_sv;
  if (!res_obj.raw_json().get(r_sv))
    result.resource_json = std::string(r_sv);

  result.annotations = parse_annotations(obj);
  return result;
}

std::expected<CallToolResult, std::string>
parse_call_tool_result(simdjson::ondemand::value &val) noexcept {
  CallToolResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::object meta_obj;
  if (!obj["_meta"].get_object().get(meta_obj)) {
    std::string_view m_sv;
    if (!meta_obj.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  simdjson::ondemand::array content_arr;
  if (obj["content"].get_array().get(content_arr))
    return std::unexpected("Missing content");
  std::string_view c_sv;
  if (!content_arr.raw_json().get(c_sv))
    result.content_json = std::string(c_sv);

  bool is_err;
  if (!obj["isError"].get_bool().get(is_err))
    result.isError = is_err;

  return result;
}

std::expected<CancelledNotification, std::string>
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
  simdjson::ondemand::json_type t;
  if (!req_id_val.type().get(t)) {
    if (t == simdjson::ondemand::json_type::string) {
      std::string_view r_sv;
      if (!req_id_val.get_string().get(r_sv))
        result.params.requestId = std::string(r_sv);
    } else if (t == simdjson::ondemand::json_type::number) {
      int64_t v;
      if (!req_id_val.get_int64().get(v))
        result.params.requestId = std::to_string(v);
    }
  }

  std::string_view rs_sv;
  if (!p_obj["reason"].get_string().get(rs_sv))
    result.params.reason = std::string(rs_sv);

  return result;
}

std::expected<ClientCapabilities, std::string>
parse_client_capabilities(simdjson::ondemand::value &val) noexcept {
  ClientCapabilities result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::object exp_obj;
  if (!obj["experimental"].get_object().get(exp_obj)) {
    std::string_view e_sv;
    if (!exp_obj.raw_json().get(e_sv))
      result.experimental = std::string(e_sv);
  }

  simdjson::ondemand::object roots_obj;
  if (!obj["roots"].get_object().get(roots_obj)) {
    ClientCapabilitiesRoots roots;
    bool lc;
    if (!roots_obj["listChanged"].get_bool().get(lc))
      roots.listChanged = lc;
    result.roots = roots;
  }

  simdjson::ondemand::object samp_obj;
  if (!obj["sampling"].get_object().get(samp_obj)) {
    std::string_view s_sv;
    if (!samp_obj.raw_json().get(s_sv))
      result.sampling = std::string(s_sv);
  }

  return result;
}

std::expected<CompleteRequest, std::string>
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
  std::string_view r_sv;
  if (!ref_obj.raw_json().get(r_sv))
    result.params.ref_json = std::string(r_sv);

  return result;
}

std::expected<CompleteResult, std::string>
parse_complete_result(simdjson::ondemand::value &val) noexcept {
  CompleteResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::object meta_obj;
  if (!obj["_meta"].get_object().get(meta_obj)) {
    std::string_view m_sv;
    if (!meta_obj.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  simdjson::ondemand::object c_obj;
  if (obj["completion"].get_object().get(c_obj))
    return std::unexpected("Missing completion");

  simdjson::ondemand::array v_arr;
  if (c_obj["values"].get_array().get(v_arr))
    return std::unexpected("Missing completion.values");
  for (auto v : v_arr) {
    std::string_view s;
    if (!v.get_string().get(s))
      result.completion.values.push_back(std::string(s));
  }

  int64_t total_val;
  if (!c_obj["total"].get_int64().get(total_val))
    result.completion.total = total_val;

  bool hm_val;
  if (!c_obj["hasMore"].get_bool().get(hm_val))
    result.completion.hasMore = hm_val;

  return result;
}

std::expected<ModelHint, std::string>
parse_model_hint(simdjson::ondemand::value &val) noexcept {
  ModelHint result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view n_sv;
  if (!obj["name"].get_string().get(n_sv))
    result.name = std::string(n_sv);
  return result;
}

std::expected<ModelPreferences, std::string>
parse_model_preferences(simdjson::ondemand::value &val) noexcept {
  ModelPreferences result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  double cp;
  if (!obj["costPriority"].get_double().get(cp))
    result.costPriority = cp;

  double ip;
  if (!obj["intelligencePriority"].get_double().get(ip))
    result.intelligencePriority = ip;

  double sp;
  if (!obj["speedPriority"].get_double().get(sp))
    result.speedPriority = sp;

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

std::expected<CreateMessageRequest, std::string>
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
  std::string_view msg_sv;
  if (!msg_arr.raw_json().get(msg_sv))
    result.params.messages_json = std::string(msg_sv);

  simdjson::ondemand::value ic_val;
  if (!p_obj["includeContext"].get(ic_val)) {
    std::string_view ic_sv;
    if (!ic_val.raw_json().get(ic_sv))
      result.params.includeContext = std::string(ic_sv);
  }

  simdjson::ondemand::value md_val;
  if (!p_obj["metadata"].get(md_val)) {
    std::string_view md_sv;
    if (!md_val.raw_json().get(md_sv))
      result.params.metadata = std::string(md_sv);
  }

  simdjson::ondemand::value mp_val;
  if (!p_obj["modelPreferences"].get(mp_val)) {
    auto mp = parse_model_preferences(mp_val);
    if (mp.has_value())
      result.params.modelPreferences = mp.operator*();
  }

  simdjson::ondemand::array ss_arr;
  if (!p_obj["stopSequences"].get_array().get(ss_arr)) {
    std::vector<std::string> ss;
    for (auto s : ss_arr) {
      std::string_view sv;
      if (!s.get_string().get(sv))
        ss.push_back(std::string(sv));
    }
    result.params.stopSequences = ss;
  }

  std::string_view sp_sv;
  if (!p_obj["systemPrompt"].get_string().get(sp_sv))
    result.params.systemPrompt = std::string(sp_sv);

  double t;
  if (!p_obj["temperature"].get_double().get(t))
    result.params.temperature = t;

  return result;
}

std::expected<CreateMessageResult, std::string>
parse_create_message_result(simdjson::ondemand::value &val) noexcept {
  CreateMessageResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  simdjson::ondemand::value c_val;
  if (obj["content"].get(c_val))
    return std::unexpected("Missing content");
  std::string_view c_sv;
  if (!c_val.raw_json().get(c_sv))
    result.content_json = std::string(c_sv);

  std::string_view mod_sv;
  if (obj["model"].get_string().get(mod_sv))
    return std::unexpected("Missing model");
  result.model = std::string(mod_sv);

  std::string_view role_sv;
  if (obj["role"].get_string().get(role_sv))
    return std::unexpected("Missing role");
  result.role = std::string(role_sv);

  std::string_view sr_sv;
  if (!obj["stopReason"].get_string().get(sr_sv))
    result.stopReason = std::string(sr_sv);

  return result;
}

std::expected<Tool, std::string>
parse_tool(simdjson::ondemand::value &val) noexcept {
  Tool result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);

  simdjson::ondemand::object is_obj;
  if (obj["inputSchema"].get_object().get(is_obj))
    return std::unexpected("Missing inputSchema");

  std::string_view t_sv;
  if (is_obj["type"].get_string().get(t_sv))
    return std::unexpected("Missing inputSchema.type");
  result.inputSchema.type = std::string(t_sv);

  simdjson::ondemand::value p_val;
  if (!is_obj["properties"].get(p_val)) {
    std::string_view p_sv;
    if (!p_val.raw_json().get(p_sv))
      result.inputSchema.properties_json = std::string(p_sv);
  }

  simdjson::ondemand::array req_arr;
  if (!is_obj["required"].get_array().get(req_arr)) {
    std::vector<std::string> req;
    for (auto r : req_arr) {
      std::string_view r_sv;
      if (!r.get_string().get(r_sv))
        req.push_back(std::string(r_sv));
    }
    result.inputSchema.required = req;
  }

  return result;
}

std::expected<ToolListChangedNotification, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }

  return result;
}

std::expected<PaginatedRequest, std::string>
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
    std::string_view c_sv;
    if (!p_obj["cursor"].get_string().get(c_sv))
      result.params.cursor = std::string(c_sv);
  }

  return result;
}

std::expected<PaginatedResult, std::string>
parse_paginated_result(simdjson::ondemand::value &val) noexcept {
  PaginatedResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  std::string_view nc_sv;
  if (!obj["nextCursor"].get_string().get(nc_sv))
    result.nextCursor = std::string(nc_sv);

  return result;
}

std::expected<Implementation, std::string>
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

std::expected<InitializeRequest, std::string>
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
    auto c_res = parse_client_capabilities(cap_val);
    if (c_res.has_value())
      result.params.capabilities = c_res.operator*();
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

std::expected<ServerCapabilities, std::string>
parse_server_capabilities(simdjson::ondemand::value &val) noexcept {
  ServerCapabilities result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::object exp_obj;
  if (!obj["experimental"].get_object().get(exp_obj)) {
    std::string_view e_sv;
    if (!exp_obj.raw_json().get(e_sv))
      result.experimental = std::string(e_sv);
  }

  simdjson::ondemand::object log_obj;
  if (!obj["logging"].get_object().get(log_obj)) {
    result.logging = ServerCapabilitiesLogging{};
  }

  simdjson::ondemand::object pr_obj;
  if (!obj["prompts"].get_object().get(pr_obj)) {
    ServerCapabilitiesPrompts prompts;
    bool lc;
    if (!pr_obj["listChanged"].get_bool().get(lc))
      prompts.listChanged = lc;
    result.prompts = prompts;
  }

  simdjson::ondemand::object res_obj;
  if (!obj["resources"].get_object().get(res_obj)) {
    ServerCapabilitiesResources resources;
    bool lc;
    if (!res_obj["listChanged"].get_bool().get(lc))
      resources.listChanged = lc;
    bool sub;
    if (!res_obj["subscribe"].get_bool().get(sub))
      resources.subscribe = sub;
    result.resources = resources;
  }

  simdjson::ondemand::object tools_obj;
  if (!obj["tools"].get_object().get(tools_obj)) {
    ServerCapabilitiesTools tools;
    bool lc;
    if (!tools_obj["listChanged"].get_bool().get(lc))
      tools.listChanged = lc;
    result.tools = tools;
  }

  return result;
}

std::expected<InitializeResult, std::string>
parse_initialize_result(simdjson::ondemand::value &val) noexcept {
  InitializeResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  std::string_view pv_sv;
  if (obj["protocolVersion"].get_string().get(pv_sv))
    return std::unexpected("Missing protocolVersion");
  result.protocolVersion = std::string(pv_sv);

  std::string_view inst_sv;
  if (!obj["instructions"].get_string().get(inst_sv))
    result.instructions = std::string(inst_sv);

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

std::expected<InitializedNotification, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }

  return result;
}

std::string parse_request_id(simdjson::ondemand::value &val) noexcept {
  simdjson::ondemand::json_type t;
  if (!val.type().get(t)) {
    if (t == simdjson::ondemand::json_type::string) {
      std::string_view r_sv;
      if (!val.get_string().get(r_sv))
        return "\"" + std::string(r_sv) + "\"";
    } else if (t == simdjson::ondemand::json_type::number) {
      int64_t v;
      if (!val.get_int64().get(v))
        return std::to_string(v);
    }
  }
  return "null";
}

std::expected<JSONRPCError, std::string>
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

  simdjson::ondemand::value data_val;
  if (!err_obj["data"].get(data_val)) {
    std::string_view d_sv;
    if (!data_val.raw_json().get(d_sv))
      result.error.data = std::string(d_sv);
  }

  return result;
}

std::expected<JSONRPCNotification, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    std::string_view raw_sv;
    if (!p_obj.raw_json().get(raw_sv))
      params._raw = std::string(raw_sv);
    result.params = params;
  }

  return result;
}

std::expected<JSONRPCRequest, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    std::string_view raw_sv;
    if (!p_obj.raw_json().get(raw_sv))
      params._raw = std::string(raw_sv);
    result.params = params;
  }

  return result;
}

std::expected<JSONRPCResponse, std::string>
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
  std::string_view r_sv;
  if (!res_val.raw_json().get(r_sv))
    result.result_json = std::string(r_sv);

  return result;
}

std::expected<ListPromptsRequest, std::string>
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
    std::string_view c_sv;
    if (!p_obj["cursor"].get_string().get(c_sv))
      params.cursor = std::string(c_sv);
    result.params = params;
  }

  return result;
}

std::expected<PromptArgument, std::string>
parse_prompt_argument(simdjson::ondemand::value &val) noexcept {
  PromptArgument result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);

  bool req;
  if (!obj["required"].get_bool().get(req))
    result.required = req;

  return result;
}

std::expected<Prompt, std::string>
parse_prompt(simdjson::ondemand::value &val) noexcept {
  Prompt result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  std::string_view n_sv;
  if (obj["name"].get_string().get(n_sv))
    return std::unexpected("Missing name");
  result.name = std::string(n_sv);

  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);

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

std::expected<ListPromptsResult, std::string>
parse_list_prompts_result(simdjson::ondemand::value &val) noexcept {
  ListPromptsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  std::string_view nc_sv;
  if (!obj["nextCursor"].get_string().get(nc_sv))
    result.nextCursor = std::string(nc_sv);

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

std::expected<GetPromptRequest, std::string>
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

  simdjson::ondemand::value arg_val;
  if (!p_obj["arguments"].get(arg_val)) {
    std::string_view a_sv;
    if (!arg_val.raw_json().get(a_sv))
      result.params.arguments = std::string(a_sv);
  }

  return result;
}

std::expected<GetPromptResult, std::string>
parse_get_prompt_result(simdjson::ondemand::value &val) noexcept {
  GetPromptResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");

  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }

  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);

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

    std::string_view c_sv;
    if (!c_val.raw_json().get(c_sv))
      pm.content_json = std::string(c_sv);
    else
      return std::unexpected("Failed to get raw JSON for content");

    result.messages.push_back(pm);
  }

  return result;
}

std::expected<PromptListChangedNotification, std::string>
parse_prompt_list_changed_notification(
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }

  return result;
}

std::expected<PromptReference, std::string>
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

std::expected<ListResourcesRequest, std::string>
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
    std::string_view c_sv;
    if (!p_obj["cursor"].get_string().get(c_sv))
      params.cursor = std::string(c_sv);
    result.params = params;
  }
  return result;
}

std::expected<Resource, std::string>
parse_resource(simdjson::ondemand::value &val) noexcept {
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
  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);
  std::string_view m_sv;
  if (!obj["mimeType"].get_string().get(m_sv))
    result.mimeType = std::string(m_sv);
  result.annotations = parse_annotations(obj);
  return result;
}

std::expected<ListResourcesResult, std::string>
parse_list_resources_result(simdjson::ondemand::value &val) noexcept {
  ListResourcesResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  std::string_view nc_sv;
  if (!obj["nextCursor"].get_string().get(nc_sv))
    result.nextCursor = std::string(nc_sv);
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

std::expected<ListResourceTemplatesRequest, std::string>
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
    std::string_view c_sv;
    if (!p_obj["cursor"].get_string().get(c_sv))
      params.cursor = std::string(c_sv);
    result.params = params;
  }
  return result;
}

std::expected<ResourceTemplate, std::string>
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
  std::string_view d_sv;
  if (!obj["description"].get_string().get(d_sv))
    result.description = std::string(d_sv);
  std::string_view m_sv;
  if (!obj["mimeType"].get_string().get(m_sv))
    result.mimeType = std::string(m_sv);
  result.annotations = parse_annotations(obj);
  return result;
}

std::expected<ListResourceTemplatesResult, std::string>
parse_list_resource_templates_result(simdjson::ondemand::value &val) noexcept {
  ListResourceTemplatesResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  std::string_view nc_sv;
  if (!obj["nextCursor"].get_string().get(nc_sv))
    result.nextCursor = std::string(nc_sv);
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

std::expected<ReadResourceRequest, std::string>
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

std::expected<ReadResourceResult, std::string>
parse_read_resource_result(simdjson::ondemand::value &val) noexcept {
  ReadResourceResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  simdjson::ondemand::array c_arr;
  if (obj["contents"].get_array().get(c_arr))
    return std::unexpected("Missing contents");
  for (auto c : c_arr) {
    std::string_view c_sv;
    if (!c.raw_json().get(c_sv))
      result.contents_json.push_back(std::string(c_sv));
  }
  return result;
}

std::expected<ResourceUpdatedNotification, std::string>
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

std::expected<ResourceListChangedNotification, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }
  return result;
}

std::expected<ResourceReference, std::string>
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

std::expected<Result, std::string>
parse_result(simdjson::ondemand::value &val) noexcept {
  Result result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  return result;
}

std::expected<Root, std::string>
parse_root(simdjson::ondemand::value &val) noexcept {
  Root result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view u_sv;
  if (obj["uri"].get_string().get(u_sv))
    return std::unexpected("Missing uri");
  result.uri = std::string(u_sv);
  std::string_view n_sv;
  if (!obj["name"].get_string().get(n_sv))
    result.name = std::string(n_sv);
  return result;
}

std::expected<RootsListChangedNotification, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }
  return result;
}

std::expected<ListRootsRequest, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }
  return result;
}

std::expected<ListRootsResult, std::string>
parse_list_roots_result(simdjson::ondemand::value &val) noexcept {
  ListRootsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
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

std::expected<SetLevelRequest, std::string>
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

std::expected<LoggingMessageNotification, std::string>
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
  std::string_view lg_sv;
  if (!p_obj["logger"].get_string().get(lg_sv))
    result.params.logger = std::string(lg_sv);
  simdjson::ondemand::value d_val;
  if (p_obj["data"].get(d_val))
    return std::unexpected("Missing data");
  std::string_view d_sv;
  if (!d_val.raw_json().get(d_sv))
    result.params.data = std::string(d_sv);
  return result;
}

std::expected<ProgressNotification, std::string>
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

  double t;
  if (!p_obj["total"].get_double().get(t))
    result.params.total = t;

  return result;
}

std::expected<EmptyResult, std::string>
parse_empty_result(simdjson::ondemand::value &val) noexcept {
  EmptyResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  return result;
}

std::expected<SamplingMessage, std::string>
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
  std::string_view c_sv;
  if (!c_val.raw_json().get(c_sv))
    result.content_json = std::string(c_sv);
  return result;
}

std::expected<PingRequest, std::string>
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
    simdjson::ondemand::value meta_val;
    if (!p_obj["_meta"].get(meta_val)) {
      std::string_view meta_sv;
      if (!meta_val.raw_json().get(meta_sv))
        params._meta = std::string(meta_sv);
    }
    result.params = params;
  }
  return result;
}

std::expected<SubscribeRequest, std::string>
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

std::expected<UnsubscribeRequest, std::string>
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

std::expected<ListToolsRequest, std::string>
parse_list_tools_request(simdjson::ondemand::value &val) noexcept {
  ListToolsRequest result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  std::string_view m_sv;
  if (obj["method"].get_string().get(m_sv))
    return std::unexpected("Missing method");
  result.method = std::string(m_sv);
  simdjson::ondemand::object p_obj;
  if (!obj["params"].get_object().get(p_obj)) {
    ListToolsRequestParams params;
    std::string_view c_sv;
    if (!p_obj["cursor"].get_string().get(c_sv))
      params.cursor = std::string(c_sv);
    result.params = params;
  }
  return result;
}

std::expected<ListToolsResult, std::string>
parse_list_tools_result(simdjson::ondemand::value &val) noexcept {
  ListToolsResult result;
  simdjson::ondemand::object obj;
  if (val.get_object().get(obj))
    return std::unexpected("Expected object");
  simdjson::ondemand::value meta_val;
  if (!obj["_meta"].get(meta_val)) {
    std::string_view m_sv;
    if (!meta_val.raw_json().get(m_sv))
      result._meta = std::string(m_sv);
  }
  std::string_view nc_sv;
  if (!obj["nextCursor"].get_string().get(nc_sv))
    result.nextCursor = std::string(nc_sv);
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
