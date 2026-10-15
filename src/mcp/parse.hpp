/// @file parse.hpp
/// @brief Model Context Protocol (MCP) JSON-RPC message parser declarations.

#pragma once
#include "../utils/error.hpp"
#include "models.hpp"
#include <simdjson.h>
#include <string>

namespace cdd_cpp::mcp {

/// @brief Standard Result alias for MCP parsing operations.
template <typename T> using McpResult = utils::Result<T>;

/// @brief Parse an Annotated object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Annotated or a typed Error.
McpResult<Annotated> parse_annotated(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a BlobResourceContents object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed BlobResourceContents or a typed
/// Error.
McpResult<BlobResourceContents>
parse_blob_resource_contents(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CallToolRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CallToolRequest or a typed Error.
McpResult<CallToolRequest>
parse_call_tool_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a TextContent object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed TextContent or a typed Error.
McpResult<TextContent>
parse_text_content(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an ImageContent object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ImageContent or a typed Error.
McpResult<ImageContent>
parse_image_content(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a TextResourceContents object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed TextResourceContents or a typed
/// Error.
McpResult<TextResourceContents>
parse_text_resource_contents(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an EmbeddedResource object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed EmbeddedResource or a typed Error.
McpResult<EmbeddedResource>
parse_embedded_resource(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CallToolResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CallToolResult or a typed Error.
McpResult<CallToolResult>
parse_call_tool_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CancelledNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CancelledNotification or a typed
/// Error.
McpResult<CancelledNotification>
parse_cancelled_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ClientCapabilities object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ClientCapabilities or a typed Error.
McpResult<ClientCapabilities>
parse_client_capabilities(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CompleteRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CompleteRequest or a typed Error.
McpResult<CompleteRequest>
parse_complete_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CompleteResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CompleteResult or a typed Error.
McpResult<CompleteResult>
parse_complete_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ModelHint object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ModelHint or a typed Error.
McpResult<ModelHint> parse_model_hint(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ModelPreferences object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ModelPreferences or a typed Error.
McpResult<ModelPreferences>
parse_model_preferences(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CreateMessageRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CreateMessageRequest or a typed
/// Error.
McpResult<CreateMessageRequest>
parse_create_message_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a CreateMessageResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed CreateMessageResult or a typed
/// Error.
McpResult<CreateMessageResult>
parse_create_message_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a Tool definition from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Tool or a typed Error.
McpResult<Tool> parse_tool(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ToolListChangedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ToolListChangedNotification or a
/// typed Error.
McpResult<ToolListChangedNotification>
parse_tool_list_changed_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PaginatedRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PaginatedRequest or a typed Error.
McpResult<PaginatedRequest>
parse_paginated_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PaginatedResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PaginatedResult or a typed Error.
McpResult<PaginatedResult>
parse_paginated_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an Implementation object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Implementation or a typed Error.
McpResult<Implementation>
parse_implementation(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an InitializeRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed InitializeRequest or a typed Error.
McpResult<InitializeRequest>
parse_initialize_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ServerCapabilities object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ServerCapabilities or a typed Error.
McpResult<ServerCapabilities>
parse_server_capabilities(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an InitializeResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed InitializeResult or a typed Error.
McpResult<InitializeResult>
parse_initialize_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an InitializedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed InitializedNotification or a typed
/// Error.
McpResult<InitializedNotification>
parse_initialized_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a JSONRPCError object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed JSONRPCError or a typed Error.
McpResult<JSONRPCError>
parse_jsonrpc_error(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a JSONRPCNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed JSONRPCNotification or a typed
/// Error.
McpResult<JSONRPCNotification>
parse_jsonrpc_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a JSONRPCRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed JSONRPCRequest or a typed Error.
McpResult<JSONRPCRequest>
parse_jsonrpc_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a JSONRPCResponse object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed JSONRPCResponse or a typed Error.
McpResult<JSONRPCResponse>
parse_jsonrpc_response(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListPromptsRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListPromptsRequest or a typed Error.
McpResult<ListPromptsRequest>
parse_list_prompts_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PromptArgument object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PromptArgument or a typed Error.
McpResult<PromptArgument>
parse_prompt_argument(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a Prompt definition from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Prompt or a typed Error.
McpResult<Prompt> parse_prompt(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListPromptsResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListPromptsResult or a typed Error.
McpResult<ListPromptsResult>
parse_list_prompts_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a GetPromptRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed GetPromptRequest or a typed Error.
McpResult<GetPromptRequest>
parse_get_prompt_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a GetPromptResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed GetPromptResult or a typed Error.
McpResult<GetPromptResult>
parse_get_prompt_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PromptListChangedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PromptListChangedNotification or a
/// typed Error.
McpResult<PromptListChangedNotification>
parse_prompt_list_changed_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PromptReference object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PromptReference or a typed Error.
McpResult<PromptReference>
parse_prompt_reference(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListResourcesRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListResourcesRequest or a typed
/// Error.
McpResult<ListResourcesRequest>
parse_list_resources_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a Resource object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Resource or a typed Error.
McpResult<Resource> parse_resource(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListResourcesResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListResourcesResult or a typed
/// Error.
McpResult<ListResourcesResult>
parse_list_resources_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListResourceTemplatesRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListResourceTemplatesRequest or a
/// typed Error.
McpResult<ListResourceTemplatesRequest>
parse_list_resource_templates_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ResourceTemplate object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ResourceTemplate or a typed Error.
McpResult<ResourceTemplate>
parse_resource_template(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListResourceTemplatesResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListResourceTemplatesResult or a
/// typed Error.
McpResult<ListResourceTemplatesResult>
parse_list_resource_templates_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ReadResourceRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ReadResourceRequest or a typed
/// Error.
McpResult<ReadResourceRequest>
parse_read_resource_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ReadResourceResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ReadResourceResult or a typed Error.
McpResult<ReadResourceResult>
parse_read_resource_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ResourceUpdatedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ResourceUpdatedNotification or a
/// typed Error.
McpResult<ResourceUpdatedNotification>
parse_resource_updated_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ResourceListChangedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ResourceListChangedNotification or a
/// typed Error.
McpResult<ResourceListChangedNotification>
parse_resource_list_changed_notification(
    simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ResourceReference object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ResourceReference or a typed Error.
McpResult<ResourceReference>
parse_resource_reference(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a generic MCP Result object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Result or a typed Error.
McpResult<Result> parse_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a Root object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed Root or a typed Error.
McpResult<Root> parse_root(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a RootsListChangedNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed RootsListChangedNotification or a
/// typed Error.
McpResult<RootsListChangedNotification>
parse_roots_list_changed_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListRootsRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListRootsRequest or a typed Error.
McpResult<ListRootsRequest>
parse_list_roots_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListRootsResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListRootsResult or a typed Error.
McpResult<ListRootsResult>
parse_list_roots_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a SetLevelRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed SetLevelRequest or a typed Error.
McpResult<SetLevelRequest>
parse_set_level_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a LoggingMessageNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed LoggingMessageNotification or a
/// typed Error.
McpResult<LoggingMessageNotification>
parse_logging_message_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ProgressNotification object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ProgressNotification or a typed
/// Error.
McpResult<ProgressNotification>
parse_progress_notification(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an EmptyResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed EmptyResult or a typed Error.
McpResult<EmptyResult>
parse_empty_result(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a SamplingMessage object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed SamplingMessage or a typed Error.
McpResult<SamplingMessage>
parse_sampling_message(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a PingRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed PingRequest or a typed Error.
McpResult<PingRequest>
parse_ping_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a SubscribeRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed SubscribeRequest or a typed Error.
McpResult<SubscribeRequest>
parse_subscribe_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse an UnsubscribeRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed UnsubscribeRequest or a typed Error.
McpResult<UnsubscribeRequest>
parse_unsubscribe_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListToolsRequest object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListToolsRequest or a typed Error.
McpResult<ListToolsRequest>
parse_list_tools_request(simdjson::ondemand::value &val) noexcept;

/// @brief Parse a ListToolsResult object from a simdjson value.
/// @param val The simdjson value to parse.
/// @return McpResult containing the parsed ListToolsResult or a typed Error.
McpResult<ListToolsResult>
parse_list_tools_result(simdjson::ondemand::value &val) noexcept;

} // namespace cdd_cpp::mcp
