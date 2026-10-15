/// @file models.hpp
/// @brief Strongly typed Model Context Protocol (MCP) 2024-11-05 data models
/// and JSON-RPC 2.0 structures.

#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

/// @namespace cdd_cpp::mcp
/// @brief Model Context Protocol (MCP) models, parsers, and emitters.
namespace cdd_cpp::mcp {

/// @brief Role of a participant in an MCP prompt or sampling message.
enum class RoleEnum {
  User,     ///< The human user or caller ("user").
  Assistant ///< The AI assistant or model ("assistant").
};

/// @brief Convert a RoleEnum to its MCP specification string.
/// @param r The RoleEnum value.
/// @return String view ("user" or "assistant").
constexpr std::string_view to_string(RoleEnum r) noexcept {
  switch (r) {
  case RoleEnum::User:
    return "user";
  case RoleEnum::Assistant:
    return "assistant";
  }
  return "user";
}

/// @brief Parse a string into a strongly typed RoleEnum.
/// @param s The string to parse.
/// @return Optional RoleEnum if recognized.
constexpr std::optional<RoleEnum>
role_from_string(std::string_view s) noexcept {
  if (s == "user")
    return RoleEnum::User;
  if (s == "assistant")
    return RoleEnum::Assistant;
  return std::nullopt;
}

/// @brief Syslog-aligned logging severity levels in MCP.
enum class LoggingLevelEnum {
  Debug,    ///< Detailed debugging information ("debug").
  Info,     ///< Informational messages ("info").
  Notice,   ///< Normal but significant events ("notice").
  Warning,  ///< Warning conditions ("warning").
  Error,    ///< Error conditions ("error").
  Critical, ///< Critical conditions ("critical").
  Alert,    ///< Action must be taken immediately ("alert").
  Emergency ///< System is unusable ("emergency").
};

/// @brief Convert a LoggingLevelEnum to its MCP specification string.
/// @param lvl The LoggingLevelEnum value.
/// @return String view representation of the logging level.
constexpr std::string_view to_string(LoggingLevelEnum lvl) noexcept {
  switch (lvl) {
  case LoggingLevelEnum::Debug:
    return "debug";
  case LoggingLevelEnum::Info:
    return "info";
  case LoggingLevelEnum::Notice:
    return "notice";
  case LoggingLevelEnum::Warning:
    return "warning";
  case LoggingLevelEnum::Error:
    return "error";
  case LoggingLevelEnum::Critical:
    return "critical";
  case LoggingLevelEnum::Alert:
    return "alert";
  case LoggingLevelEnum::Emergency:
    return "emergency";
  }
  return "info";
}

/// @brief Parse a string into a strongly typed LoggingLevelEnum.
/// @param s The string to parse.
/// @return Optional LoggingLevelEnum if recognized.
constexpr std::optional<LoggingLevelEnum>
logging_level_from_string(std::string_view s) noexcept {
  if (s == "debug")
    return LoggingLevelEnum::Debug;
  if (s == "info")
    return LoggingLevelEnum::Info;
  if (s == "notice")
    return LoggingLevelEnum::Notice;
  if (s == "warning")
    return LoggingLevelEnum::Warning;
  if (s == "error")
    return LoggingLevelEnum::Error;
  if (s == "critical")
    return LoggingLevelEnum::Critical;
  if (s == "alert")
    return LoggingLevelEnum::Alert;
  if (s == "emergency")
    return LoggingLevelEnum::Emergency;
  return std::nullopt;
}

/// @brief Scope of MCP server context to include in a sampling request.
enum class IncludeContextEnum {
  None,       ///< Do not include any context ("none").
  ThisServer, ///< Include context from the requesting server ("thisServer").
  AllServers ///< Include context from all connected MCP servers ("allServers").
};

/// @brief Convert an IncludeContextEnum to its MCP specification string.
/// @param ctx The IncludeContextEnum value.
/// @return String view ("none", "thisServer", or "allServers").
constexpr std::string_view to_string(IncludeContextEnum ctx) noexcept {
  switch (ctx) {
  case IncludeContextEnum::None:
    return "none";
  case IncludeContextEnum::ThisServer:
    return "thisServer";
  case IncludeContextEnum::AllServers:
    return "allServers";
  }
  return "none";
}

/// @brief Parse a string into a strongly typed IncludeContextEnum.
/// @param s The string to parse.
/// @return Optional IncludeContextEnum if recognized.
constexpr std::optional<IncludeContextEnum>
include_context_from_string(std::string_view s) noexcept {
  if (s == "none")
    return IncludeContextEnum::None;
  if (s == "thisServer")
    return IncludeContextEnum::ThisServer;
  if (s == "allServers")
    return IncludeContextEnum::AllServers;
  return std::nullopt;
}

/// @brief Optional metadata annotations attached to resources, tools, and
/// content blocks.
struct Annotations {
  std::optional<std::string> audience; ///< Intended audience role(s) ("user",
                                       ///< "assistant", or JSON array).
  std::optional<double>
      priority; ///< Importance priority between 0.0 (least) and 1.0 (most).

  /// @brief Equality comparison operator.
  bool operator==(const Annotations &) const noexcept = default;
};

/// @brief Base mixin for MCP objects carrying optional Annotations.
struct Annotated {
  std::optional<Annotations> annotations; ///< Optional annotations metadata.

