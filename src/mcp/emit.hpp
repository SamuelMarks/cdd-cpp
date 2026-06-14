// GCOV_EXCL_BR_START
#pragma once
#include "../utils/json_writer.hpp"
#include "models.hpp"
#include <string>

namespace cdd_cpp::mcp {

/// @brief Auto-generated doc.
void emit_annotated(const Annotated &annotated, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_blob_resource_contents(const BlobResourceContents &blob,
                                 utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_call_tool_request(const CallToolRequest &req,
                            utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_text_content(const TextContent &text, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_image_content(const ImageContent &image,
                        utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_text_resource_contents(const TextResourceContents &text,
                                 utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_embedded_resource(const EmbeddedResource &embed,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_call_tool_result(const CallToolResult &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_cancelled_notification(const CancelledNotification &notif,
                                 utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_client_capabilities(const ClientCapabilities &caps,
                              utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_complete_request(const CompleteRequest &req,
                           utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_complete_result(const CompleteResult &res,
                          utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_model_hint(const ModelHint &hint, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_model_preferences(const ModelPreferences &prefs,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_create_message_request(const CreateMessageRequest &req,
                                 utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_create_message_result(const CreateMessageResult &res,
                                utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_tool(const Tool &tool, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_tool_list_changed_notification(
    const ToolListChangedNotification &notif, utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_paginated_request(const PaginatedRequest &req,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_paginated_result(const PaginatedResult &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_implementation(const Implementation &impl,
                         utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_initialize_request(const InitializeRequest &req,
                             utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_server_capabilities(const ServerCapabilities &caps,
                              utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_initialize_result(const InitializeResult &res,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_initialized_notification(const InitializedNotification &notif,
                                   utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_jsonrpc_error(const JSONRPCError &err,
                        utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_jsonrpc_notification(const JSONRPCNotification &notif,
                               utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_jsonrpc_request(const JSONRPCRequest &req,
                          utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_jsonrpc_response(const JSONRPCResponse &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_list_prompts_request(const ListPromptsRequest &req,
                               utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_prompt_argument(const PromptArgument &arg,
                          utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_prompt(const Prompt &p, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_prompts_result(const ListPromptsResult &res,
                              utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_get_prompt_request(const GetPromptRequest &req,
                             utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_get_prompt_result(const GetPromptResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_prompt_list_changed_notification(
    const PromptListChangedNotification &notif, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_prompt_reference(const PromptReference &ref,
                           utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_list_resources_request(const ListResourcesRequest &req,
                                 utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_resource(const Resource &res, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_resources_result(const ListResourcesResult &res,
                                utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_list_resource_templates_request(
    const ListResourceTemplatesRequest &req, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_resource_template(const ResourceTemplate &rt,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_resource_templates_result(const ListResourceTemplatesResult &res,
                                         utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_read_resource_request(const ReadResourceRequest &req,
                                utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_read_resource_result(const ReadResourceResult &res,
                               utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_resource_updated_notification(
    const ResourceUpdatedNotification &notif, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_resource_list_changed_notification(
    const ResourceListChangedNotification &notif,
    utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_resource_reference(const ResourceReference &ref,
                             utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_result(const Result &res, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_root(const Root &root, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_roots_list_changed_notification(
    const RootsListChangedNotification &notif, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_roots_request(const ListRootsRequest &req,
                             utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_roots_result(const ListRootsResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_empty_result(const EmptyResult &res, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_sampling_message(const SamplingMessage &msg,
                           utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_ping_request(const PingRequest &req, utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_subscribe_request(const SubscribeRequest &req,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_unsubscribe_request(const UnsubscribeRequest &req,
                              utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_tools_request(const ListToolsRequest &req,
                             utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_list_tools_result(const ListToolsResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Auto-generated doc.
void emit_set_level_request(const SetLevelRequest &req,
                            utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_logging_message_notification(const LoggingMessageNotification &notif,
                                       utils::JsonWriter &jw) noexcept;
/// @brief Auto-generated doc.
void emit_progress_notification(const ProgressNotification &notif,
                                utils::JsonWriter &jw) noexcept;

} // namespace cdd_cpp::mcp

// GCOV_EXCL_BR_STOP
