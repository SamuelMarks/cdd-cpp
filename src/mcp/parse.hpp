#pragma once
#include "models.hpp"
#include <expected>
#include <simdjson.h>
#include <string>

namespace cdd_cpp::mcp {

/// @brief Auto-generated doc.
std::expected<Annotated, std::string>
parse_annotated(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<BlobResourceContents, std::string>
parse_blob_resource_contents(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<CallToolRequest, std::string>
parse_call_tool_request(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<TextContent, std::string>
parse_text_content(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ImageContent, std::string>
parse_image_content(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<TextResourceContents, std::string>
parse_text_resource_contents(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<EmbeddedResource, std::string>
parse_embedded_resource(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<CallToolResult, std::string>
parse_call_tool_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<CancelledNotification, std::string>
parse_cancelled_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ClientCapabilities, std::string>
parse_client_capabilities(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<CompleteRequest, std::string>
parse_complete_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<CompleteResult, std::string>
parse_complete_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ModelHint, std::string>
parse_model_hint(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ModelPreferences, std::string>
parse_model_preferences(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<CreateMessageRequest, std::string>
parse_create_message_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<CreateMessageResult, std::string>
parse_create_message_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<Tool, std::string>
parse_tool(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ToolListChangedNotification, std::string>
parse_tool_list_changed_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<PaginatedRequest, std::string>
parse_paginated_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<PaginatedResult, std::string>
parse_paginated_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<Implementation, std::string>
parse_implementation(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<InitializeRequest, std::string>
parse_initialize_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ServerCapabilities, std::string>
parse_server_capabilities(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<InitializeResult, std::string>
parse_initialize_result(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<InitializedNotification, std::string>
parse_initialized_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<JSONRPCError, std::string>
parse_jsonrpc_error(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<JSONRPCNotification, std::string>
parse_jsonrpc_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<JSONRPCRequest, std::string>
parse_jsonrpc_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<JSONRPCResponse, std::string>
parse_jsonrpc_response(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ListPromptsRequest, std::string>
parse_list_prompts_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<PromptArgument, std::string>
parse_prompt_argument(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<Prompt, std::string>
parse_prompt(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListPromptsResult, std::string>
parse_list_prompts_result(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<GetPromptRequest, std::string>
parse_get_prompt_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<GetPromptResult, std::string>
parse_get_prompt_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<PromptListChangedNotification, std::string>
parse_prompt_list_changed_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<PromptReference, std::string>
parse_prompt_reference(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ListResourcesRequest, std::string>
parse_list_resources_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<Resource, std::string>
parse_resource(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListResourcesResult, std::string>
parse_list_resources_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ListResourceTemplatesRequest, std::string>
parse_list_resource_templates_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ResourceTemplate, std::string>
parse_resource_template(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListResourceTemplatesResult, std::string>
parse_list_resource_templates_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ReadResourceRequest, std::string>
parse_read_resource_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ReadResourceResult, std::string>
parse_read_resource_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<ResourceUpdatedNotification, std::string>
parse_resource_updated_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ResourceListChangedNotification, std::string>
parse_resource_list_changed_notification(
    simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ResourceReference, std::string>
parse_resource_reference(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<Result, std::string>
parse_result(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<Root, std::string>
parse_root(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<RootsListChangedNotification, std::string>
parse_roots_list_changed_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListRootsRequest, std::string>
parse_list_roots_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListRootsResult, std::string>
parse_list_roots_result(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<SetLevelRequest, std::string>
parse_set_level_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<LoggingMessageNotification, std::string>
parse_logging_message_notification(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ProgressNotification, std::string>
parse_progress_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<EmptyResult, std::string>
parse_empty_result(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<SamplingMessage, std::string>
parse_sampling_message(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<PingRequest, std::string>
parse_ping_request(simdjson::ondemand::value &val) noexcept;

/// @brief Auto-generated doc.
std::expected<SubscribeRequest, std::string>
parse_subscribe_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<UnsubscribeRequest, std::string>
parse_unsubscribe_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListToolsRequest, std::string>
parse_list_tools_request(simdjson::ondemand::value &val) noexcept;
/// @brief Auto-generated doc.
std::expected<ListToolsResult, std::string>
parse_list_tools_result(simdjson::ondemand::value &val) noexcept;

} // namespace cdd_cpp::mcp