  /// @brief Equality comparison operator.
  bool operator==(const Annotated &) const noexcept = default;
};

/// @brief Base structure representing the URI and MIME type of a resource's
/// contents.
struct ResourceContents {
  std::string uri;                     ///< The URI of the resource.
  std::optional<std::string> mimeType; ///< Optional MIME type of the resource.

  /// @brief Equality comparison operator.
  bool operator==(const ResourceContents &) const noexcept = default;
};

/// @brief Binary resource contents encoded in base64.
struct BlobResourceContents {
  std::string blob; ///< Base64-encoded binary data of the resource.
  std::optional<std::string>
      mimeType;    ///< Optional MIME type of the binary resource.
  std::string uri; ///< The URI of the resource.

  /// @brief Equality comparison operator.
  bool operator==(const BlobResourceContents &) const noexcept = default;
};

/// @brief Textual resource contents.
struct TextResourceContents : ResourceContents {
  std::string text; ///< The text content of the resource.

  /// @brief Equality comparison operator.
  bool operator==(const TextResourceContents &) const noexcept = default;
};

/// @brief Variant representing either text or binary resource contents.
using ResourceContentsVariant =
    std::variant<TextResourceContents, BlobResourceContents>;

/// @brief Parameters for invoking an MCP tool (`tools/call`).
struct CallToolRequestParams {
  std::string name; ///< Name of the tool to invoke.
  std::optional<std::string>
      arguments; ///< Optional JSON object string of tool arguments.

  /// @brief Equality comparison operator.
  bool operator==(const CallToolRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to invoke an MCP tool (`tools/call`).
struct CallToolRequest {
  std::string method;           ///< Method name ("tools/call").
  CallToolRequestParams params; ///< Tool invocation parameters.

  /// @brief Equality comparison operator.
  bool operator==(const CallToolRequest &) const noexcept = default;
};

/// @brief Text content block returned by a tool or prompt message.
struct TextContent {
  std::string type;                       ///< Content block type ("text").
  std::string text;                       ///< The text content.
  std::optional<Annotations> annotations; ///< Optional annotations.

  /// @brief Equality comparison operator.
  bool operator==(const TextContent &) const noexcept = default;
};

/// @brief Base64-encoded image content block.
struct ImageContent {
  std::string type;     ///< Content block type ("image").
  std::string data;     ///< Base64-encoded image data.
  std::string mimeType; ///< Image MIME type (e.g., "image/png").
  std::optional<Annotations> annotations; ///< Optional annotations.

  /// @brief Equality comparison operator.
  bool operator==(const ImageContent &) const noexcept = default;
};

/// @brief Resource embedded directly inside a tool result or message.
struct EmbeddedResource {
  std::string type;          ///< Content block type ("resource").
  std::string resource_json; ///< Serialized JSON of TextResourceContents or
                             ///< BlobResourceContents.
  std::optional<Annotations> annotations; ///< Optional annotations.

  /// @brief Equality comparison operator.
  bool operator==(const EmbeddedResource &) const noexcept = default;
};

/// @brief Variant representing any MCP content block (text, image, or embedded
/// resource).
using ContentBlockVariant =
    std::variant<TextContent, ImageContent, EmbeddedResource>;

/// @brief Result returned by the server from a `tools/call` invocation.
struct CallToolResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::string content_json;         ///< JSON array of content blocks.
  std::optional<bool>
      isError; ///< True if the tool execution resulted in an error.

  /// @brief Equality comparison operator.
  bool operator==(const CallToolResult &) const noexcept = default;
};

/// @brief Parameters for a `notifications/cancelled` notification.
struct CancelledNotificationParams {
  std::string requestId; ///< ID of the request being cancelled.
  std::optional<std::string>
      reason; ///< Optional human-readable reason for cancellation.

  /// @brief Equality comparison operator.
  bool operator==(const CancelledNotificationParams &) const noexcept = default;
};

/// @brief Notification sent by either party to cancel an in-progress request.
struct CancelledNotification {
  std::string method; ///< Method name ("notifications/cancelled").
  CancelledNotificationParams params; ///< Cancellation parameters.

  /// @brief Equality comparison operator.
  bool operator==(const CancelledNotification &) const noexcept = default;
};

/// @brief Client capability flags for filesystem roots.
struct ClientCapabilitiesRoots {
  std::optional<bool> listChanged; ///< Whether the client supports
                                   ///< notifications for root list changes.

  /// @brief Equality comparison operator.
  bool operator==(const ClientCapabilitiesRoots &) const noexcept = default;
};

/// @brief Capabilities advertised by an MCP client during initialization.
struct ClientCapabilities {
  std::optional<std::string>
      experimental; ///< Non-standard experimental capabilities JSON.
  std::optional<ClientCapabilitiesRoots>
      roots; ///< Filesystem roots capability configuration.
  std::optional<std::string>
      sampling; ///< LLM sampling capability configuration JSON.

  /// @brief Equality comparison operator.
  bool operator==(const ClientCapabilities &) const noexcept = default;
};

/// @brief Argument being completed in a `completion/complete` request.
struct CompleteRequestArgument {
  std::string name;  ///< The name of the argument.
  std::string value; ///< The current partial value of the argument.

  /// @brief Equality comparison operator.
  bool operator==(const CompleteRequestArgument &) const noexcept = default;
};

/// @brief Reference to a prompt by name (`ref/prompt`).
struct PromptReference {
  std::string type; ///< Reference type ("ref/prompt").
  std::string name; ///< Name of the referenced prompt.

