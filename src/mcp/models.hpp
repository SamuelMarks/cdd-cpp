// GCOV_EXCL_BR_START
#pragma once
#include <optional>
#include <string>
#include <vector>

namespace cdd_cpp::mcp {

/// @brief Auto-generated doc.
struct Annotations {
  std::optional<std::string> audience;
  std::optional<double> priority;
};

/// @brief Auto-generated doc.
struct Annotated {
  std::optional<Annotations> annotations;
};

/// @brief Auto-generated doc.
struct BlobResourceContents {
  std::string blob;
  std::optional<std::string> mimeType;
  std::string uri;
};

/// @brief Auto-generated doc.
struct CallToolRequestParams {
  std::string name;
  std::optional<std::string> arguments;
};

/// @brief Auto-generated doc.
struct CallToolRequest {
  std::string method;
  CallToolRequestParams params;
};

/// @brief Auto-generated doc.
struct TextContent {
  std::string type; // "text"
  std::string text;
  std::optional<Annotations> annotations;
};

/// @brief Auto-generated doc.
struct ImageContent {
  std::string type; // "image"
  std::string data;
  std::string mimeType;
  std::optional<Annotations> annotations;
};

/// @brief Auto-generated doc.
struct ResourceContents {
  std::string uri;
  std::optional<std::string> mimeType;
};

/// @brief Auto-generated doc.
struct TextResourceContents : ResourceContents {
  std::string text;
};

// BlobResourceContents is already defined, let's make it inherit
// ResourceContents wait, we can just define it independently or inherit. Let's
// keep BlobResourceContents as is.

// Actually EmbeddedResource holds ResourceContents
/// @brief Auto-generated doc.
struct EmbeddedResource {
  std::string type; // "resource"
  std::string
      resource_json; // raw json of TextResourceContents or BlobResourceContents
  std::optional<Annotations> annotations;
};

/// @brief Auto-generated doc.
struct CallToolResult {
  std::optional<std::string> _meta; // raw json
  std::string content_json;         // raw json array of content
  std::optional<bool> isError;
};

/// @brief Auto-generated doc.
struct CancelledNotificationParams {
  std::string requestId;
  std::optional<std::string> reason;
};

/// @brief Auto-generated doc.
struct CancelledNotification {
  std::string method;
  CancelledNotificationParams params;
};

/// @brief Auto-generated doc.
struct ClientCapabilitiesRoots {
  std::optional<bool> listChanged;
};

/// @brief Auto-generated doc.
struct ClientCapabilities {
  std::optional<std::string> experimental; // raw json
  std::optional<ClientCapabilitiesRoots> roots;
  std::optional<std::string> sampling; // raw json
};

/// @brief Auto-generated doc.
struct CompleteRequestArgument {
  std::string name;
  std::string value;
};

/// @brief Auto-generated doc.
struct CompleteRequestParams {
  CompleteRequestArgument argument;
  std::string
      ref_json; // raw json for ref (PromptReference | ResourceReference)
};

/// @brief Auto-generated doc.
struct CompleteRequest {
  std::string method; // "completion/complete"
  CompleteRequestParams params;
};

/// @brief Auto-generated doc.
struct CompleteResultCompletion {
  std::vector<std::string> values;
  std::optional<int> total;
  std::optional<bool> hasMore;
};

/// @brief Auto-generated doc.
struct CompleteResult {
  std::optional<std::string> _meta; // raw json
  CompleteResultCompletion completion;
};

/// @brief Auto-generated doc.
struct ModelHint {
  std::optional<std::string> name;
};

/// @brief Auto-generated doc.
struct ModelPreferences {
  std::optional<double> costPriority;
  std::optional<std::vector<ModelHint>> hints;
  std::optional<double> intelligencePriority;
  std::optional<double> speedPriority;
};

/// @brief Auto-generated doc.
struct CreateMessageRequestParams {
  int maxTokens;
  std::string messages_json;                 // raw json
  std::optional<std::string> includeContext; // raw json
  std::optional<std::string> metadata;       // raw json
  std::optional<ModelPreferences> modelPreferences;
  std::optional<std::vector<std::string>> stopSequences;
  std::optional<std::string> systemPrompt;
  std::optional<double> temperature;
};

/// @brief Auto-generated doc.
struct CreateMessageRequest {
  std::string method; // "sampling/createMessage"
  CreateMessageRequestParams params;
};

/// @brief Auto-generated doc.
struct CreateMessageResult {
  std::optional<std::string> _meta; // raw json
  std::string content_json;         // raw json
  std::string model;
  std::string role;
  std::optional<std::string> stopReason;
};

/// @brief Auto-generated doc.
struct ToolInputSchema {
  std::string type;                           // "object"
  std::optional<std::string> properties_json; // raw json
  std::optional<std::vector<std::string>> required;
};

/// @brief Auto-generated doc.
struct Tool {
  std::string name;
  std::optional<std::string> description;
  ToolInputSchema inputSchema;
};

/// @brief Auto-generated doc.
struct ToolListChangedNotificationParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct ToolListChangedNotification {
  std::string method; // "notifications/tools/list_changed"
  std::optional<ToolListChangedNotificationParams> params;
};

typedef std::string Cursor;

/// @brief Auto-generated doc.
struct PaginatedRequestParams {
  std::optional<Cursor> cursor;
};

/// @brief Auto-generated doc.
struct PaginatedRequest {
  std::string method;
  PaginatedRequestParams params;
};

/// @brief Auto-generated doc.
struct PaginatedResult {
  std::optional<std::string> _meta; // raw json
  std::optional<Cursor> nextCursor;
};

/// @brief Auto-generated doc.
struct Implementation {
  std::string name;
  std::string version;
};

/// @brief Auto-generated doc.
struct InitializeRequestParams {
  std::string protocolVersion;
  std::optional<ClientCapabilities> capabilities;
  Implementation clientInfo;
};

/// @brief Auto-generated doc.
struct InitializeRequest {
  std::string method; // "initialize"
  InitializeRequestParams params;
};

/// @brief Auto-generated doc.
struct ServerCapabilitiesLogging {};

/// @brief Auto-generated doc.
struct ServerCapabilitiesPrompts {
  std::optional<bool> listChanged;
};

/// @brief Auto-generated doc.
struct ServerCapabilitiesResources {
  std::optional<bool> subscribe;
  std::optional<bool> listChanged;
};

/// @brief Auto-generated doc.
struct ServerCapabilitiesTools {
  std::optional<bool> listChanged;
};

/// @brief Auto-generated doc.
struct ServerCapabilities {
  std::optional<std::string> experimental; // raw json
  std::optional<ServerCapabilitiesLogging> logging;
  std::optional<ServerCapabilitiesPrompts> prompts;
  std::optional<ServerCapabilitiesResources> resources;
  std::optional<ServerCapabilitiesTools> tools;
};

/// @brief Auto-generated doc.
struct InitializeResult {
  std::optional<std::string> _meta; // raw json
  std::string protocolVersion;
  ServerCapabilities capabilities;
  Implementation serverInfo;
  std::optional<std::string> instructions;
};

/// @brief Auto-generated doc.
struct InitializedNotificationParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct InitializedNotification {
  std::string method; // "notifications/initialized"
  std::optional<InitializedNotificationParams> params;
};

// RequestId can be string or number, we'll represent it as a raw json string to
// support both cleanly
typedef std::string RequestId;

/// @brief Auto-generated doc.
struct JSONRPCErrorError {
  int code;
  std::string message;
  std::optional<std::string> data; // raw json
};

/// @brief Auto-generated doc.
struct JSONRPCError {
  std::string jsonrpc; // "2.0"
  RequestId id;
  JSONRPCErrorError error;
};

/// @brief Auto-generated doc.
struct JSONRPCNotificationParams {
  std::optional<std::string> _meta; // raw json
  // allows other properties
  std::optional<std::string> _raw; // raw json
};

/// @brief Auto-generated doc.
struct JSONRPCNotification {
  std::string jsonrpc; // "2.0"
  std::string method;
  std::optional<JSONRPCNotificationParams> params;
};

/// @brief Auto-generated doc.
struct JSONRPCRequestParams {
  std::optional<std::string> _meta; // raw json
  // allows other properties
  std::optional<std::string> _raw; // raw json
};

/// @brief Auto-generated doc.
struct JSONRPCRequest {
  std::string jsonrpc; // "2.0"
  RequestId id;
  std::string method;
  std::optional<JSONRPCRequestParams> params;
};

/// @brief Auto-generated doc.
struct JSONRPCResponse {
  std::string jsonrpc; // "2.0"
  RequestId id;
  std::string result_json; // raw json
};

/// @brief Auto-generated doc.
struct ListPromptsRequestParams {
  std::optional<Cursor> cursor;
};

/// @brief Auto-generated doc.
struct ListPromptsRequest {
  std::string method; // "prompts/list"
  std::optional<ListPromptsRequestParams> params;
};

/// @brief Auto-generated doc.
struct PromptArgument {
  std::string name;
  std::optional<std::string> description;
  std::optional<bool> required;
};

/// @brief Auto-generated doc.
struct Prompt {
  std::string name;
  std::optional<std::string> description;
  std::optional<std::vector<PromptArgument>> arguments;
};

/// @brief Auto-generated doc.
struct ListPromptsResult {
  std::optional<std::string> _meta; // raw json
  std::optional<Cursor> nextCursor;
  std::vector<Prompt> prompts;
};

/// @brief Auto-generated doc.
struct GetPromptRequestParams {
  std::string name;
  std::optional<std::string> arguments; // raw json map
};

/// @brief Auto-generated doc.
struct GetPromptRequest {
  std::string method; // "prompts/get"
  GetPromptRequestParams params;
};

/// @brief Auto-generated doc.
struct PromptMessage {
  std::string role;
  std::string content_json; // raw json array or object
};

/// @brief Auto-generated doc.
struct GetPromptResult {
  std::optional<std::string> _meta; // raw json
  std::optional<std::string> description;
  std::vector<PromptMessage> messages;
};

/// @brief Auto-generated doc.
struct PromptListChangedNotificationParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct PromptListChangedNotification {
  std::string method; // "notifications/prompts/list_changed"
  std::optional<PromptListChangedNotificationParams> params;
};

/// @brief Auto-generated doc.
struct PromptReference {
  std::string type; // "ref/prompt"
  std::string name;
};

/// @brief Auto-generated doc.
struct ListResourcesRequestParams {
  std::optional<Cursor> cursor;
};

/// @brief Auto-generated doc.
struct ListResourcesRequest {
  std::string method; // "resources/list"
  std::optional<ListResourcesRequestParams> params;
};

/// @brief Auto-generated doc.
struct Resource {
  std::string uri;
  std::string name;
  std::optional<std::string> description;
  std::optional<std::string> mimeType;
  std::optional<Annotations> annotations; // inherited through extensions? Or
                                          // just plain field in 2024-11-05
};

/// @brief Auto-generated doc.
struct ListResourcesResult {
  std::optional<std::string> _meta; // raw json
  std::optional<Cursor> nextCursor;
  std::vector<Resource> resources;
};

/// @brief Auto-generated doc.
struct ListResourceTemplatesRequestParams {
  std::optional<Cursor> cursor;
};

/// @brief Auto-generated doc.
struct ListResourceTemplatesRequest {
  std::string method; // "resources/templates/list"
  std::optional<ListResourceTemplatesRequestParams> params;
};

/// @brief Auto-generated doc.
struct ResourceTemplate {
  std::string uriTemplate;
  std::string name;
  std::optional<std::string> description;
  std::optional<std::string> mimeType;
  std::optional<Annotations> annotations;
};

/// @brief Auto-generated doc.
struct ListResourceTemplatesResult {
  std::optional<std::string> _meta; // raw json
  std::optional<Cursor> nextCursor;
  std::vector<ResourceTemplate> resourceTemplates;
};

/// @brief Auto-generated doc.
struct ReadResourceRequestParams {
  std::string uri;
};

/// @brief Auto-generated doc.
struct ReadResourceRequest {
  std::string method; // "resources/read"
  ReadResourceRequestParams params;
};

/// @brief Auto-generated doc.
struct ReadResourceResult {
  std::optional<std::string> _meta; // raw json
  std::vector<std::string>
      contents_json; // raw json objects (TextResourceContents or
                     // BlobResourceContents)
};

/// @brief Auto-generated doc.
struct ResourceUpdatedNotificationParams {
  std::string uri;
};

/// @brief Auto-generated doc.
struct ResourceUpdatedNotification {
  std::string method; // "notifications/resources/updated"
  ResourceUpdatedNotificationParams params;
};

/// @brief Auto-generated doc.
struct ResourceListChangedNotificationParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct ResourceListChangedNotification {
  std::string method; // "notifications/resources/list_changed"
  std::optional<ResourceListChangedNotificationParams> params;
};

/// @brief Auto-generated doc.
struct ResourceReference {
  std::string type; // "ref/resource"
  std::string uri;
};

/// @brief Auto-generated doc.
struct Result {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct Root {
  std::string uri;
  std::optional<std::string> name;
};

/// @brief Auto-generated doc.
struct RootsListChangedNotificationParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct RootsListChangedNotification {
  std::string method; // "notifications/roots/list_changed"
  std::optional<RootsListChangedNotificationParams> params;
};

/// @brief Auto-generated doc.
struct ListRootsRequestParams {
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
struct ListRootsRequest {
  std::string method; // "roots/list"
  std::optional<ListRootsRequestParams> params;
};

/// @brief Auto-generated doc.
struct ListRootsResult {
  std::optional<std::string> _meta; // raw json
  std::vector<Root> roots;
};

typedef std::string LoggingLevel; // Enum string in practice

/// @brief Auto-generated doc.
struct SetLevelRequestParams {
  LoggingLevel level;
};

/// @brief Auto-generated doc.
struct SetLevelRequest {
  std::string method; // "logging/setLevel"
  SetLevelRequestParams params;
};

/// @brief Auto-generated doc.
struct LoggingMessageNotificationParams {
  LoggingLevel level;
  std::optional<std::string> logger;
  std::string
      data; // can be any data type, we'll expose as raw json or string, schema
            // says "data": {} so it's arbitrary. We will map to raw_json
};

/// @brief Auto-generated doc.
struct LoggingMessageNotification {
  std::string method; // "notifications/message"
  LoggingMessageNotificationParams params;
};

typedef std::string
    ProgressToken; // string or integer, we'll keep as raw json string

/// @brief Auto-generated doc.
struct ProgressNotificationParams {
  ProgressToken progressToken;
  double progress;
  std::optional<double> total;
};

/// @brief Auto-generated doc.
struct ProgressNotification {
  std::string method; // "notifications/progress"
  ProgressNotificationParams params;
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
// GCOV_EXCL_START
// GCOV_EXCL_STOP
struct EmptyResult {
  // GCOV_EXCL_STOP
  std::optional<std::string> _meta; // raw json
};

typedef std::string Role; // "user" | "assistant"
// GCOV_EXCL_START

// GCOV_EXCL_STOP
/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct SamplingMessage {
  // GCOV_EXCL_STOP
  Role role;
  // GCOV_EXCL_START
  std::string content_json; // raw json
  // GCOV_EXCL_STOP
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
// GCOV_EXCL_START
struct PingRequestParams {
  // GCOV_EXCL_STOP
  // GCOV_EXCL_STOP
  std::optional<std::string> _meta; // raw json
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
// GCOV_EXCL_START
// GCOV_EXCL_STOP
struct PingRequest {
  // GCOV_EXCL_STOP
  std::string method; // "ping"
  std::optional<PingRequestParams> params;
  // GCOV_EXCL_START
};
// GCOV_EXCL_STOP

/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct SubscribeRequestParams {
  // GCOV_EXCL_STOP
  // GCOV_EXCL_START
  std::string uri;
  // GCOV_EXCL_STOP
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
// GCOV_EXCL_START
struct SubscribeRequest {
  // GCOV_EXCL_STOP
  // GCOV_EXCL_STOP
  std::string method; // "resources/subscribe"
  SubscribeRequestParams params;
};

// GCOV_EXCL_START
/// @brief Auto-generated doc.
// GCOV_EXCL_STOP
// GCOV_EXCL_START
struct UnsubscribeRequestParams {
  // GCOV_EXCL_STOP
  std::string uri;
  // GCOV_EXCL_START
};
// GCOV_EXCL_STOP

/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct UnsubscribeRequest {
  // GCOV_EXCL_STOP
  // GCOV_EXCL_START
  std::string method; // "resources/unsubscribe"
                      // GCOV_EXCL_STOP
  UnsubscribeRequestParams params;
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct ListToolsRequestParams {
  // GCOV_EXCL_STOP
  std::optional<Cursor> cursor;
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct ListToolsRequest {
  // GCOV_EXCL_STOP
  std::string method; // "tools/list"
  std::optional<ListToolsRequestParams> params;
};

/// @brief Auto-generated doc.
// GCOV_EXCL_START
struct ListToolsResult {
  // GCOV_EXCL_STOP
  std::optional<std::string> _meta; // raw json
  std::optional<Cursor> nextCursor;
  std::vector<Tool> tools;
};

} // namespace cdd_cpp::mcp

// GCOV_EXCL_BR_STOP
