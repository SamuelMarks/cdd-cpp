#include "emit.hpp"
#include <algorithm>

namespace cdd_cpp::mcp {

void emit_annotated(const Annotated &annotated,
                    utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (annotated.annotations.has_value()) {
    jw.key("annotations");
    jw.start_object();
    if (annotated.annotations->audience.has_value()) {
      jw.key_value("audience", annotated.annotations->audience.operator*());
    }
    if (annotated.annotations->priority.has_value()) {
      jw.key_value("priority", annotated.annotations->priority.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_blob_resource_contents(const BlobResourceContents &blob,
                                 utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("blob", blob.blob);
  if (blob.mimeType.has_value()) {
    jw.key_value("mimeType", blob.mimeType.operator*());
  }
  jw.key_value("uri", blob.uri);
  jw.end_object();
}

void emit_call_tool_request(const CallToolRequest &req,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("name", req.params.name);
  if (req.params.arguments.has_value()) {
    jw.key("arguments");
    jw.raw_value(req.params.arguments.operator*());
  }
  jw.end_object();
  jw.end_object();
}

void emit_annotations(const std::optional<Annotations> &annotations,
                      utils::JsonWriter &jw) noexcept {
  if (annotations.has_value()) {
    jw.key("annotations");
    jw.start_object();
    if (annotations->audience.has_value()) {
      jw.key_value("audience", annotations->audience.operator*());
    }
    if (annotations->priority.has_value()) {
      jw.key_value("priority", annotations->priority.operator*());
    }
    jw.end_object();
  }
}

void emit_text_content(const TextContent &text,
                       utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("type", text.type);
  jw.key_value("text", text.text);
  emit_annotations(text.annotations, jw);
  jw.end_object();
}

void emit_image_content(const ImageContent &image,
                        utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("type", image.type);
  jw.key_value("data", image.data);
  jw.key_value("mimeType", image.mimeType);
  emit_annotations(image.annotations, jw);
  jw.end_object();
}

void emit_text_resource_contents(const TextResourceContents &text,
                                 utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("uri", text.uri);
  if (text.mimeType.has_value())
    jw.key_value("mimeType", text.mimeType.operator*());
  jw.key_value("text", text.text);
  jw.end_object();
}

void emit_embedded_resource(const EmbeddedResource &embed,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("type", embed.type);
  jw.key("resource");
  jw.raw_value(embed.resource_json);
  emit_annotations(embed.annotations, jw);
  jw.end_object();
}

void emit_call_tool_result(const CallToolResult &res,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key("content");
  jw.raw_value(res.content_json);
  if (res.isError.has_value()) {
    jw.key_value("isError", res.isError.operator*());
  }
  jw.end_object();
}

void emit_cancelled_notification(const CancelledNotification &notif,
                                 utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  jw.key("params");
  jw.start_object();

  // Check if requestId is a number or string
  bool is_number = !notif.params.requestId.empty() &&
                   std::all_of(notif.params.requestId.begin(),
                               notif.params.requestId.end(), ::isdigit);

  jw.key("requestId");
  if (is_number) {
    jw.raw_value(notif.params.requestId);
  } else {
    jw.value(notif.params.requestId);
  }

  if (notif.params.reason.has_value()) {
    jw.key_value("reason", notif.params.reason.operator*());
  }
  jw.end_object();
  jw.end_object();
}

void emit_client_capabilities(const ClientCapabilities &caps,
                              utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (caps.experimental.has_value()) {
    jw.key("experimental");
    jw.raw_value(caps.experimental.operator*());
  }
  if (caps.roots.has_value()) {
    jw.key("roots");
    jw.start_object();
    if (caps.roots->listChanged.has_value()) {
      jw.key_value("listChanged", caps.roots->listChanged.operator*());
    }
    jw.end_object();
  }
  if (caps.sampling.has_value()) {
    jw.key("sampling");
    jw.raw_value(caps.sampling.operator*());
  }
  jw.end_object();
}

void emit_complete_request(const CompleteRequest &req,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();

  jw.key("argument");
  jw.start_object();
  jw.key_value("name", req.params.argument.name);
  jw.key_value("value", req.params.argument.value);
  jw.end_object();

  jw.key("ref");
  jw.raw_value(req.params.ref_json);

  jw.end_object();
  jw.end_object();
}

void emit_complete_result(const CompleteResult &res,
                          utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key("completion");
  jw.start_object();

  jw.key("values");
  jw.start_array();
  for (const auto &v : res.completion.values) {
    jw.value(v);
  }
  jw.end_array();

  if (res.completion.total.has_value())
    jw.key_value("total", res.completion.total.operator*());
  if (res.completion.hasMore.has_value())
    jw.key_value("hasMore", res.completion.hasMore.operator*());

  jw.end_object();
  jw.end_object();
}

void emit_model_hint(const ModelHint &hint, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (hint.name.has_value())
    jw.key_value("name", hint.name.operator*());
  jw.end_object();
}

void emit_model_preferences(const ModelPreferences &prefs,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (prefs.costPriority.has_value())
    jw.key_value("costPriority", prefs.costPriority.operator*());
  if (prefs.intelligencePriority.has_value())
    jw.key_value("intelligencePriority",
                 prefs.intelligencePriority.operator*());
  if (prefs.speedPriority.has_value())
    jw.key_value("speedPriority", prefs.speedPriority.operator*());
  if (prefs.hints.has_value()) {
    jw.key("hints");
    jw.start_array();
    for (const auto &h : prefs.hints.operator*()) {
      emit_model_hint(h, jw);
    }
    jw.end_array();
  }
  jw.end_object();
}

void emit_create_message_request(const CreateMessageRequest &req,
                                 utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();

  jw.key_value("maxTokens", req.params.maxTokens);

  jw.key("messages");
  jw.raw_value(req.params.messages_json);

  if (req.params.includeContext.has_value()) {
    jw.key("includeContext");
    jw.raw_value(req.params.includeContext.operator*());
  }
  if (req.params.metadata.has_value()) {
    jw.key("metadata");
    jw.raw_value(req.params.metadata.operator*());
  }
  if (req.params.modelPreferences.has_value()) {
    jw.key("modelPreferences");
    emit_model_preferences(req.params.modelPreferences.operator*(), jw);
  }
  if (req.params.stopSequences.has_value()) {
    jw.key("stopSequences");
    jw.start_array();
    for (const auto &s : req.params.stopSequences.operator*()) {
      jw.value(s);
    }
    jw.end_array();
  }
  if (req.params.systemPrompt.has_value()) {
    jw.key_value("systemPrompt", req.params.systemPrompt.operator*());
  }
  if (req.params.temperature.has_value()) {
    jw.key_value("temperature", req.params.temperature.operator*());
  }

  jw.end_object();
  jw.end_object();
}

void emit_create_message_result(const CreateMessageResult &res,
                                utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key("content");
  jw.raw_value(res.content_json);

  jw.key_value("model", res.model);
  jw.key_value("role", res.role);

  if (res.stopReason.has_value()) {
    jw.key_value("stopReason", res.stopReason.operator*());
  }

  jw.end_object();
}

void emit_tool(const Tool &tool, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("name", tool.name);
  if (tool.description.has_value()) {
    jw.key_value("description", tool.description.operator*());
  }
  jw.key("inputSchema");
  jw.start_object();
  jw.key_value("type", tool.inputSchema.type);
  if (tool.inputSchema.properties_json.has_value()) {
    jw.key("properties");
    jw.raw_value(tool.inputSchema.properties_json.operator*());
  }
  if (tool.inputSchema.required.has_value()) {
    jw.key("required");
    jw.start_array();
    for (const auto &r : tool.inputSchema.required.operator*()) {
      jw.value(r);
    }
    jw.end_array();
  }
  jw.end_object();
  jw.end_object();
}

void emit_tool_list_changed_notification(
    const ToolListChangedNotification &notif, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (notif.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(notif.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_paginated_request(const PaginatedRequest &req,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.cursor.has_value()) {
    jw.key("params");
    jw.start_object();
    jw.key_value("cursor", req.params.cursor.operator*());
    jw.end_object();
  }
  jw.end_object();
}

void emit_paginated_result(const PaginatedResult &res,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.nextCursor.has_value()) {
    jw.key_value("nextCursor", res.nextCursor.operator*());
  }
  jw.end_object();
}

void emit_implementation(const Implementation &impl,
                         utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("name", impl.name);
  jw.key_value("version", impl.version);
  jw.end_object();
}

void emit_initialize_request(const InitializeRequest &req,
                             utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();

  jw.key_value("protocolVersion", req.params.protocolVersion);
  if (req.params.capabilities.has_value()) {
    jw.key("capabilities");
    emit_client_capabilities(req.params.capabilities.operator*(), jw);
  }

  jw.key("clientInfo");
  emit_implementation(req.params.clientInfo, jw);

  jw.end_object();
  jw.end_object();
}

void emit_server_capabilities(const ServerCapabilities &caps,
                              utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (caps.experimental.has_value()) {
    jw.key("experimental");
    jw.raw_value(caps.experimental.operator*());
  }
  if (caps.logging.has_value()) {
    jw.key("logging");
    jw.start_object();
    jw.end_object();
  }
  if (caps.prompts.has_value()) {
    jw.key("prompts");
    jw.start_object();
    if (caps.prompts->listChanged.has_value())
      jw.key_value("listChanged", caps.prompts->listChanged.operator*());
    jw.end_object();
  }
  if (caps.resources.has_value()) {
    jw.key("resources");
    jw.start_object();
    if (caps.resources->subscribe.has_value())
      jw.key_value("subscribe", caps.resources->subscribe.operator*());
    if (caps.resources->listChanged.has_value())
      jw.key_value("listChanged", caps.resources->listChanged.operator*());
    jw.end_object();
  }
  if (caps.tools.has_value()) {
    jw.key("tools");
    jw.start_object();
    if (caps.tools->listChanged.has_value())
      jw.key_value("listChanged", caps.tools->listChanged.operator*());
    jw.end_object();
  }
  jw.end_object();
}

void emit_initialize_result(const InitializeResult &res,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key_value("protocolVersion", res.protocolVersion);

  jw.key("capabilities");
  emit_server_capabilities(res.capabilities, jw);

  jw.key("serverInfo");
  emit_implementation(res.serverInfo, jw);

  if (res.instructions.has_value()) {
    jw.key_value("instructions", res.instructions.operator*());
  }
  jw.end_object();
}

void emit_initialized_notification(const InitializedNotification &notif,
                                   utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (notif.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(notif.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_jsonrpc_error(const JSONRPCError &err,
                        utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("jsonrpc", err.jsonrpc);
  jw.key("id");
  jw.raw_value(err.id);
  jw.key("error");
  jw.start_object();
  jw.key_value("code", err.error.code);
  jw.key_value("message", err.error.message);
  if (err.error.data.has_value()) {
    jw.key("data");
    jw.raw_value(err.error.data.operator*());
  }
  jw.end_object();
  jw.end_object();
}

void emit_jsonrpc_notification(const JSONRPCNotification &notif,
                               utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("jsonrpc", notif.jsonrpc);
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    if (notif.params->_raw.has_value()) {
      jw.raw_value(notif.params->_raw.operator*());
    } else {
      jw.start_object();
      if (notif.params->_meta.has_value()) {
        jw.key("_meta");
        jw.raw_value(notif.params->_meta.operator*());
      }
      jw.end_object();
    }
  }
  jw.end_object();
}

void emit_jsonrpc_request(const JSONRPCRequest &req,
                          utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("jsonrpc", req.jsonrpc);
  jw.key("id");
  jw.raw_value(req.id);
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    if (req.params->_raw.has_value()) {
      jw.raw_value(req.params->_raw.operator*());
    } else {
      jw.start_object();
      if (req.params->_meta.has_value()) {
        jw.key("_meta");
        jw.raw_value(req.params->_meta.operator*());
      }
      jw.end_object();
    }
  }
  jw.end_object();
}

void emit_jsonrpc_response(const JSONRPCResponse &res,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("jsonrpc", res.jsonrpc);
  jw.key("id");
  jw.raw_value(res.id);
  jw.key("result");
  jw.raw_value(res.result_json);
  jw.end_object();
}

void emit_list_prompts_request(const ListPromptsRequest &req,
                               utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->cursor.has_value()) {
      jw.key_value("cursor", req.params->cursor.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_prompt_argument(const PromptArgument &arg,
                          utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("name", arg.name);
  if (arg.description.has_value())
    jw.key_value("description", arg.description.operator*());
  if (arg.required.has_value())
    jw.key_value("required", arg.required.operator*());
  jw.end_object();
}

void emit_prompt(const Prompt &p, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("name", p.name);
  if (p.description.has_value())
    jw.key_value("description", p.description.operator*());
  if (p.arguments.has_value()) {
    jw.key("arguments");
    jw.start_array();
    for (const auto &a : p.arguments.operator*()) {
      emit_prompt_argument(a, jw);
    }
    jw.end_array();
  }
  jw.end_object();
}

void emit_list_prompts_result(const ListPromptsResult &res,
                              utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.nextCursor.has_value()) {
    jw.key_value("nextCursor", res.nextCursor.operator*());
  }
  jw.key("prompts");
  jw.start_array();
  for (const auto &p : res.prompts) {
    emit_prompt(p, jw);
  }
  jw.end_array();
  jw.end_object();
}

void emit_get_prompt_request(const GetPromptRequest &req,
                             utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("name", req.params.name);
  if (req.params.arguments.has_value()) {
    jw.key("arguments");
    jw.raw_value(req.params.arguments.operator*());
  }
  jw.end_object();
  jw.end_object();
}

void emit_get_prompt_result(const GetPromptResult &res,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.description.has_value()) {
    jw.key_value("description", res.description.operator*());
  }
  jw.key("messages");
  jw.start_array();
  for (const auto &m : res.messages) {
    jw.start_object();
    jw.key_value("role", m.role);
    jw.key("content");
    jw.raw_value(m.content_json);
    jw.end_object();
  }
  jw.end_array();
  jw.end_object();
}

void emit_prompt_list_changed_notification(
    const PromptListChangedNotification &notif,
    utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (notif.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(notif.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_prompt_reference(const PromptReference &ref,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("type", ref.type);
  jw.key_value("name", ref.name);
  jw.end_object();
}

void emit_list_resources_request(const ListResourcesRequest &req,
                                 utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->cursor.has_value())
      jw.key_value("cursor", req.params->cursor.operator*());
    jw.end_object();
  }
  jw.end_object();
}

void emit_resource(const Resource &res, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("uri", res.uri);
  jw.key_value("name", res.name);
  if (res.description.has_value())
    jw.key_value("description", res.description.operator*());
  if (res.mimeType.has_value())
    jw.key_value("mimeType", res.mimeType.operator*());
  emit_annotations(res.annotations, jw);
  jw.end_object();
}

void emit_list_resources_result(const ListResourcesResult &res,
                                utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.nextCursor.has_value())
    jw.key_value("nextCursor", res.nextCursor.operator*());
  jw.key("resources");
  jw.start_array();
  for (const auto &r : res.resources)
    emit_resource(r, jw);
  jw.end_array();
  jw.end_object();
}

void emit_list_resource_templates_request(
    const ListResourceTemplatesRequest &req, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->cursor.has_value())
      jw.key_value("cursor", req.params->cursor.operator*());
    jw.end_object();
  }
  jw.end_object();
}

void emit_resource_template(const ResourceTemplate &rt,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("uriTemplate", rt.uriTemplate);
  jw.key_value("name", rt.name);
  if (rt.description.has_value())
    jw.key_value("description", rt.description.operator*());
  if (rt.mimeType.has_value())
    jw.key_value("mimeType", rt.mimeType.operator*());
  emit_annotations(rt.annotations, jw);
  jw.end_object();
}

void emit_list_resource_templates_result(const ListResourceTemplatesResult &res,
                                         utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.nextCursor.has_value())
    jw.key_value("nextCursor", res.nextCursor.operator*());
  jw.key("resourceTemplates");
  jw.start_array();
  for (const auto &rt : res.resourceTemplates)
    emit_resource_template(rt, jw);
  jw.end_array();
  jw.end_object();
}

void emit_read_resource_request(const ReadResourceRequest &req,
                                utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("uri", req.params.uri);
  jw.end_object();
  jw.end_object();
}

void emit_read_resource_result(const ReadResourceResult &res,
                               utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key("contents");
  jw.start_array();
  for (const auto &c : res.contents_json)
    jw.raw_value(c);
  jw.end_array();
  jw.end_object();
}

void emit_resource_updated_notification(
    const ResourceUpdatedNotification &notif, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("uri", notif.params.uri);
  jw.end_object();
  jw.end_object();
}

void emit_resource_list_changed_notification(
    const ResourceListChangedNotification &notif,
    utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (notif.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(notif.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_resource_reference(const ResourceReference &ref,
                             utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("type", ref.type);
  jw.key_value("uri", ref.uri);
  jw.end_object();
}

void emit_result(const Result &res, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.end_object();
}

void emit_root(const Root &root, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("uri", root.uri);
  if (root.name.has_value())
    jw.key_value("name", root.name.operator*());
  jw.end_object();
}

void emit_roots_list_changed_notification(
    const RootsListChangedNotification &notif, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  if (notif.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (notif.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(notif.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_list_roots_request(const ListRootsRequest &req,
                             utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(req.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_list_roots_result(const ListRootsResult &res,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.key("roots");
  jw.start_array();
  for (const auto &r : res.roots)
    emit_root(r, jw);
  jw.end_array();
  jw.end_object();
}

void emit_set_level_request(const SetLevelRequest &req,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("level", req.params.level);
  jw.end_object();
  jw.end_object();
}

void emit_logging_message_notification(const LoggingMessageNotification &notif,
                                       utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("level", notif.params.level);
  if (notif.params.logger.has_value()) {
    jw.key_value("logger", notif.params.logger.operator*());
  }
  jw.key("data");
  jw.raw_value(notif.params.data);
  jw.end_object();
  jw.end_object();
}

void emit_progress_notification(const ProgressNotification &notif,
                                utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", notif.method);
  jw.key("params");
  jw.start_object();
  jw.key("progressToken");
  jw.raw_value(notif.params.progressToken);
  jw.key_value("progress", notif.params.progress);
  if (notif.params.total.has_value()) {
    jw.key_value("total", notif.params.total.operator*());
  }
  jw.end_object();
  jw.end_object();
}

void emit_empty_result(const EmptyResult &res, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  jw.end_object();
}

void emit_sampling_message(const SamplingMessage &msg,
                           utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("role", msg.role);
  jw.key("content");
  jw.raw_value(msg.content_json);
  jw.end_object();
}

void emit_ping_request(const PingRequest &req, utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->_meta.has_value()) {
      jw.key("_meta");
      jw.raw_value(req.params->_meta.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_subscribe_request(const SubscribeRequest &req,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("uri", req.params.uri);
  jw.end_object();
  jw.end_object();
}

void emit_unsubscribe_request(const UnsubscribeRequest &req,
                              utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  jw.key("params");
  jw.start_object();
  jw.key_value("uri", req.params.uri);
  jw.end_object();
  jw.end_object();
}

void emit_list_tools_request(const ListToolsRequest &req,
                             utils::JsonWriter &jw) noexcept {
  jw.start_object();
  jw.key_value("method", req.method);
  if (req.params.has_value()) {
    jw.key("params");
    jw.start_object();
    if (req.params->cursor.has_value()) {
      jw.key_value("cursor", req.params->cursor.operator*());
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_list_tools_result(const ListToolsResult &res,
                            utils::JsonWriter &jw) noexcept {
  jw.start_object();
  if (res._meta.has_value()) {
    jw.key("_meta");
    jw.raw_value(res._meta.operator*());
  }
  if (res.nextCursor.has_value()) {
    jw.key_value("nextCursor", res.nextCursor.operator*());
  }
  jw.key("tools");
  jw.start_array();
  for (const auto &t : res.tools) {
    emit_tool(t, jw);
  }
  jw.end_array();
  jw.end_object();
}

} // namespace cdd_cpp::mcp