  /// @brief Equality comparison operator.
  bool operator==(const PromptReference &) const noexcept = default;
};

/// @brief Reference to a resource or template by URI (`ref/resource`).
struct ResourceReference {
  std::string type; ///< Reference type ("ref/resource").
  std::string uri;  ///< URI or URI template of the referenced resource.

  /// @brief Equality comparison operator.
  bool operator==(const ResourceReference &) const noexcept = default;
};

/// @brief Variant representing either a PromptReference or a ResourceReference.
using CompletionReferenceVariant =
    std::variant<PromptReference, ResourceReference>;

/// @brief Parameters for a `completion/complete` request.
struct CompleteRequestParams {
  CompleteRequestArgument argument; ///< The argument being autocompleted.
  std::string
      ref_json; ///< Serialized JSON of PromptReference or ResourceReference.

  /// @brief Equality comparison operator.
  bool operator==(const CompleteRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to ask for autocompletion options.
struct CompleteRequest {
  std::string method;           ///< Method name ("completion/complete").
  CompleteRequestParams params; ///< Completion request parameters.

  /// @brief Equality comparison operator.
  bool operator==(const CompleteRequest &) const noexcept = default;
};

/// @brief Completion candidates payload inside `CompleteResult`.
struct CompleteResultCompletion {
  std::vector<std::string> values; ///< Autocompletion string values (max 100).
  std::optional<int> total; ///< Total number of available completion options.
  std::optional<bool>
      hasMore; ///< True if additional completion options exist beyond `values`.

  /// @brief Equality comparison operator.
  bool operator==(const CompleteResultCompletion &) const noexcept = default;
};

/// @brief Response returned by the server for a `completion/complete` request.
struct CompleteResult {
  std::optional<std::string> _meta;    ///< Optional protocol metadata JSON.
  CompleteResultCompletion completion; ///< Completion candidates result.

  /// @brief Equality comparison operator.
  bool operator==(const CompleteResult &) const noexcept = default;
};

/// @brief Hint for model selection during MCP sampling.
struct ModelHint {
  std::optional<std::string>
      name; ///< Substring hint for a model name (e.g., "claude-3-5-sonnet").

  /// @brief Equality comparison operator.
  bool operator==(const ModelHint &) const noexcept = default;
};

/// @brief Server's preferences for model selection requested from the client
/// during sampling.
struct ModelPreferences {
  std::optional<double>
      costPriority; ///< Priority for minimizing cost (0.0 to 1.0).
  std::optional<std::vector<ModelHint>>
      hints; ///< Ordered list of model name hints.
  std::optional<double>
      intelligencePriority; ///< Priority for model capability/intelligence (0.0
                            ///< to 1.0).
  std::optional<double>
      speedPriority; ///< Priority for low latency/speed (0.0 to 1.0).

  /// @brief Equality comparison operator.
  bool operator==(const ModelPreferences &) const noexcept = default;
};

/// @brief Parameters for a `sampling/createMessage` request.
struct CreateMessageRequestParams {
  int maxTokens = 0; ///< Maximum number of tokens to sample.
  std::string
      messages_json; ///< Serialized JSON array of SamplingMessage objects.
  std::optional<std::string>
      includeContext; ///< Context scope ("none", "thisServer", "allServers").
  std::optional<std::string>
      metadata; ///< Optional provider-specific metadata JSON.
  std::optional<ModelPreferences>
      modelPreferences; ///< Optional model selection preferences.
  std::optional<std::vector<std::string>>
      stopSequences;                       ///< Optional stop sequences.
  std::optional<std::string> systemPrompt; ///< Optional system prompt string.
  std::optional<double> temperature;       ///< Optional sampling temperature.

  /// @brief Get the strongly typed IncludeContextEnum if set and valid.
  /// @return Optional IncludeContextEnum value.
  std::optional<IncludeContextEnum> typed_include_context() const noexcept {
    if (!includeContext.has_value())
      return std::nullopt;
    std::string_view sv = *includeContext;
    if (sv.size() >= 2 && sv.front() == '"' && sv.back() == '"')
      sv = sv.substr(1, sv.size() - 2);
    return include_context_from_string(sv);
  }

  /// @brief Set the includeContext field from a strongly typed
  /// IncludeContextEnum.
  /// @param ctx The IncludeContextEnum value to set.
  void set_typed_include_context(IncludeContextEnum ctx) noexcept {
    includeContext = std::string(to_string(ctx));
  }

  /// @brief Equality comparison operator.
  bool operator==(const CreateMessageRequestParams &) const noexcept = default;
};

/// @brief Request sent from the server to the client to sample an LLM message.
struct CreateMessageRequest {
  std::string method; ///< Method name ("sampling/createMessage").
  CreateMessageRequestParams params; ///< Sampling parameters.

  /// @brief Equality comparison operator.
  bool operator==(const CreateMessageRequest &) const noexcept = default;
};

/// @brief Result returned by the client after sampling an LLM message.
struct CreateMessageResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::string content_json; ///< Serialized JSON of the sampled content block.
  std::string model; ///< Identifier of the model that generated the message.
  std::string role;  ///< Role of the generated message ("assistant" or "user").
  std::optional<std::string> stopReason; ///< Reason why sampling stopped (e.g.,
                                         ///< "endTurn", "maxTokens").

  /// @brief Get the strongly typed RoleEnum for this result.
  /// @return Optional RoleEnum value.
  std::optional<RoleEnum> typed_role() const noexcept {
    return role_from_string(role);
  }

  /// @brief Set the role from a strongly typed RoleEnum.
  /// @param r The RoleEnum value to set.
  void set_typed_role(RoleEnum r) noexcept { role = std::string(to_string(r)); }

  /// @brief Equality comparison operator.
  bool operator==(const CreateMessageResult &) const noexcept = default;
};

/// @brief JSON Schema definition for a tool's input parameters.
struct ToolInputSchema {
  std::string type; ///< Schema root type ("object").
  std::optional<std::string>
      properties_json; ///< Serialized JSON object of property schemas.
  std::optional<std::vector<std::string>>
      required; ///< List of required property names.

