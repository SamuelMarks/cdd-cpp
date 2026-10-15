/// @file emit.hpp
/// @brief Model Context Protocol (MCP) JSON-RPC message emitter declarations.

#pragma once
#include "../utils/json_writer.hpp"
#include "models.hpp"
#include <string>

namespace cdd_cpp::mcp {

/// @brief Serialize an Annotated object into a JsonWriter.
/// @param annotated The Annotated object to serialize.
/// @param jw Destination JsonWriter.
void emit_annotated(const Annotated &annotated, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a BlobResourceContents object into a JsonWriter.
/// @param blob The BlobResourceContents object to serialize.
/// @param jw Destination JsonWriter.
void emit_blob_resource_contents(const BlobResourceContents &blob,
                                 utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CallToolRequest object into a JsonWriter.
/// @param req The CallToolRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_call_tool_request(const CallToolRequest &req,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a TextContent object into a JsonWriter.
/// @param text The TextContent object to serialize.
/// @param jw Destination JsonWriter.
void emit_text_content(const TextContent &text, utils::JsonWriter &jw) noexcept;

/// @brief Serialize an ImageContent object into a JsonWriter.
/// @param image The ImageContent object to serialize.
/// @param jw Destination JsonWriter.
void emit_image_content(const ImageContent &image,
                        utils::JsonWriter &jw) noexcept;

/// @brief Serialize a TextResourceContents object into a JsonWriter.
/// @param text The TextResourceContents object to serialize.
/// @param jw Destination JsonWriter.
void emit_text_resource_contents(const TextResourceContents &text,
                                 utils::JsonWriter &jw) noexcept;

/// @brief Serialize an EmbeddedResource object into a JsonWriter.
/// @param embed The EmbeddedResource object to serialize.
/// @param jw Destination JsonWriter.
void emit_embedded_resource(const EmbeddedResource &embed,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CallToolResult object into a JsonWriter.
/// @param res The CallToolResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_call_tool_result(const CallToolResult &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CancelledNotification object into a JsonWriter.
/// @param notif The CancelledNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_cancelled_notification(const CancelledNotification &notif,
                                 utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ClientCapabilities object into a JsonWriter.
/// @param caps The ClientCapabilities object to serialize.
/// @param jw Destination JsonWriter.
void emit_client_capabilities(const ClientCapabilities &caps,
                              utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CompleteRequest object into a JsonWriter.
/// @param req The CompleteRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_complete_request(const CompleteRequest &req,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CompleteResult object into a JsonWriter.
/// @param res The CompleteResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_complete_result(const CompleteResult &res,
                          utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ModelHint object into a JsonWriter.
/// @param hint The ModelHint object to serialize.
/// @param jw Destination JsonWriter.
void emit_model_hint(const ModelHint &hint, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ModelPreferences object into a JsonWriter.
/// @param prefs The ModelPreferences object to serialize.
/// @param jw Destination JsonWriter.
void emit_model_preferences(const ModelPreferences &prefs,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CreateMessageRequest object into a JsonWriter.
/// @param req The CreateMessageRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_create_message_request(const CreateMessageRequest &req,
                                 utils::JsonWriter &jw) noexcept;

/// @brief Serialize a CreateMessageResult object into a JsonWriter.
/// @param res The CreateMessageResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_create_message_result(const CreateMessageResult &res,
                                utils::JsonWriter &jw) noexcept;

/// @brief Serialize a Tool definition into a JsonWriter.
/// @param tool The Tool object to serialize.
/// @param jw Destination JsonWriter.
void emit_tool(const Tool &tool, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ToolListChangedNotification object into a JsonWriter.
/// @param notif The ToolListChangedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_tool_list_changed_notification(
    const ToolListChangedNotification &notif, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PaginatedRequest object into a JsonWriter.
/// @param req The PaginatedRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_paginated_request(const PaginatedRequest &req,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PaginatedResult object into a JsonWriter.
/// @param res The PaginatedResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_paginated_result(const PaginatedResult &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize an Implementation object into a JsonWriter.
/// @param impl The Implementation object to serialize.
/// @param jw Destination JsonWriter.
void emit_implementation(const Implementation &impl,
                         utils::JsonWriter &jw) noexcept;

/// @brief Serialize an InitializeRequest object into a JsonWriter.
/// @param req The InitializeRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_initialize_request(const InitializeRequest &req,
                             utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ServerCapabilities object into a JsonWriter.
/// @param caps The ServerCapabilities object to serialize.
/// @param jw Destination JsonWriter.
void emit_server_capabilities(const ServerCapabilities &caps,
                              utils::JsonWriter &jw) noexcept;

/// @brief Serialize an InitializeResult object into a JsonWriter.
/// @param res The InitializeResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_initialize_result(const InitializeResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize an InitializedNotification object into a JsonWriter.
/// @param notif The InitializedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_initialized_notification(const InitializedNotification &notif,
                                   utils::JsonWriter &jw) noexcept;

/// @brief Serialize a JSONRPCError object into a JsonWriter.
/// @param err The JSONRPCError object to serialize.
/// @param jw Destination JsonWriter.
void emit_jsonrpc_error(const JSONRPCError &err,
                        utils::JsonWriter &jw) noexcept;

/// @brief Serialize a JSONRPCNotification object into a JsonWriter.
/// @param notif The JSONRPCNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_jsonrpc_notification(const JSONRPCNotification &notif,
                               utils::JsonWriter &jw) noexcept;

/// @brief Serialize a JSONRPCRequest object into a JsonWriter.
/// @param req The JSONRPCRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_jsonrpc_request(const JSONRPCRequest &req,
                          utils::JsonWriter &jw) noexcept;

/// @brief Serialize a JSONRPCResponse object into a JsonWriter.
/// @param res The JSONRPCResponse object to serialize.
/// @param jw Destination JsonWriter.
void emit_jsonrpc_response(const JSONRPCResponse &res,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListPromptsRequest object into a JsonWriter.
/// @param req The ListPromptsRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_prompts_request(const ListPromptsRequest &req,
                               utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PromptArgument object into a JsonWriter.
/// @param arg The PromptArgument object to serialize.
/// @param jw Destination JsonWriter.
void emit_prompt_argument(const PromptArgument &arg,
                          utils::JsonWriter &jw) noexcept;

/// @brief Serialize a Prompt definition into a JsonWriter.
/// @param p The Prompt object to serialize.
/// @param jw Destination JsonWriter.
void emit_prompt(const Prompt &p, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListPromptsResult object into a JsonWriter.
/// @param res The ListPromptsResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_prompts_result(const ListPromptsResult &res,
                              utils::JsonWriter &jw) noexcept;

/// @brief Serialize a GetPromptRequest object into a JsonWriter.
/// @param req The GetPromptRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_get_prompt_request(const GetPromptRequest &req,
                             utils::JsonWriter &jw) noexcept;

/// @brief Serialize a GetPromptResult object into a JsonWriter.
/// @param res The GetPromptResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_get_prompt_result(const GetPromptResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PromptListChangedNotification object into a JsonWriter.
/// @param notif The PromptListChangedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_prompt_list_changed_notification(
    const PromptListChangedNotification &notif, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PromptReference object into a JsonWriter.
/// @param ref The PromptReference object to serialize.
/// @param jw Destination JsonWriter.
void emit_prompt_reference(const PromptReference &ref,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListResourcesRequest object into a JsonWriter.
/// @param req The ListResourcesRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_resources_request(const ListResourcesRequest &req,
                                 utils::JsonWriter &jw) noexcept;

/// @brief Serialize a Resource object into a JsonWriter.
/// @param res The Resource object to serialize.
/// @param jw Destination JsonWriter.
void emit_resource(const Resource &res, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListResourcesResult object into a JsonWriter.
/// @param res The ListResourcesResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_resources_result(const ListResourcesResult &res,
                                utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListResourceTemplatesRequest object into a JsonWriter.
/// @param req The ListResourceTemplatesRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_resource_templates_request(
    const ListResourceTemplatesRequest &req, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ResourceTemplate object into a JsonWriter.
/// @param rt The ResourceTemplate object to serialize.
/// @param jw Destination JsonWriter.
void emit_resource_template(const ResourceTemplate &rt,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListResourceTemplatesResult object into a JsonWriter.
/// @param res The ListResourceTemplatesResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_resource_templates_result(const ListResourceTemplatesResult &res,
                                         utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ReadResourceRequest object into a JsonWriter.
/// @param req The ReadResourceRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_read_resource_request(const ReadResourceRequest &req,
                                utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ReadResourceResult object into a JsonWriter.
/// @param res The ReadResourceResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_read_resource_result(const ReadResourceResult &res,
                               utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ResourceUpdatedNotification object into a JsonWriter.
/// @param notif The ResourceUpdatedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_resource_updated_notification(
    const ResourceUpdatedNotification &notif, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ResourceListChangedNotification object into a JsonWriter.
/// @param notif The ResourceListChangedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_resource_list_changed_notification(
    const ResourceListChangedNotification &notif,
    utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ResourceReference object into a JsonWriter.
/// @param ref The ResourceReference object to serialize.
/// @param jw Destination JsonWriter.
void emit_resource_reference(const ResourceReference &ref,
                             utils::JsonWriter &jw) noexcept;

/// @brief Serialize a generic MCP Result object into a JsonWriter.
/// @param res The Result object to serialize.
/// @param jw Destination JsonWriter.
void emit_result(const Result &res, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a Root object into a JsonWriter.
/// @param root The Root object to serialize.
/// @param jw Destination JsonWriter.
void emit_root(const Root &root, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a RootsListChangedNotification object into a JsonWriter.
/// @param notif The RootsListChangedNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_roots_list_changed_notification(
    const RootsListChangedNotification &notif, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListRootsRequest object into a JsonWriter.
/// @param req The ListRootsRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_roots_request(const ListRootsRequest &req,
                             utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListRootsResult object into a JsonWriter.
/// @param res The ListRootsResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_roots_result(const ListRootsResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize an EmptyResult object into a JsonWriter.
/// @param res The EmptyResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_empty_result(const EmptyResult &res, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a SamplingMessage object into a JsonWriter.
/// @param msg The SamplingMessage object to serialize.
/// @param jw Destination JsonWriter.
void emit_sampling_message(const SamplingMessage &msg,
                           utils::JsonWriter &jw) noexcept;

/// @brief Serialize a PingRequest object into a JsonWriter.
/// @param req The PingRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_ping_request(const PingRequest &req, utils::JsonWriter &jw) noexcept;

/// @brief Serialize a SubscribeRequest object into a JsonWriter.
/// @param req The SubscribeRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_subscribe_request(const SubscribeRequest &req,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize an UnsubscribeRequest object into a JsonWriter.
/// @param req The UnsubscribeRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_unsubscribe_request(const UnsubscribeRequest &req,
                              utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListToolsRequest object into a JsonWriter.
/// @param req The ListToolsRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_tools_request(const ListToolsRequest &req,
                             utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ListToolsResult object into a JsonWriter.
/// @param res The ListToolsResult object to serialize.
/// @param jw Destination JsonWriter.
void emit_list_tools_result(const ListToolsResult &res,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a SetLevelRequest object into a JsonWriter.
/// @param req The SetLevelRequest object to serialize.
/// @param jw Destination JsonWriter.
void emit_set_level_request(const SetLevelRequest &req,
                            utils::JsonWriter &jw) noexcept;

/// @brief Serialize a LoggingMessageNotification object into a JsonWriter.
/// @param notif The LoggingMessageNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_logging_message_notification(const LoggingMessageNotification &notif,
                                       utils::JsonWriter &jw) noexcept;

/// @brief Serialize a ProgressNotification object into a JsonWriter.
/// @param notif The ProgressNotification object to serialize.
/// @param jw Destination JsonWriter.
void emit_progress_notification(const ProgressNotification &notif,
                                utils::JsonWriter &jw) noexcept;

} // namespace cdd_cpp::mcp