  /// @brief Equality comparison operator.
  bool operator==(const ToolInputSchema &) const noexcept = default;
};

/// @brief Definition of an executable tool exposed by an MCP server.
struct Tool {
  std::string name; ///< Unique identifier name of the tool.
  std::optional<std::string>
      description; ///< Human-readable description of what the tool does.
  ToolInputSchema
      inputSchema; ///< JSON Schema defining expected tool arguments.

  /// @brief Equality comparison operator.
  bool operator==(const Tool &) const noexcept = default;
};

/// @brief Parameters for a `notifications/tools/list_changed` notification.
struct ToolListChangedNotificationParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const ToolListChangedNotificationParams &) const noexcept =
      default;
};

/// @brief Notification sent by the server when the list of available tools
/// changes.
struct ToolListChangedNotification {
  std::string method; ///< Method name ("notifications/tools/list_changed").
  std::optional<ToolListChangedNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ToolListChangedNotification &) const noexcept = default;
};

/// @brief Opaque pagination cursor token string.
using Cursor = std::string;

/// @brief Parameters for paginated MCP requests.
struct PaginatedRequestParams {
  std::optional<Cursor>
      cursor; ///< Opaque cursor position from a previous PaginatedResult.

  /// @brief Equality comparison operator.
  bool operator==(const PaginatedRequestParams &) const noexcept = default;
};

/// @brief Base structure for paginated client requests.
struct PaginatedRequest {
  std::string method;            ///< RPC method name.
  PaginatedRequestParams params; ///< Pagination parameters.

  /// @brief Equality comparison operator.
  bool operator==(const PaginatedRequest &) const noexcept = default;
};

/// @brief Base structure for paginated server results.
struct PaginatedResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<Cursor> nextCursor; ///< Opaque cursor for fetching the next
                                    ///< page if more results exist.

  /// @brief Equality comparison operator.
  bool operator==(const PaginatedResult &) const noexcept = default;
};

/// @brief Name and version of an MCP client or server implementation.
struct Implementation {
  std::string name;    ///< Name of the MCP implementation.
  std::string version; ///< Version string of the MCP implementation.

  /// @brief Equality comparison operator.
  bool operator==(const Implementation &) const noexcept = default;
};

/// @brief Parameters for the initial `initialize` handshake request.
struct InitializeRequestParams {
  std::string
      protocolVersion; ///< Latest MCP protocol version supported by the client.
  std::optional<ClientCapabilities>
      capabilities;          ///< Capabilities supported by the client.
  Implementation clientInfo; ///< Client implementation name and version.

  /// @brief Equality comparison operator.
  bool operator==(const InitializeRequestParams &) const noexcept = default;
};

/// @brief Handshake request sent by the client when first connecting
/// (`initialize`).
struct InitializeRequest {
  std::string method;             ///< Method name ("initialize").
  InitializeRequestParams params; ///< Initialization parameters.

  /// @brief Equality comparison operator.
  bool operator==(const InitializeRequest &) const noexcept = default;
};

/// @brief Server capability marker for structured logging support.
struct ServerCapabilitiesLogging {
  /// @brief Equality comparison operator.
  bool operator==(const ServerCapabilitiesLogging &) const noexcept = default;
};

/// @brief Server capability flags for prompts.
struct ServerCapabilitiesPrompts {
  std::optional<bool>
      listChanged; ///< True if server emits prompt list change notifications.

  /// @brief Equality comparison operator.
  bool operator==(const ServerCapabilitiesPrompts &) const noexcept = default;
};

/// @brief Server capability flags for resources.
struct ServerCapabilitiesResources {
  std::optional<bool> subscribe; ///< True if server supports subscribing to
                                 ///< individual resource updates.
  std::optional<bool>
      listChanged; ///< True if server emits resource list change notifications.

  /// @brief Equality comparison operator.
  bool operator==(const ServerCapabilitiesResources &) const noexcept = default;
};

/// @brief Server capability flags for tools.
struct ServerCapabilitiesTools {
  std::optional<bool>
      listChanged; ///< True if server emits tool list change notifications.

  /// @brief Equality comparison operator.
  bool operator==(const ServerCapabilitiesTools &) const noexcept = default;
};

/// @brief Capabilities advertised by an MCP server during initialization.
struct ServerCapabilities {
  std::optional<std::string>
      experimental; ///< Optional experimental capabilities JSON.
  std::optional<ServerCapabilitiesLogging>
      logging; ///< Present if server supports logging.
  std::optional<ServerCapabilitiesPrompts>
      prompts; ///< Present if server offers prompt templates.
  std::optional<ServerCapabilitiesResources>
      resources; ///< Present if server offers readable resources.
  std::optional<ServerCapabilitiesTools>
      tools; ///< Present if server offers callable tools.

  /// @brief Equality comparison operator.
  bool operator==(const ServerCapabilities &) const noexcept = default;
};

/// @brief Result returned by the server in response to an `initialize` request.
struct InitializeResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::string protocolVersion;      ///< Negotiated MCP protocol version string.
  ServerCapabilities capabilities;  ///< Capabilities supported by the server.
  Implementation serverInfo;        ///< Server implementation name and version.
  std::optional<std::string>
      instructions; ///< Optional usage instructions for the client/LLM.

  /// @brief Equality comparison operator.
  bool operator==(const InitializeResult &) const noexcept = default;
};

/// @brief Parameters for a `notifications/initialized` notification.
struct InitializedNotificationParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool
  operator==(const InitializedNotificationParams &) const noexcept = default;
};

/// @brief Notification sent by the client after receiving `InitializeResult`.
struct InitializedNotification {
  std::string method; ///< Method name ("notifications/initialized").
  std::optional<InitializedNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool operator==(const InitializedNotification &) const noexcept = default;
};

/// @brief JSON-RPC 2.0 request identifier (serialized string or integer
/// representation).
using RequestId = std::string;

/// @brief Strongly typed variant for JSON-RPC 2.0 request IDs (either 64-bit
/// integer or string).
using TypedRequestId = std::variant<int64_t, std::string>;

/// @brief Error payload inside a JSON-RPC 2.0 error response.
struct JSONRPCErrorError {
  int code = 0;        ///< Standard JSON-RPC 2.0 integer error code.
  std::string message; ///< Short description of the error.
  std::optional<std::string> data; ///< Optional additional error data JSON.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCErrorError &) const noexcept = default;
};

/// @brief JSON-RPC 2.0 error response envelope.
struct JSONRPCError {
  std::string jsonrpc;     ///< JSON-RPC protocol version ("2.0").
  RequestId id;            ///< Request identifier matching the failed request.
  JSONRPCErrorError error; ///< Detailed error object.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCError &) const noexcept = default;
};

/// @brief Parameters payload for a generic JSON-RPC 2.0 notification.
struct JSONRPCNotificationParams {
  std::optional<std::string> _meta; ///< Optional `_meta` object JSON.
  std::optional<std::string> _raw;  ///< Additional raw JSON properties.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCNotificationParams &) const noexcept = default;
};

/// @brief Generic JSON-RPC 2.0 notification message (no `id`).
struct JSONRPCNotification {
  std::string jsonrpc; ///< JSON-RPC protocol version ("2.0").
  std::string method;  ///< Notification method name.
  std::optional<JSONRPCNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCNotification &) const noexcept = default;
};

/// @brief Parameters payload for a generic JSON-RPC 2.0 request.
struct JSONRPCRequestParams {
  std::optional<std::string> _meta; ///< Optional `_meta` object JSON.
  std::optional<std::string> _raw;  ///< Additional raw JSON properties.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCRequestParams &) const noexcept = default;
};

/// @brief Generic JSON-RPC 2.0 request message.
struct JSONRPCRequest {
  std::string jsonrpc; ///< JSON-RPC protocol version ("2.0").
  RequestId id;        ///< Unique request identifier.
  std::string method;  ///< Invoked method name.
  std::optional<JSONRPCRequestParams> params; ///< Optional method parameters.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCRequest &) const noexcept = default;
};

/// @brief Generic JSON-RPC 2.0 successful response message.
struct JSONRPCResponse {
  std::string jsonrpc;     ///< JSON-RPC protocol version ("2.0").
  RequestId id;            ///< Request identifier matching the request.
  std::string result_json; ///< Serialized JSON result payload.

  /// @brief Equality comparison operator.
  bool operator==(const JSONRPCResponse &) const noexcept = default;
};

/// @brief Parameters for a `prompts/list` request.
struct ListPromptsRequestParams {
  std::optional<Cursor> cursor; ///< Optional pagination cursor.

  /// @brief Equality comparison operator.
  bool operator==(const ListPromptsRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to list available prompt templates
/// (`prompts/list`).
struct ListPromptsRequest {
  std::string method; ///< Method name ("prompts/list").
  std::optional<ListPromptsRequestParams>
      params; ///< Optional pagination parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ListPromptsRequest &) const noexcept = default;
};

/// @brief Argument definition for an MCP Prompt template.
struct PromptArgument {
  std::string name; ///< Name of the prompt argument.
  std::optional<std::string>
      description;              ///< Human-readable description of the argument.
  std::optional<bool> required; ///< Whether the argument must be provided.

  /// @brief Equality comparison operator.
  bool operator==(const PromptArgument &) const noexcept = default;
};

/// @brief Prompt template exposed by an MCP server.
struct Prompt {
  std::string name; ///< Unique name of the prompt template.
  std::optional<std::string>
      description; ///< Optional description of what the prompt does.
  std::optional<std::vector<PromptArgument>>
      arguments; ///< Optional list of template arguments.

  /// @brief Equality comparison operator.
  bool operator==(const Prompt &) const noexcept = default;
};

/// @brief Result returned by the server for a `prompts/list` request.
struct ListPromptsResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<Cursor>
      nextCursor; ///< Optional cursor for the next page of prompts.
  std::vector<Prompt> prompts; ///< List of available prompt templates.

  /// @brief Equality comparison operator.
  bool operator==(const ListPromptsResult &) const noexcept = default;
};

/// @brief Parameters for retrieving a specific prompt (`prompts/get`).
struct GetPromptRequestParams {
  std::string name; ///< Name of the prompt to retrieve.
  std::optional<std::string>
      arguments; ///< Optional JSON object map of argument key/values.

  /// @brief Equality comparison operator.
  bool operator==(const GetPromptRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to render a prompt template
/// (`prompts/get`).
struct GetPromptRequest {
  std::string method;            ///< Method name ("prompts/get").
  GetPromptRequestParams params; ///< Prompt retrieval parameters.

  /// @brief Equality comparison operator.
  bool operator==(const GetPromptRequest &) const noexcept = default;
};

/// @brief Single message within a rendered prompt template.
struct PromptMessage {
  std::string role;         ///< Sender role ("user" or "assistant").
  std::string content_json; ///< Serialized JSON of TextContent, ImageContent,
                            ///< or EmbeddedResource.

  /// @brief Get the strongly typed RoleEnum for this message.
  /// @return Optional RoleEnum value.
  std::optional<RoleEnum> typed_role() const noexcept {
    return role_from_string(role);
  }

  /// @brief Set the role from a strongly typed RoleEnum.
  /// @param r The RoleEnum value to set.
  void set_typed_role(RoleEnum r) noexcept { role = std::string(to_string(r)); }

  /// @brief Equality comparison operator.
  bool operator==(const PromptMessage &) const noexcept = default;
};

/// @brief Result returned by the server for a `prompts/get` request.
struct GetPromptResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<std::string>
      description;                     ///< Optional description of the prompt.
  std::vector<PromptMessage> messages; ///< Rendered prompt messages.

  /// @brief Equality comparison operator.
  bool operator==(const GetPromptResult &) const noexcept = default;
};

/// @brief Parameters for a `notifications/prompts/list_changed` notification.
struct PromptListChangedNotificationParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const PromptListChangedNotificationParams &) const noexcept =
      default;
};

/// @brief Notification sent by the server when the list of available prompts
/// changes.
struct PromptListChangedNotification {
  std::string method; ///< Method name ("notifications/prompts/list_changed").
  std::optional<PromptListChangedNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool
  operator==(const PromptListChangedNotification &) const noexcept = default;
};

/// @brief Parameters for a `resources/list` request.
struct ListResourcesRequestParams {
  std::optional<Cursor> cursor; ///< Optional pagination cursor.

  /// @brief Equality comparison operator.
  bool operator==(const ListResourcesRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to list available resources
/// (`resources/list`).
struct ListResourcesRequest {
  std::string method; ///< Method name ("resources/list").
  std::optional<ListResourcesRequestParams>
      params; ///< Optional pagination parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ListResourcesRequest &) const noexcept = default;
};

/// @brief A readable resource exposed by an MCP server.
struct Resource {
  std::string uri;  ///< Unique URI identifying the resource.
  std::string name; ///< Human-readable name of the resource.
  std::optional<std::string>
      description; ///< Optional description of the resource.
  std::optional<std::string> mimeType;    ///< Optional MIME type if known.
  std::optional<Annotations> annotations; ///< Optional client annotations.

  /// @brief Equality comparison operator.
  bool operator==(const Resource &) const noexcept = default;
};

/// @brief Result returned by the server for a `resources/list` request.
struct ListResourcesResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<Cursor> nextCursor; ///< Optional cursor for the next page.
  std::vector<Resource> resources;  ///< List of available resources.

  /// @brief Equality comparison operator.
  bool operator==(const ListResourcesResult &) const noexcept = default;
};

/// @brief Parameters for a `resources/templates/list` request.
struct ListResourceTemplatesRequestParams {
  std::optional<Cursor> cursor; ///< Optional pagination cursor.

  /// @brief Equality comparison operator.
  bool operator==(const ListResourceTemplatesRequestParams &) const noexcept =
      default;
};

/// @brief Request sent by the client to list parameterized resource URI
/// templates.
struct ListResourceTemplatesRequest {
  std::string method; ///< Method name ("resources/templates/list").
  std::optional<ListResourceTemplatesRequestParams>
      params; ///< Optional pagination parameters.

  /// @brief Equality comparison operator.
  bool
  operator==(const ListResourceTemplatesRequest &) const noexcept = default;
};

/// @brief RFC 6570 URI template describing dynamic resources on the server.
struct ResourceTemplate {
  std::string uriTemplate; ///< RFC 6570 URI template string.
  std::string name;        ///< Human-readable name for the template.
  std::optional<std::string>
      description; ///< Optional description of the template.
  std::optional<std::string>
      mimeType; ///< Optional MIME type for matching resources.
  std::optional<Annotations> annotations; ///< Optional client annotations.

  /// @brief Equality comparison operator.
  bool operator==(const ResourceTemplate &) const noexcept = default;
};

/// @brief Result returned by the server for a `resources/templates/list`
/// request.
struct ListResourceTemplatesResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<Cursor> nextCursor; ///< Optional cursor for the next page.
  std::vector<ResourceTemplate>
      resourceTemplates; ///< List of resource templates.

  /// @brief Equality comparison operator.
  bool operator==(const ListResourceTemplatesResult &) const noexcept = default;
};

/// @brief Parameters for a `resources/read` request.
struct ReadResourceRequestParams {
  std::string uri; ///< The URI of the resource to read.

  /// @brief Equality comparison operator.
  bool operator==(const ReadResourceRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to read the contents of a resource
/// (`resources/read`).
struct ReadResourceRequest {
  std::string method;               ///< Method name ("resources/read").
  ReadResourceRequestParams params; ///< Resource read parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ReadResourceRequest &) const noexcept = default;
};

/// @brief Result returned by the server for a `resources/read` request.
struct ReadResourceResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::vector<std::string>
      contents_json; ///< Serialized JSON objects (TextResourceContents or
                     ///< BlobResourceContents).

  /// @brief Equality comparison operator.
  bool operator==(const ReadResourceResult &) const noexcept = default;
};

/// @brief Parameters for a `notifications/resources/updated` notification.
struct ResourceUpdatedNotificationParams {
  std::string uri; ///< The URI of the resource that was updated.

  /// @brief Equality comparison operator.
  bool operator==(const ResourceUpdatedNotificationParams &) const noexcept =
      default;
};

/// @brief Notification sent by the server when a subscribed resource changes.
struct ResourceUpdatedNotification {
  std::string method; ///< Method name ("notifications/resources/updated").
  ResourceUpdatedNotificationParams params; ///< Updated resource parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ResourceUpdatedNotification &) const noexcept = default;
};

/// @brief Parameters for a `notifications/resources/list_changed` notification.
struct ResourceListChangedNotificationParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(
      const ResourceListChangedNotificationParams &) const noexcept = default;
};

/// @brief Notification sent by the server when the list of available resources
/// changes.
struct ResourceListChangedNotification {
  std::string method; ///< Method name ("notifications/resources/list_changed").
  std::optional<ResourceListChangedNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool
  operator==(const ResourceListChangedNotification &) const noexcept = default;
};

/// @brief Generic MCP result object carrying optional `_meta`.
struct Result {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const Result &) const noexcept = default;
};

/// @brief Filesystem or workspace root URI exposed by the client to the server.
struct Root {
  std::string uri; ///< `file://` URI of the root directory.
  std::optional<std::string>
      name; ///< Optional human-readable name for the root.

  /// @brief Equality comparison operator.
  bool operator==(const Root &) const noexcept = default;
};

/// @brief Parameters for a `notifications/roots/list_changed` notification.
struct RootsListChangedNotificationParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const RootsListChangedNotificationParams &) const noexcept =
      default;
};

/// @brief Notification sent by the client when its list of roots changes.
struct RootsListChangedNotification {
  std::string method; ///< Method name ("notifications/roots/list_changed").
  std::optional<RootsListChangedNotificationParams>
      params; ///< Optional notification parameters.

  /// @brief Equality comparison operator.
  bool
  operator==(const RootsListChangedNotification &) const noexcept = default;
};

/// @brief Parameters for a `roots/list` request.
struct ListRootsRequestParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const ListRootsRequestParams &) const noexcept = default;
};

/// @brief Request sent from the server to the client to list filesystem roots
/// (`roots/list`).
struct ListRootsRequest {
  std::string method; ///< Method name ("roots/list").
  std::optional<ListRootsRequestParams>
      params; ///< Optional request parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ListRootsRequest &) const noexcept = default;
};

/// @brief Result returned by the client for a `roots/list` request.
struct ListRootsResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::vector<Root> roots;          ///< List of client root directories.

  /// @brief Equality comparison operator.
  bool operator==(const ListRootsResult &) const noexcept = default;
};

/// @brief Logging level string alias (use LoggingLevelEnum for strong typing).
using LoggingLevel = std::string;

/// @brief Parameters for a `logging/setLevel` request.
struct SetLevelRequestParams {
  LoggingLevel level; ///< Minimum logging severity level to emit.

  /// @brief Get the strongly typed LoggingLevelEnum for this parameter.
  /// @return Optional LoggingLevelEnum value.
  std::optional<LoggingLevelEnum> typed_level() const noexcept {
    return logging_level_from_string(level);
  }

  /// @brief Set the logging level from a strongly typed LoggingLevelEnum.
  /// @param lvl The LoggingLevelEnum value to set.
  void set_typed_level(LoggingLevelEnum lvl) noexcept {
    level = std::string(to_string(lvl));
  }

  /// @brief Equality comparison operator.
  bool operator==(const SetLevelRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to configure the server's logging
/// threshold (`logging/setLevel`).
struct SetLevelRequest {
  std::string method;           ///< Method name ("logging/setLevel").
  SetLevelRequestParams params; ///< Logging level parameters.

  /// @brief Equality comparison operator.
  bool operator==(const SetLevelRequest &) const noexcept = default;
};

/// @brief Parameters for a `notifications/message` log notification.
struct LoggingMessageNotificationParams {
  LoggingLevel level; ///< Severity level of the log message.
  std::optional<std::string>
      logger;       ///< Optional name of the logger emitting the message.
  std::string data; ///< Arbitrary serialized JSON log payload.

  /// @brief Get the strongly typed LoggingLevelEnum for this log message.
  /// @return Optional LoggingLevelEnum value.
  std::optional<LoggingLevelEnum> typed_level() const noexcept {
    return logging_level_from_string(level);
  }

  /// @brief Set the logging level from a strongly typed LoggingLevelEnum.
  /// @param lvl The LoggingLevelEnum value to set.
  void set_typed_level(LoggingLevelEnum lvl) noexcept {
    level = std::string(to_string(lvl));
  }

  /// @brief Equality comparison operator.
  bool
  operator==(const LoggingMessageNotificationParams &) const noexcept = default;
};

/// @brief Log message notification sent from the server to the client
/// (`notifications/message`).
struct LoggingMessageNotification {
  std::string method; ///< Method name ("notifications/message").
  LoggingMessageNotificationParams params; ///< Log message payload.

  /// @brief Equality comparison operator.
  bool operator==(const LoggingMessageNotification &) const noexcept = default;
};

/// @brief Token associating progress notifications with an in-flight request
/// (string or integer).
using ProgressToken = std::string;

/// @brief Parameters for a `notifications/progress` notification.
struct ProgressNotificationParams {
  ProgressToken
      progressToken;     ///< Opaque progress token from the request's `_meta`.
  double progress = 0.0; ///< Current progress value.
  std::optional<double> total; ///< Optional total target progress value.

  /// @brief Equality comparison operator.
  bool operator==(const ProgressNotificationParams &) const noexcept = default;
};

/// @brief Out-of-band progress notification for a long-running request
/// (`notifications/progress`).
struct ProgressNotification {
  std::string method; ///< Method name ("notifications/progress").
  ProgressNotificationParams params; ///< Progress notification parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ProgressNotification &) const noexcept = default;
};

/// @brief Empty result acknowledgement carrying optional `_meta`.
using EmptyResult = Result;

/// @brief Role string alias ("user" | "assistant"; see RoleEnum for strong
/// typing).
using Role = std::string;

/// @brief Message exchanged in an LLM sampling request
/// (`sampling/createMessage`).
struct SamplingMessage {
  Role role;                ///< Participant role ("user" or "assistant").
  std::string content_json; ///< Serialized JSON of TextContent or ImageContent.

  /// @brief Get the strongly typed RoleEnum for this message.
  /// @return Optional RoleEnum value.
  std::optional<RoleEnum> typed_role() const noexcept {
    return role_from_string(role);
  }

  /// @brief Set the role from a strongly typed RoleEnum.
  /// @param r The RoleEnum value to set.
  void set_typed_role(RoleEnum r) noexcept { role = std::string(to_string(r)); }

  /// @brief Equality comparison operator.
  bool operator==(const SamplingMessage &) const noexcept = default;
};

/// @brief Parameters for a `ping` request.
struct PingRequestParams {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.

  /// @brief Equality comparison operator.
  bool operator==(const PingRequestParams &) const noexcept = default;
};

/// @brief Liveness check request sent by either client or server (`ping`).
struct PingRequest {
  std::string method;                      ///< Method name ("ping").
  std::optional<PingRequestParams> params; ///< Optional ping parameters.

  /// @brief Equality comparison operator.
  bool operator==(const PingRequest &) const noexcept = default;
};

/// @brief Parameters for a `resources/subscribe` request.
struct SubscribeRequestParams {
  std::string uri; ///< The URI of the resource to subscribe to.

  /// @brief Equality comparison operator.
  bool operator==(const SubscribeRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to subscribe to resource update
/// notifications (`resources/subscribe`).
struct SubscribeRequest {
  std::string method;            ///< Method name ("resources/subscribe").
  SubscribeRequestParams params; ///< Subscription parameters.

  /// @brief Equality comparison operator.
  bool operator==(const SubscribeRequest &) const noexcept = default;
};

/// @brief Parameters for a `resources/unsubscribe` request.
struct UnsubscribeRequestParams {
  std::string uri; ///< The URI of the resource to unsubscribe from.

  /// @brief Equality comparison operator.
  bool operator==(const UnsubscribeRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to cancel a resource subscription
/// (`resources/unsubscribe`).
struct UnsubscribeRequest {
  std::string method;              ///< Method name ("resources/unsubscribe").
  UnsubscribeRequestParams params; ///< Unsubscription parameters.

  /// @brief Equality comparison operator.
  bool operator==(const UnsubscribeRequest &) const noexcept = default;
};

/// @brief Parameters for a `tools/list` request.
struct ListToolsRequestParams {
  std::optional<Cursor> cursor; ///< Optional pagination cursor.

  /// @brief Equality comparison operator.
  bool operator==(const ListToolsRequestParams &) const noexcept = default;
};

/// @brief Request sent by the client to list available server tools
/// (`tools/list`).
struct ListToolsRequest {
  std::string method; ///< Method name ("tools/list").
  std::optional<ListToolsRequestParams>
      params; ///< Optional pagination parameters.

  /// @brief Equality comparison operator.
  bool operator==(const ListToolsRequest &) const noexcept = default;
};

/// @brief Result returned by the server for a `tools/list` request.
struct ListToolsResult {
  std::optional<std::string> _meta; ///< Optional protocol metadata JSON.
  std::optional<Cursor>
      nextCursor;          ///< Optional cursor for the next page of tools.
  std::vector<Tool> tools; ///< List of available tools.

  /// @brief Equality comparison operator.
  bool operator==(const ListToolsResult &) const noexcept = default;
};

} // namespace cdd_cpp::mcp
