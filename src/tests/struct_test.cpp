#include "../mcp/models.hpp"
#include "../openapi/models.hpp"
#include <utility>
namespace cdd_cpp::struct_tests {
void test_structs_openapi() {
  cdd_cpp::openapi::Reference v1_Reference;
  cdd_cpp::openapi::Reference v2_Reference(v1_Reference);
  cdd_cpp::openapi::Reference v3_Reference(std::move(v1_Reference));
  v2_Reference = v3_Reference;
  v3_Reference = std::move(v2_Reference);
  cdd_cpp::openapi::Reference v4_Reference = v3_Reference;
  (void)v4_Reference;
  cdd_cpp::openapi::Contact v1_Contact;
  cdd_cpp::openapi::Contact v2_Contact(v1_Contact);
  cdd_cpp::openapi::Contact v3_Contact(std::move(v1_Contact));
  v2_Contact = v3_Contact;
  v3_Contact = std::move(v2_Contact);
  cdd_cpp::openapi::Contact v4_Contact = v3_Contact;
  (void)v4_Contact;
  cdd_cpp::openapi::License v1_License;
  cdd_cpp::openapi::License v2_License(v1_License);
  cdd_cpp::openapi::License v3_License(std::move(v1_License));
  v2_License = v3_License;
  v3_License = std::move(v2_License);
  cdd_cpp::openapi::License v4_License = v3_License;
  (void)v4_License;
  cdd_cpp::openapi::Info v1_Info;
  cdd_cpp::openapi::Info v2_Info(v1_Info);
  cdd_cpp::openapi::Info v3_Info(std::move(v1_Info));
  v2_Info = v3_Info;
  v3_Info = std::move(v2_Info);
  cdd_cpp::openapi::Info v4_Info = v3_Info;
  (void)v4_Info;
  cdd_cpp::openapi::ServerVariable v1_ServerVariable;
  cdd_cpp::openapi::ServerVariable v2_ServerVariable(v1_ServerVariable);
  cdd_cpp::openapi::ServerVariable v3_ServerVariable(
      std::move(v1_ServerVariable));
  v2_ServerVariable = v3_ServerVariable;
  v3_ServerVariable = std::move(v2_ServerVariable);
  cdd_cpp::openapi::ServerVariable v4_ServerVariable = v3_ServerVariable;
  (void)v4_ServerVariable;
  cdd_cpp::openapi::Server v1_Server;
  cdd_cpp::openapi::Server v2_Server(v1_Server);
  cdd_cpp::openapi::Server v3_Server(std::move(v1_Server));
  v2_Server = v3_Server;
  v3_Server = std::move(v2_Server);
  cdd_cpp::openapi::Server v4_Server = v3_Server;
  (void)v4_Server;
  cdd_cpp::openapi::ExternalDocumentation v1_ExternalDocumentation;
  cdd_cpp::openapi::ExternalDocumentation v2_ExternalDocumentation(
      v1_ExternalDocumentation);
  cdd_cpp::openapi::ExternalDocumentation v3_ExternalDocumentation(
      std::move(v1_ExternalDocumentation));
  v2_ExternalDocumentation = v3_ExternalDocumentation;
  v3_ExternalDocumentation = std::move(v2_ExternalDocumentation);
  cdd_cpp::openapi::ExternalDocumentation v4_ExternalDocumentation =
      v3_ExternalDocumentation;
  (void)v4_ExternalDocumentation;
  cdd_cpp::openapi::Tag v1_Tag;
  cdd_cpp::openapi::Tag v2_Tag(v1_Tag);
  cdd_cpp::openapi::Tag v3_Tag(std::move(v1_Tag));
  v2_Tag = v3_Tag;
  v3_Tag = std::move(v2_Tag);
  cdd_cpp::openapi::Tag v4_Tag = v3_Tag;
  (void)v4_Tag;
  cdd_cpp::openapi::Discriminator v1_Discriminator;
  cdd_cpp::openapi::Discriminator v2_Discriminator(v1_Discriminator);
  cdd_cpp::openapi::Discriminator v3_Discriminator(std::move(v1_Discriminator));
  v2_Discriminator = v3_Discriminator;
  v3_Discriminator = std::move(v2_Discriminator);
  cdd_cpp::openapi::Discriminator v4_Discriminator = v3_Discriminator;
  (void)v4_Discriminator;
  cdd_cpp::openapi::XML v1_XML;
  cdd_cpp::openapi::XML v2_XML(v1_XML);
  cdd_cpp::openapi::XML v3_XML(std::move(v1_XML));
  v2_XML = v3_XML;
  v3_XML = std::move(v2_XML);
  cdd_cpp::openapi::XML v4_XML = v3_XML;
  (void)v4_XML;
  cdd_cpp::openapi::Schema v1_Schema;
  cdd_cpp::openapi::Schema v2_Schema(v1_Schema);
  cdd_cpp::openapi::Schema v3_Schema(std::move(v1_Schema));
  v2_Schema = v3_Schema;
  v3_Schema = std::move(v2_Schema);
  cdd_cpp::openapi::Schema v4_Schema = v3_Schema;
  (void)v4_Schema;
  cdd_cpp::openapi::Example v1_Example;
  cdd_cpp::openapi::Example v2_Example(v1_Example);
  cdd_cpp::openapi::Example v3_Example(std::move(v1_Example));
  v2_Example = v3_Example;
  v3_Example = std::move(v2_Example);
  cdd_cpp::openapi::Example v4_Example = v3_Example;
  (void)v4_Example;
  cdd_cpp::openapi::Encoding v1_Encoding;
  cdd_cpp::openapi::Encoding v2_Encoding(v1_Encoding);
  cdd_cpp::openapi::Encoding v3_Encoding(std::move(v1_Encoding));
  v2_Encoding = v3_Encoding;
  v3_Encoding = std::move(v2_Encoding);
  cdd_cpp::openapi::Encoding v4_Encoding = v3_Encoding;
  (void)v4_Encoding;
  cdd_cpp::openapi::MediaType v1_MediaType;
  cdd_cpp::openapi::MediaType v2_MediaType(v1_MediaType);
  cdd_cpp::openapi::MediaType v3_MediaType(std::move(v1_MediaType));
  v2_MediaType = v3_MediaType;
  v3_MediaType = std::move(v2_MediaType);
  cdd_cpp::openapi::MediaType v4_MediaType = v3_MediaType;
  (void)v4_MediaType;
  cdd_cpp::openapi::Header v1_Header;
  cdd_cpp::openapi::Header v2_Header(v1_Header);
  cdd_cpp::openapi::Header v3_Header(std::move(v1_Header));
  v2_Header = v3_Header;
  v3_Header = std::move(v2_Header);
  cdd_cpp::openapi::Header v4_Header = v3_Header;
  (void)v4_Header;
  cdd_cpp::openapi::RequestBody v1_RequestBody;
  cdd_cpp::openapi::RequestBody v2_RequestBody(v1_RequestBody);
  cdd_cpp::openapi::RequestBody v3_RequestBody(std::move(v1_RequestBody));
  v2_RequestBody = v3_RequestBody;
  v3_RequestBody = std::move(v2_RequestBody);
  cdd_cpp::openapi::RequestBody v4_RequestBody = v3_RequestBody;
  (void)v4_RequestBody;
  cdd_cpp::openapi::Link v1_Link;
  cdd_cpp::openapi::Link v2_Link(v1_Link);
  cdd_cpp::openapi::Link v3_Link(std::move(v1_Link));
  v2_Link = v3_Link;
  v3_Link = std::move(v2_Link);
  cdd_cpp::openapi::Link v4_Link = v3_Link;
  (void)v4_Link;
  cdd_cpp::openapi::Response v1_Response;
  cdd_cpp::openapi::Response v2_Response(v1_Response);
  cdd_cpp::openapi::Response v3_Response(std::move(v1_Response));
  v2_Response = v3_Response;
  v3_Response = std::move(v2_Response);
  cdd_cpp::openapi::Response v4_Response = v3_Response;
  (void)v4_Response;
  cdd_cpp::openapi::Parameter v1_Parameter;
  cdd_cpp::openapi::Parameter v2_Parameter(v1_Parameter);
  cdd_cpp::openapi::Parameter v3_Parameter(std::move(v1_Parameter));
  v2_Parameter = v3_Parameter;
  v3_Parameter = std::move(v2_Parameter);
  cdd_cpp::openapi::Parameter v4_Parameter = v3_Parameter;
  (void)v4_Parameter;
  cdd_cpp::openapi::OAuthFlow v1_OAuthFlow;
  cdd_cpp::openapi::OAuthFlow v2_OAuthFlow(v1_OAuthFlow);
  cdd_cpp::openapi::OAuthFlow v3_OAuthFlow(std::move(v1_OAuthFlow));
  v2_OAuthFlow = v3_OAuthFlow;
  v3_OAuthFlow = std::move(v2_OAuthFlow);
  cdd_cpp::openapi::OAuthFlow v4_OAuthFlow = v3_OAuthFlow;
  (void)v4_OAuthFlow;
  cdd_cpp::openapi::OAuthFlows v1_OAuthFlows;
  cdd_cpp::openapi::OAuthFlows v2_OAuthFlows(v1_OAuthFlows);
  cdd_cpp::openapi::OAuthFlows v3_OAuthFlows(std::move(v1_OAuthFlows));
  v2_OAuthFlows = v3_OAuthFlows;
  v3_OAuthFlows = std::move(v2_OAuthFlows);
  cdd_cpp::openapi::OAuthFlows v4_OAuthFlows = v3_OAuthFlows;
  (void)v4_OAuthFlows;
  cdd_cpp::openapi::SecurityScheme v1_SecurityScheme;
  cdd_cpp::openapi::SecurityScheme v2_SecurityScheme(v1_SecurityScheme);
  cdd_cpp::openapi::SecurityScheme v3_SecurityScheme(
      std::move(v1_SecurityScheme));
  v2_SecurityScheme = v3_SecurityScheme;
  v3_SecurityScheme = std::move(v2_SecurityScheme);
  cdd_cpp::openapi::SecurityScheme v4_SecurityScheme = v3_SecurityScheme;
  (void)v4_SecurityScheme;
  cdd_cpp::openapi::Operation v1_Operation;
  cdd_cpp::openapi::Operation v2_Operation(v1_Operation);
  cdd_cpp::openapi::Operation v3_Operation(std::move(v1_Operation));
  v2_Operation = v3_Operation;
  v3_Operation = std::move(v2_Operation);
  cdd_cpp::openapi::Operation v4_Operation = v3_Operation;
  (void)v4_Operation;
  cdd_cpp::openapi::PathItem v1_PathItem;
  cdd_cpp::openapi::PathItem v2_PathItem(v1_PathItem);
  cdd_cpp::openapi::PathItem v3_PathItem(std::move(v1_PathItem));
  v2_PathItem = v3_PathItem;
  v3_PathItem = std::move(v2_PathItem);
  cdd_cpp::openapi::PathItem v4_PathItem = v3_PathItem;
  (void)v4_PathItem;
  cdd_cpp::openapi::Components v1_Components;
  cdd_cpp::openapi::Components v2_Components(v1_Components);
  cdd_cpp::openapi::Components v3_Components(std::move(v1_Components));
  v2_Components = v3_Components;
  v3_Components = std::move(v2_Components);
  cdd_cpp::openapi::Components v4_Components = v3_Components;
  (void)v4_Components;
  cdd_cpp::openapi::OpenAPI v1_OpenAPI;
  cdd_cpp::openapi::OpenAPI v2_OpenAPI(v1_OpenAPI);
  cdd_cpp::openapi::OpenAPI v3_OpenAPI(std::move(v1_OpenAPI));
  v2_OpenAPI = v3_OpenAPI;
  v3_OpenAPI = std::move(v2_OpenAPI);
  cdd_cpp::openapi::OpenAPI v4_OpenAPI = v3_OpenAPI;
  (void)v4_OpenAPI;
}
void test_structs_mcp() {
  cdd_cpp::mcp::Annotations v1_Annotations;
  cdd_cpp::mcp::Annotations v2_Annotations(v1_Annotations);
  cdd_cpp::mcp::Annotations v3_Annotations(std::move(v1_Annotations));
  v2_Annotations = v3_Annotations;
  v3_Annotations = std::move(v2_Annotations);
  cdd_cpp::mcp::Annotations v4_Annotations = v3_Annotations;
  (void)v4_Annotations;
  cdd_cpp::mcp::Annotated v1_Annotated;
  cdd_cpp::mcp::Annotated v2_Annotated(v1_Annotated);
  cdd_cpp::mcp::Annotated v3_Annotated(std::move(v1_Annotated));
  v2_Annotated = v3_Annotated;
  v3_Annotated = std::move(v2_Annotated);
  cdd_cpp::mcp::Annotated v4_Annotated = v3_Annotated;
  (void)v4_Annotated;
  cdd_cpp::mcp::BlobResourceContents v1_BlobResourceContents;
  cdd_cpp::mcp::BlobResourceContents v2_BlobResourceContents(
      v1_BlobResourceContents);
  cdd_cpp::mcp::BlobResourceContents v3_BlobResourceContents(
      std::move(v1_BlobResourceContents));
  v2_BlobResourceContents = v3_BlobResourceContents;
  v3_BlobResourceContents = std::move(v2_BlobResourceContents);
  cdd_cpp::mcp::BlobResourceContents v4_BlobResourceContents =
      v3_BlobResourceContents;
  (void)v4_BlobResourceContents;
  cdd_cpp::mcp::CallToolRequestParams v1_CallToolRequestParams;
  cdd_cpp::mcp::CallToolRequestParams v2_CallToolRequestParams(
      v1_CallToolRequestParams);
  cdd_cpp::mcp::CallToolRequestParams v3_CallToolRequestParams(
      std::move(v1_CallToolRequestParams));
  v2_CallToolRequestParams = v3_CallToolRequestParams;
  v3_CallToolRequestParams = std::move(v2_CallToolRequestParams);
  cdd_cpp::mcp::CallToolRequestParams v4_CallToolRequestParams =
      v3_CallToolRequestParams;
  (void)v4_CallToolRequestParams;
  cdd_cpp::mcp::CallToolRequest v1_CallToolRequest;
  cdd_cpp::mcp::CallToolRequest v2_CallToolRequest(v1_CallToolRequest);
  cdd_cpp::mcp::CallToolRequest v3_CallToolRequest(
      std::move(v1_CallToolRequest));
  v2_CallToolRequest = v3_CallToolRequest;
  v3_CallToolRequest = std::move(v2_CallToolRequest);
  cdd_cpp::mcp::CallToolRequest v4_CallToolRequest = v3_CallToolRequest;
  (void)v4_CallToolRequest;
  cdd_cpp::mcp::TextContent v1_TextContent;
  cdd_cpp::mcp::TextContent v2_TextContent(v1_TextContent);
  cdd_cpp::mcp::TextContent v3_TextContent(std::move(v1_TextContent));
  v2_TextContent = v3_TextContent;
  v3_TextContent = std::move(v2_TextContent);
  cdd_cpp::mcp::TextContent v4_TextContent = v3_TextContent;
  (void)v4_TextContent;
  cdd_cpp::mcp::ImageContent v1_ImageContent;
  cdd_cpp::mcp::ImageContent v2_ImageContent(v1_ImageContent);
  cdd_cpp::mcp::ImageContent v3_ImageContent(std::move(v1_ImageContent));
  v2_ImageContent = v3_ImageContent;
  v3_ImageContent = std::move(v2_ImageContent);
  cdd_cpp::mcp::ImageContent v4_ImageContent = v3_ImageContent;
  (void)v4_ImageContent;
  cdd_cpp::mcp::ResourceContents v1_ResourceContents;
  cdd_cpp::mcp::ResourceContents v2_ResourceContents(v1_ResourceContents);
  cdd_cpp::mcp::ResourceContents v3_ResourceContents(
      std::move(v1_ResourceContents));
  v2_ResourceContents = v3_ResourceContents;
  v3_ResourceContents = std::move(v2_ResourceContents);
  cdd_cpp::mcp::ResourceContents v4_ResourceContents = v3_ResourceContents;
  (void)v4_ResourceContents;
  cdd_cpp::mcp::EmbeddedResource v1_EmbeddedResource;
  cdd_cpp::mcp::EmbeddedResource v2_EmbeddedResource(v1_EmbeddedResource);
  cdd_cpp::mcp::EmbeddedResource v3_EmbeddedResource(
      std::move(v1_EmbeddedResource));
  v2_EmbeddedResource = v3_EmbeddedResource;
  v3_EmbeddedResource = std::move(v2_EmbeddedResource);
  cdd_cpp::mcp::EmbeddedResource v4_EmbeddedResource = v3_EmbeddedResource;
  (void)v4_EmbeddedResource;
  cdd_cpp::mcp::CallToolResult v1_CallToolResult;
  cdd_cpp::mcp::CallToolResult v2_CallToolResult(v1_CallToolResult);
  cdd_cpp::mcp::CallToolResult v3_CallToolResult(std::move(v1_CallToolResult));
  v2_CallToolResult = v3_CallToolResult;
  v3_CallToolResult = std::move(v2_CallToolResult);
  cdd_cpp::mcp::CallToolResult v4_CallToolResult = v3_CallToolResult;
  (void)v4_CallToolResult;
  cdd_cpp::mcp::CancelledNotificationParams v1_CancelledNotificationParams;
  cdd_cpp::mcp::CancelledNotificationParams v2_CancelledNotificationParams(
      v1_CancelledNotificationParams);
  cdd_cpp::mcp::CancelledNotificationParams v3_CancelledNotificationParams(
      std::move(v1_CancelledNotificationParams));
  v2_CancelledNotificationParams = v3_CancelledNotificationParams;
  v3_CancelledNotificationParams = std::move(v2_CancelledNotificationParams);
  cdd_cpp::mcp::CancelledNotificationParams v4_CancelledNotificationParams =
      v3_CancelledNotificationParams;
  (void)v4_CancelledNotificationParams;
  cdd_cpp::mcp::CancelledNotification v1_CancelledNotification;
  cdd_cpp::mcp::CancelledNotification v2_CancelledNotification(
      v1_CancelledNotification);
  cdd_cpp::mcp::CancelledNotification v3_CancelledNotification(
      std::move(v1_CancelledNotification));
  v2_CancelledNotification = v3_CancelledNotification;
  v3_CancelledNotification = std::move(v2_CancelledNotification);
  cdd_cpp::mcp::CancelledNotification v4_CancelledNotification =
      v3_CancelledNotification;
  (void)v4_CancelledNotification;
  cdd_cpp::mcp::ClientCapabilitiesRoots v1_ClientCapabilitiesRoots;
  cdd_cpp::mcp::ClientCapabilitiesRoots v2_ClientCapabilitiesRoots(
      v1_ClientCapabilitiesRoots);
  cdd_cpp::mcp::ClientCapabilitiesRoots v3_ClientCapabilitiesRoots(
      std::move(v1_ClientCapabilitiesRoots));
  v2_ClientCapabilitiesRoots = v3_ClientCapabilitiesRoots;
  v3_ClientCapabilitiesRoots = std::move(v2_ClientCapabilitiesRoots);
  cdd_cpp::mcp::ClientCapabilitiesRoots v4_ClientCapabilitiesRoots =
      v3_ClientCapabilitiesRoots;
  (void)v4_ClientCapabilitiesRoots;
  cdd_cpp::mcp::ClientCapabilities v1_ClientCapabilities;
  cdd_cpp::mcp::ClientCapabilities v2_ClientCapabilities(v1_ClientCapabilities);
  cdd_cpp::mcp::ClientCapabilities v3_ClientCapabilities(
      std::move(v1_ClientCapabilities));
  v2_ClientCapabilities = v3_ClientCapabilities;
  v3_ClientCapabilities = std::move(v2_ClientCapabilities);
  cdd_cpp::mcp::ClientCapabilities v4_ClientCapabilities =
      v3_ClientCapabilities;
  (void)v4_ClientCapabilities;
  cdd_cpp::mcp::CompleteRequestArgument v1_CompleteRequestArgument;
  cdd_cpp::mcp::CompleteRequestArgument v2_CompleteRequestArgument(
      v1_CompleteRequestArgument);
  cdd_cpp::mcp::CompleteRequestArgument v3_CompleteRequestArgument(
      std::move(v1_CompleteRequestArgument));
  v2_CompleteRequestArgument = v3_CompleteRequestArgument;
  v3_CompleteRequestArgument = std::move(v2_CompleteRequestArgument);
  cdd_cpp::mcp::CompleteRequestArgument v4_CompleteRequestArgument =
      v3_CompleteRequestArgument;
  (void)v4_CompleteRequestArgument;
  cdd_cpp::mcp::CompleteRequestParams v1_CompleteRequestParams;
  cdd_cpp::mcp::CompleteRequestParams v2_CompleteRequestParams(
      v1_CompleteRequestParams);
  cdd_cpp::mcp::CompleteRequestParams v3_CompleteRequestParams(
      std::move(v1_CompleteRequestParams));
  v2_CompleteRequestParams = v3_CompleteRequestParams;
  v3_CompleteRequestParams = std::move(v2_CompleteRequestParams);
  cdd_cpp::mcp::CompleteRequestParams v4_CompleteRequestParams =
      v3_CompleteRequestParams;
  (void)v4_CompleteRequestParams;
  cdd_cpp::mcp::CompleteRequest v1_CompleteRequest;
  cdd_cpp::mcp::CompleteRequest v2_CompleteRequest(v1_CompleteRequest);
  cdd_cpp::mcp::CompleteRequest v3_CompleteRequest(
      std::move(v1_CompleteRequest));
  v2_CompleteRequest = v3_CompleteRequest;
  v3_CompleteRequest = std::move(v2_CompleteRequest);
  cdd_cpp::mcp::CompleteRequest v4_CompleteRequest = v3_CompleteRequest;
  (void)v4_CompleteRequest;
  cdd_cpp::mcp::CompleteResultCompletion v1_CompleteResultCompletion;
  cdd_cpp::mcp::CompleteResultCompletion v2_CompleteResultCompletion(
      v1_CompleteResultCompletion);
  cdd_cpp::mcp::CompleteResultCompletion v3_CompleteResultCompletion(
      std::move(v1_CompleteResultCompletion));
  v2_CompleteResultCompletion = v3_CompleteResultCompletion;
  v3_CompleteResultCompletion = std::move(v2_CompleteResultCompletion);
  cdd_cpp::mcp::CompleteResultCompletion v4_CompleteResultCompletion =
      v3_CompleteResultCompletion;
  (void)v4_CompleteResultCompletion;
  cdd_cpp::mcp::CompleteResult v1_CompleteResult;
  cdd_cpp::mcp::CompleteResult v2_CompleteResult(v1_CompleteResult);
  cdd_cpp::mcp::CompleteResult v3_CompleteResult(std::move(v1_CompleteResult));
  v2_CompleteResult = v3_CompleteResult;
  v3_CompleteResult = std::move(v2_CompleteResult);
  cdd_cpp::mcp::CompleteResult v4_CompleteResult = v3_CompleteResult;
  (void)v4_CompleteResult;
  cdd_cpp::mcp::ModelHint v1_ModelHint;
  cdd_cpp::mcp::ModelHint v2_ModelHint(v1_ModelHint);
  cdd_cpp::mcp::ModelHint v3_ModelHint(std::move(v1_ModelHint));
  v2_ModelHint = v3_ModelHint;
  v3_ModelHint = std::move(v2_ModelHint);
  cdd_cpp::mcp::ModelHint v4_ModelHint = v3_ModelHint;
  (void)v4_ModelHint;
  cdd_cpp::mcp::ModelPreferences v1_ModelPreferences;
  cdd_cpp::mcp::ModelPreferences v2_ModelPreferences(v1_ModelPreferences);
  cdd_cpp::mcp::ModelPreferences v3_ModelPreferences(
      std::move(v1_ModelPreferences));
  v2_ModelPreferences = v3_ModelPreferences;
  v3_ModelPreferences = std::move(v2_ModelPreferences);
  cdd_cpp::mcp::ModelPreferences v4_ModelPreferences = v3_ModelPreferences;
  (void)v4_ModelPreferences;
  cdd_cpp::mcp::CreateMessageRequestParams v1_CreateMessageRequestParams;
  cdd_cpp::mcp::CreateMessageRequestParams v2_CreateMessageRequestParams(
      v1_CreateMessageRequestParams);
  cdd_cpp::mcp::CreateMessageRequestParams v3_CreateMessageRequestParams(
      std::move(v1_CreateMessageRequestParams));
  v2_CreateMessageRequestParams = v3_CreateMessageRequestParams;
  v3_CreateMessageRequestParams = std::move(v2_CreateMessageRequestParams);
  cdd_cpp::mcp::CreateMessageRequestParams v4_CreateMessageRequestParams =
      v3_CreateMessageRequestParams;
  (void)v4_CreateMessageRequestParams;
  cdd_cpp::mcp::CreateMessageRequest v1_CreateMessageRequest;
  cdd_cpp::mcp::CreateMessageRequest v2_CreateMessageRequest(
      v1_CreateMessageRequest);
  cdd_cpp::mcp::CreateMessageRequest v3_CreateMessageRequest(
      std::move(v1_CreateMessageRequest));
  v2_CreateMessageRequest = v3_CreateMessageRequest;
  v3_CreateMessageRequest = std::move(v2_CreateMessageRequest);
  cdd_cpp::mcp::CreateMessageRequest v4_CreateMessageRequest =
      v3_CreateMessageRequest;
  (void)v4_CreateMessageRequest;
  cdd_cpp::mcp::CreateMessageResult v1_CreateMessageResult;
  cdd_cpp::mcp::CreateMessageResult v2_CreateMessageResult(
      v1_CreateMessageResult);
  cdd_cpp::mcp::CreateMessageResult v3_CreateMessageResult(
      std::move(v1_CreateMessageResult));
  v2_CreateMessageResult = v3_CreateMessageResult;
  v3_CreateMessageResult = std::move(v2_CreateMessageResult);
  cdd_cpp::mcp::CreateMessageResult v4_CreateMessageResult =
      v3_CreateMessageResult;
  (void)v4_CreateMessageResult;
  cdd_cpp::mcp::ToolInputSchema v1_ToolInputSchema;
  cdd_cpp::mcp::ToolInputSchema v2_ToolInputSchema(v1_ToolInputSchema);
  cdd_cpp::mcp::ToolInputSchema v3_ToolInputSchema(
      std::move(v1_ToolInputSchema));
  v2_ToolInputSchema = v3_ToolInputSchema;
  v3_ToolInputSchema = std::move(v2_ToolInputSchema);
  cdd_cpp::mcp::ToolInputSchema v4_ToolInputSchema = v3_ToolInputSchema;
  (void)v4_ToolInputSchema;
  cdd_cpp::mcp::Tool v1_Tool;
  cdd_cpp::mcp::Tool v2_Tool(v1_Tool);
  cdd_cpp::mcp::Tool v3_Tool(std::move(v1_Tool));
  v2_Tool = v3_Tool;
  v3_Tool = std::move(v2_Tool);
  cdd_cpp::mcp::Tool v4_Tool = v3_Tool;
  (void)v4_Tool;
  cdd_cpp::mcp::ToolListChangedNotificationParams
      v1_ToolListChangedNotificationParams;
  cdd_cpp::mcp::ToolListChangedNotificationParams
      v2_ToolListChangedNotificationParams(
          v1_ToolListChangedNotificationParams);
  cdd_cpp::mcp::ToolListChangedNotificationParams
      v3_ToolListChangedNotificationParams(
          std::move(v1_ToolListChangedNotificationParams));
  v2_ToolListChangedNotificationParams = v3_ToolListChangedNotificationParams;
  v3_ToolListChangedNotificationParams =
      std::move(v2_ToolListChangedNotificationParams);
  cdd_cpp::mcp::ToolListChangedNotificationParams
      v4_ToolListChangedNotificationParams =
          v3_ToolListChangedNotificationParams;
  (void)v4_ToolListChangedNotificationParams;
  cdd_cpp::mcp::ToolListChangedNotification v1_ToolListChangedNotification;
  cdd_cpp::mcp::ToolListChangedNotification v2_ToolListChangedNotification(
      v1_ToolListChangedNotification);
  cdd_cpp::mcp::ToolListChangedNotification v3_ToolListChangedNotification(
      std::move(v1_ToolListChangedNotification));
  v2_ToolListChangedNotification = v3_ToolListChangedNotification;
  v3_ToolListChangedNotification = std::move(v2_ToolListChangedNotification);
  cdd_cpp::mcp::ToolListChangedNotification v4_ToolListChangedNotification =
      v3_ToolListChangedNotification;
  (void)v4_ToolListChangedNotification;
  cdd_cpp::mcp::PaginatedRequestParams v1_PaginatedRequestParams;
  cdd_cpp::mcp::PaginatedRequestParams v2_PaginatedRequestParams(
      v1_PaginatedRequestParams);
  cdd_cpp::mcp::PaginatedRequestParams v3_PaginatedRequestParams(
      std::move(v1_PaginatedRequestParams));
  v2_PaginatedRequestParams = v3_PaginatedRequestParams;
  v3_PaginatedRequestParams = std::move(v2_PaginatedRequestParams);
  cdd_cpp::mcp::PaginatedRequestParams v4_PaginatedRequestParams =
      v3_PaginatedRequestParams;
  (void)v4_PaginatedRequestParams;
  cdd_cpp::mcp::PaginatedRequest v1_PaginatedRequest;
  cdd_cpp::mcp::PaginatedRequest v2_PaginatedRequest(v1_PaginatedRequest);
  cdd_cpp::mcp::PaginatedRequest v3_PaginatedRequest(
      std::move(v1_PaginatedRequest));
  v2_PaginatedRequest = v3_PaginatedRequest;
  v3_PaginatedRequest = std::move(v2_PaginatedRequest);
  cdd_cpp::mcp::PaginatedRequest v4_PaginatedRequest = v3_PaginatedRequest;
  (void)v4_PaginatedRequest;
  cdd_cpp::mcp::PaginatedResult v1_PaginatedResult;
  cdd_cpp::mcp::PaginatedResult v2_PaginatedResult(v1_PaginatedResult);
  cdd_cpp::mcp::PaginatedResult v3_PaginatedResult(
      std::move(v1_PaginatedResult));
  v2_PaginatedResult = v3_PaginatedResult;
  v3_PaginatedResult = std::move(v2_PaginatedResult);
  cdd_cpp::mcp::PaginatedResult v4_PaginatedResult = v3_PaginatedResult;
  (void)v4_PaginatedResult;
  cdd_cpp::mcp::Implementation v1_Implementation;
  cdd_cpp::mcp::Implementation v2_Implementation(v1_Implementation);
  cdd_cpp::mcp::Implementation v3_Implementation(std::move(v1_Implementation));
  v2_Implementation = v3_Implementation;
  v3_Implementation = std::move(v2_Implementation);
  cdd_cpp::mcp::Implementation v4_Implementation = v3_Implementation;
  (void)v4_Implementation;
  cdd_cpp::mcp::InitializeRequestParams v1_InitializeRequestParams;
  cdd_cpp::mcp::InitializeRequestParams v2_InitializeRequestParams(
      v1_InitializeRequestParams);
  cdd_cpp::mcp::InitializeRequestParams v3_InitializeRequestParams(
      std::move(v1_InitializeRequestParams));
  v2_InitializeRequestParams = v3_InitializeRequestParams;
  v3_InitializeRequestParams = std::move(v2_InitializeRequestParams);
  cdd_cpp::mcp::InitializeRequestParams v4_InitializeRequestParams =
      v3_InitializeRequestParams;
  (void)v4_InitializeRequestParams;
  cdd_cpp::mcp::InitializeRequest v1_InitializeRequest;
  cdd_cpp::mcp::InitializeRequest v2_InitializeRequest(v1_InitializeRequest);
  cdd_cpp::mcp::InitializeRequest v3_InitializeRequest(
      std::move(v1_InitializeRequest));
  v2_InitializeRequest = v3_InitializeRequest;
  v3_InitializeRequest = std::move(v2_InitializeRequest);
  cdd_cpp::mcp::InitializeRequest v4_InitializeRequest = v3_InitializeRequest;
  (void)v4_InitializeRequest;
  cdd_cpp::mcp::ServerCapabilitiesLogging v1_ServerCapabilitiesLogging;
  cdd_cpp::mcp::ServerCapabilitiesLogging v2_ServerCapabilitiesLogging(
      v1_ServerCapabilitiesLogging);
  cdd_cpp::mcp::ServerCapabilitiesLogging v3_ServerCapabilitiesLogging(
      std::move(v1_ServerCapabilitiesLogging));
  v2_ServerCapabilitiesLogging = v3_ServerCapabilitiesLogging;
  v3_ServerCapabilitiesLogging = std::move(v2_ServerCapabilitiesLogging);
  cdd_cpp::mcp::ServerCapabilitiesLogging v4_ServerCapabilitiesLogging =
      v3_ServerCapabilitiesLogging;
  (void)v4_ServerCapabilitiesLogging;
  cdd_cpp::mcp::ServerCapabilitiesPrompts v1_ServerCapabilitiesPrompts;
  cdd_cpp::mcp::ServerCapabilitiesPrompts v2_ServerCapabilitiesPrompts(
      v1_ServerCapabilitiesPrompts);
  cdd_cpp::mcp::ServerCapabilitiesPrompts v3_ServerCapabilitiesPrompts(
      std::move(v1_ServerCapabilitiesPrompts));
  v2_ServerCapabilitiesPrompts = v3_ServerCapabilitiesPrompts;
  v3_ServerCapabilitiesPrompts = std::move(v2_ServerCapabilitiesPrompts);
  cdd_cpp::mcp::ServerCapabilitiesPrompts v4_ServerCapabilitiesPrompts =
      v3_ServerCapabilitiesPrompts;
  (void)v4_ServerCapabilitiesPrompts;
  cdd_cpp::mcp::ServerCapabilitiesResources v1_ServerCapabilitiesResources;
  cdd_cpp::mcp::ServerCapabilitiesResources v2_ServerCapabilitiesResources(
      v1_ServerCapabilitiesResources);
  cdd_cpp::mcp::ServerCapabilitiesResources v3_ServerCapabilitiesResources(
      std::move(v1_ServerCapabilitiesResources));
  v2_ServerCapabilitiesResources = v3_ServerCapabilitiesResources;
  v3_ServerCapabilitiesResources = std::move(v2_ServerCapabilitiesResources);
  cdd_cpp::mcp::ServerCapabilitiesResources v4_ServerCapabilitiesResources =
      v3_ServerCapabilitiesResources;
  (void)v4_ServerCapabilitiesResources;
  cdd_cpp::mcp::ServerCapabilitiesTools v1_ServerCapabilitiesTools;
  cdd_cpp::mcp::ServerCapabilitiesTools v2_ServerCapabilitiesTools(
      v1_ServerCapabilitiesTools);
  cdd_cpp::mcp::ServerCapabilitiesTools v3_ServerCapabilitiesTools(
      std::move(v1_ServerCapabilitiesTools));
  v2_ServerCapabilitiesTools = v3_ServerCapabilitiesTools;
  v3_ServerCapabilitiesTools = std::move(v2_ServerCapabilitiesTools);
  cdd_cpp::mcp::ServerCapabilitiesTools v4_ServerCapabilitiesTools =
      v3_ServerCapabilitiesTools;
  (void)v4_ServerCapabilitiesTools;
  cdd_cpp::mcp::ServerCapabilities v1_ServerCapabilities;
  cdd_cpp::mcp::ServerCapabilities v2_ServerCapabilities(v1_ServerCapabilities);
  cdd_cpp::mcp::ServerCapabilities v3_ServerCapabilities(
      std::move(v1_ServerCapabilities));
  v2_ServerCapabilities = v3_ServerCapabilities;
  v3_ServerCapabilities = std::move(v2_ServerCapabilities);
  cdd_cpp::mcp::ServerCapabilities v4_ServerCapabilities =
      v3_ServerCapabilities;
  (void)v4_ServerCapabilities;
  cdd_cpp::mcp::InitializeResult v1_InitializeResult;
  cdd_cpp::mcp::InitializeResult v2_InitializeResult(v1_InitializeResult);
  cdd_cpp::mcp::InitializeResult v3_InitializeResult(
      std::move(v1_InitializeResult));
  v2_InitializeResult = v3_InitializeResult;
  v3_InitializeResult = std::move(v2_InitializeResult);
  cdd_cpp::mcp::InitializeResult v4_InitializeResult = v3_InitializeResult;
  (void)v4_InitializeResult;
  cdd_cpp::mcp::InitializedNotificationParams v1_InitializedNotificationParams;
  cdd_cpp::mcp::InitializedNotificationParams v2_InitializedNotificationParams(
      v1_InitializedNotificationParams);
  cdd_cpp::mcp::InitializedNotificationParams v3_InitializedNotificationParams(
      std::move(v1_InitializedNotificationParams));
  v2_InitializedNotificationParams = v3_InitializedNotificationParams;
  v3_InitializedNotificationParams =
      std::move(v2_InitializedNotificationParams);
  cdd_cpp::mcp::InitializedNotificationParams v4_InitializedNotificationParams =
      v3_InitializedNotificationParams;
  (void)v4_InitializedNotificationParams;
  cdd_cpp::mcp::InitializedNotification v1_InitializedNotification;
  cdd_cpp::mcp::InitializedNotification v2_InitializedNotification(
      v1_InitializedNotification);
  cdd_cpp::mcp::InitializedNotification v3_InitializedNotification(
      std::move(v1_InitializedNotification));
  v2_InitializedNotification = v3_InitializedNotification;
  v3_InitializedNotification = std::move(v2_InitializedNotification);
  cdd_cpp::mcp::InitializedNotification v4_InitializedNotification =
      v3_InitializedNotification;
  (void)v4_InitializedNotification;
  cdd_cpp::mcp::JSONRPCErrorError v1_JSONRPCErrorError;
  cdd_cpp::mcp::JSONRPCErrorError v2_JSONRPCErrorError(v1_JSONRPCErrorError);
  cdd_cpp::mcp::JSONRPCErrorError v3_JSONRPCErrorError(
      std::move(v1_JSONRPCErrorError));
  v2_JSONRPCErrorError = v3_JSONRPCErrorError;
  v3_JSONRPCErrorError = std::move(v2_JSONRPCErrorError);
  cdd_cpp::mcp::JSONRPCErrorError v4_JSONRPCErrorError = v3_JSONRPCErrorError;
  (void)v4_JSONRPCErrorError;
  cdd_cpp::mcp::JSONRPCError v1_JSONRPCError;
  cdd_cpp::mcp::JSONRPCError v2_JSONRPCError(v1_JSONRPCError);
  cdd_cpp::mcp::JSONRPCError v3_JSONRPCError(std::move(v1_JSONRPCError));
  v2_JSONRPCError = v3_JSONRPCError;
  v3_JSONRPCError = std::move(v2_JSONRPCError);
  cdd_cpp::mcp::JSONRPCError v4_JSONRPCError = v3_JSONRPCError;
  (void)v4_JSONRPCError;
  cdd_cpp::mcp::JSONRPCNotificationParams v1_JSONRPCNotificationParams;
  cdd_cpp::mcp::JSONRPCNotificationParams v2_JSONRPCNotificationParams(
      v1_JSONRPCNotificationParams);
  cdd_cpp::mcp::JSONRPCNotificationParams v3_JSONRPCNotificationParams(
      std::move(v1_JSONRPCNotificationParams));
  v2_JSONRPCNotificationParams = v3_JSONRPCNotificationParams;
  v3_JSONRPCNotificationParams = std::move(v2_JSONRPCNotificationParams);
  cdd_cpp::mcp::JSONRPCNotificationParams v4_JSONRPCNotificationParams =
      v3_JSONRPCNotificationParams;
  (void)v4_JSONRPCNotificationParams;
  cdd_cpp::mcp::JSONRPCNotification v1_JSONRPCNotification;
  cdd_cpp::mcp::JSONRPCNotification v2_JSONRPCNotification(
      v1_JSONRPCNotification);
  cdd_cpp::mcp::JSONRPCNotification v3_JSONRPCNotification(
      std::move(v1_JSONRPCNotification));
  v2_JSONRPCNotification = v3_JSONRPCNotification;
  v3_JSONRPCNotification = std::move(v2_JSONRPCNotification);
  cdd_cpp::mcp::JSONRPCNotification v4_JSONRPCNotification =
      v3_JSONRPCNotification;
  (void)v4_JSONRPCNotification;
  cdd_cpp::mcp::JSONRPCRequestParams v1_JSONRPCRequestParams;
  cdd_cpp::mcp::JSONRPCRequestParams v2_JSONRPCRequestParams(
      v1_JSONRPCRequestParams);
  cdd_cpp::mcp::JSONRPCRequestParams v3_JSONRPCRequestParams(
      std::move(v1_JSONRPCRequestParams));
  v2_JSONRPCRequestParams = v3_JSONRPCRequestParams;
  v3_JSONRPCRequestParams = std::move(v2_JSONRPCRequestParams);
  cdd_cpp::mcp::JSONRPCRequestParams v4_JSONRPCRequestParams =
      v3_JSONRPCRequestParams;
  (void)v4_JSONRPCRequestParams;
  cdd_cpp::mcp::JSONRPCRequest v1_JSONRPCRequest;
  cdd_cpp::mcp::JSONRPCRequest v2_JSONRPCRequest(v1_JSONRPCRequest);
  cdd_cpp::mcp::JSONRPCRequest v3_JSONRPCRequest(std::move(v1_JSONRPCRequest));
  v2_JSONRPCRequest = v3_JSONRPCRequest;
  v3_JSONRPCRequest = std::move(v2_JSONRPCRequest);
  cdd_cpp::mcp::JSONRPCRequest v4_JSONRPCRequest = v3_JSONRPCRequest;
  (void)v4_JSONRPCRequest;
  cdd_cpp::mcp::JSONRPCResponse v1_JSONRPCResponse;
  cdd_cpp::mcp::JSONRPCResponse v2_JSONRPCResponse(v1_JSONRPCResponse);
  cdd_cpp::mcp::JSONRPCResponse v3_JSONRPCResponse(
      std::move(v1_JSONRPCResponse));
  v2_JSONRPCResponse = v3_JSONRPCResponse;
  v3_JSONRPCResponse = std::move(v2_JSONRPCResponse);
  cdd_cpp::mcp::JSONRPCResponse v4_JSONRPCResponse = v3_JSONRPCResponse;
  (void)v4_JSONRPCResponse;
  cdd_cpp::mcp::ListPromptsRequestParams v1_ListPromptsRequestParams;
  cdd_cpp::mcp::ListPromptsRequestParams v2_ListPromptsRequestParams(
      v1_ListPromptsRequestParams);
  cdd_cpp::mcp::ListPromptsRequestParams v3_ListPromptsRequestParams(
      std::move(v1_ListPromptsRequestParams));
  v2_ListPromptsRequestParams = v3_ListPromptsRequestParams;
  v3_ListPromptsRequestParams = std::move(v2_ListPromptsRequestParams);
  cdd_cpp::mcp::ListPromptsRequestParams v4_ListPromptsRequestParams =
      v3_ListPromptsRequestParams;
  (void)v4_ListPromptsRequestParams;
  cdd_cpp::mcp::ListPromptsRequest v1_ListPromptsRequest;
  cdd_cpp::mcp::ListPromptsRequest v2_ListPromptsRequest(v1_ListPromptsRequest);
  cdd_cpp::mcp::ListPromptsRequest v3_ListPromptsRequest(
      std::move(v1_ListPromptsRequest));
  v2_ListPromptsRequest = v3_ListPromptsRequest;
  v3_ListPromptsRequest = std::move(v2_ListPromptsRequest);
  cdd_cpp::mcp::ListPromptsRequest v4_ListPromptsRequest =
      v3_ListPromptsRequest;
  (void)v4_ListPromptsRequest;
  cdd_cpp::mcp::PromptArgument v1_PromptArgument;
  cdd_cpp::mcp::PromptArgument v2_PromptArgument(v1_PromptArgument);
  cdd_cpp::mcp::PromptArgument v3_PromptArgument(std::move(v1_PromptArgument));
  v2_PromptArgument = v3_PromptArgument;
  v3_PromptArgument = std::move(v2_PromptArgument);
  cdd_cpp::mcp::PromptArgument v4_PromptArgument = v3_PromptArgument;
  (void)v4_PromptArgument;
  cdd_cpp::mcp::Prompt v1_Prompt;
  cdd_cpp::mcp::Prompt v2_Prompt(v1_Prompt);
  cdd_cpp::mcp::Prompt v3_Prompt(std::move(v1_Prompt));
  v2_Prompt = v3_Prompt;
  v3_Prompt = std::move(v2_Prompt);
  cdd_cpp::mcp::Prompt v4_Prompt = v3_Prompt;
  (void)v4_Prompt;
  cdd_cpp::mcp::ListPromptsResult v1_ListPromptsResult;
  cdd_cpp::mcp::ListPromptsResult v2_ListPromptsResult(v1_ListPromptsResult);
  cdd_cpp::mcp::ListPromptsResult v3_ListPromptsResult(
      std::move(v1_ListPromptsResult));
  v2_ListPromptsResult = v3_ListPromptsResult;
  v3_ListPromptsResult = std::move(v2_ListPromptsResult);
  cdd_cpp::mcp::ListPromptsResult v4_ListPromptsResult = v3_ListPromptsResult;
  (void)v4_ListPromptsResult;
  cdd_cpp::mcp::GetPromptRequestParams v1_GetPromptRequestParams;
  cdd_cpp::mcp::GetPromptRequestParams v2_GetPromptRequestParams(
      v1_GetPromptRequestParams);
  cdd_cpp::mcp::GetPromptRequestParams v3_GetPromptRequestParams(
      std::move(v1_GetPromptRequestParams));
  v2_GetPromptRequestParams = v3_GetPromptRequestParams;
  v3_GetPromptRequestParams = std::move(v2_GetPromptRequestParams);
  cdd_cpp::mcp::GetPromptRequestParams v4_GetPromptRequestParams =
      v3_GetPromptRequestParams;
  (void)v4_GetPromptRequestParams;
  cdd_cpp::mcp::GetPromptRequest v1_GetPromptRequest;
  cdd_cpp::mcp::GetPromptRequest v2_GetPromptRequest(v1_GetPromptRequest);
  cdd_cpp::mcp::GetPromptRequest v3_GetPromptRequest(
      std::move(v1_GetPromptRequest));
  v2_GetPromptRequest = v3_GetPromptRequest;
  v3_GetPromptRequest = std::move(v2_GetPromptRequest);
  cdd_cpp::mcp::GetPromptRequest v4_GetPromptRequest = v3_GetPromptRequest;
  (void)v4_GetPromptRequest;
  cdd_cpp::mcp::PromptMessage v1_PromptMessage;
  cdd_cpp::mcp::PromptMessage v2_PromptMessage(v1_PromptMessage);
  cdd_cpp::mcp::PromptMessage v3_PromptMessage(std::move(v1_PromptMessage));
  v2_PromptMessage = v3_PromptMessage;
  v3_PromptMessage = std::move(v2_PromptMessage);
  cdd_cpp::mcp::PromptMessage v4_PromptMessage = v3_PromptMessage;
  (void)v4_PromptMessage;
  cdd_cpp::mcp::GetPromptResult v1_GetPromptResult;
  cdd_cpp::mcp::GetPromptResult v2_GetPromptResult(v1_GetPromptResult);
  cdd_cpp::mcp::GetPromptResult v3_GetPromptResult(
      std::move(v1_GetPromptResult));
  v2_GetPromptResult = v3_GetPromptResult;
  v3_GetPromptResult = std::move(v2_GetPromptResult);
  cdd_cpp::mcp::GetPromptResult v4_GetPromptResult = v3_GetPromptResult;
  (void)v4_GetPromptResult;
  cdd_cpp::mcp::PromptListChangedNotificationParams
      v1_PromptListChangedNotificationParams;
  cdd_cpp::mcp::PromptListChangedNotificationParams
      v2_PromptListChangedNotificationParams(
          v1_PromptListChangedNotificationParams);
  cdd_cpp::mcp::PromptListChangedNotificationParams
      v3_PromptListChangedNotificationParams(
          std::move(v1_PromptListChangedNotificationParams));
  v2_PromptListChangedNotificationParams =
      v3_PromptListChangedNotificationParams;
  v3_PromptListChangedNotificationParams =
      std::move(v2_PromptListChangedNotificationParams);
  cdd_cpp::mcp::PromptListChangedNotificationParams
      v4_PromptListChangedNotificationParams =
          v3_PromptListChangedNotificationParams;
  (void)v4_PromptListChangedNotificationParams;
  cdd_cpp::mcp::PromptListChangedNotification v1_PromptListChangedNotification;
  cdd_cpp::mcp::PromptListChangedNotification v2_PromptListChangedNotification(
      v1_PromptListChangedNotification);
  cdd_cpp::mcp::PromptListChangedNotification v3_PromptListChangedNotification(
      std::move(v1_PromptListChangedNotification));
  v2_PromptListChangedNotification = v3_PromptListChangedNotification;
  v3_PromptListChangedNotification =
      std::move(v2_PromptListChangedNotification);
  cdd_cpp::mcp::PromptListChangedNotification v4_PromptListChangedNotification =
      v3_PromptListChangedNotification;
  (void)v4_PromptListChangedNotification;
  cdd_cpp::mcp::PromptReference v1_PromptReference;
  cdd_cpp::mcp::PromptReference v2_PromptReference(v1_PromptReference);
  cdd_cpp::mcp::PromptReference v3_PromptReference(
      std::move(v1_PromptReference));
  v2_PromptReference = v3_PromptReference;
  v3_PromptReference = std::move(v2_PromptReference);
  cdd_cpp::mcp::PromptReference v4_PromptReference = v3_PromptReference;
  (void)v4_PromptReference;
  cdd_cpp::mcp::ListResourcesRequestParams v1_ListResourcesRequestParams;
  cdd_cpp::mcp::ListResourcesRequestParams v2_ListResourcesRequestParams(
      v1_ListResourcesRequestParams);
  cdd_cpp::mcp::ListResourcesRequestParams v3_ListResourcesRequestParams(
      std::move(v1_ListResourcesRequestParams));
  v2_ListResourcesRequestParams = v3_ListResourcesRequestParams;
  v3_ListResourcesRequestParams = std::move(v2_ListResourcesRequestParams);
  cdd_cpp::mcp::ListResourcesRequestParams v4_ListResourcesRequestParams =
      v3_ListResourcesRequestParams;
  (void)v4_ListResourcesRequestParams;
  cdd_cpp::mcp::ListResourcesRequest v1_ListResourcesRequest;
  cdd_cpp::mcp::ListResourcesRequest v2_ListResourcesRequest(
      v1_ListResourcesRequest);
  cdd_cpp::mcp::ListResourcesRequest v3_ListResourcesRequest(
      std::move(v1_ListResourcesRequest));
  v2_ListResourcesRequest = v3_ListResourcesRequest;
  v3_ListResourcesRequest = std::move(v2_ListResourcesRequest);
  cdd_cpp::mcp::ListResourcesRequest v4_ListResourcesRequest =
      v3_ListResourcesRequest;
  (void)v4_ListResourcesRequest;
  cdd_cpp::mcp::Resource v1_Resource;
  cdd_cpp::mcp::Resource v2_Resource(v1_Resource);
  cdd_cpp::mcp::Resource v3_Resource(std::move(v1_Resource));
  v2_Resource = v3_Resource;
  v3_Resource = std::move(v2_Resource);
  cdd_cpp::mcp::Resource v4_Resource = v3_Resource;
  (void)v4_Resource;
  cdd_cpp::mcp::ListResourcesResult v1_ListResourcesResult;
  cdd_cpp::mcp::ListResourcesResult v2_ListResourcesResult(
      v1_ListResourcesResult);
  cdd_cpp::mcp::ListResourcesResult v3_ListResourcesResult(
      std::move(v1_ListResourcesResult));
  v2_ListResourcesResult = v3_ListResourcesResult;
  v3_ListResourcesResult = std::move(v2_ListResourcesResult);
  cdd_cpp::mcp::ListResourcesResult v4_ListResourcesResult =
      v3_ListResourcesResult;
  (void)v4_ListResourcesResult;
  cdd_cpp::mcp::ListResourceTemplatesRequestParams
      v1_ListResourceTemplatesRequestParams;
  cdd_cpp::mcp::ListResourceTemplatesRequestParams
      v2_ListResourceTemplatesRequestParams(
          v1_ListResourceTemplatesRequestParams);
  cdd_cpp::mcp::ListResourceTemplatesRequestParams
      v3_ListResourceTemplatesRequestParams(
          std::move(v1_ListResourceTemplatesRequestParams));
  v2_ListResourceTemplatesRequestParams = v3_ListResourceTemplatesRequestParams;
  v3_ListResourceTemplatesRequestParams =
      std::move(v2_ListResourceTemplatesRequestParams);
  cdd_cpp::mcp::ListResourceTemplatesRequestParams
      v4_ListResourceTemplatesRequestParams =
          v3_ListResourceTemplatesRequestParams;
  (void)v4_ListResourceTemplatesRequestParams;
  cdd_cpp::mcp::ListResourceTemplatesRequest v1_ListResourceTemplatesRequest;
  cdd_cpp::mcp::ListResourceTemplatesRequest v2_ListResourceTemplatesRequest(
      v1_ListResourceTemplatesRequest);
  cdd_cpp::mcp::ListResourceTemplatesRequest v3_ListResourceTemplatesRequest(
      std::move(v1_ListResourceTemplatesRequest));
  v2_ListResourceTemplatesRequest = v3_ListResourceTemplatesRequest;
  v3_ListResourceTemplatesRequest = std::move(v2_ListResourceTemplatesRequest);
  cdd_cpp::mcp::ListResourceTemplatesRequest v4_ListResourceTemplatesRequest =
      v3_ListResourceTemplatesRequest;
  (void)v4_ListResourceTemplatesRequest;
  cdd_cpp::mcp::ResourceTemplate v1_ResourceTemplate;
  cdd_cpp::mcp::ResourceTemplate v2_ResourceTemplate(v1_ResourceTemplate);
  cdd_cpp::mcp::ResourceTemplate v3_ResourceTemplate(
      std::move(v1_ResourceTemplate));
  v2_ResourceTemplate = v3_ResourceTemplate;
  v3_ResourceTemplate = std::move(v2_ResourceTemplate);
  cdd_cpp::mcp::ResourceTemplate v4_ResourceTemplate = v3_ResourceTemplate;
  (void)v4_ResourceTemplate;
  cdd_cpp::mcp::ListResourceTemplatesResult v1_ListResourceTemplatesResult;
  cdd_cpp::mcp::ListResourceTemplatesResult v2_ListResourceTemplatesResult(
      v1_ListResourceTemplatesResult);
  cdd_cpp::mcp::ListResourceTemplatesResult v3_ListResourceTemplatesResult(
      std::move(v1_ListResourceTemplatesResult));
  v2_ListResourceTemplatesResult = v3_ListResourceTemplatesResult;
  v3_ListResourceTemplatesResult = std::move(v2_ListResourceTemplatesResult);
  cdd_cpp::mcp::ListResourceTemplatesResult v4_ListResourceTemplatesResult =
      v3_ListResourceTemplatesResult;
  (void)v4_ListResourceTemplatesResult;
  cdd_cpp::mcp::ReadResourceRequestParams v1_ReadResourceRequestParams;
  cdd_cpp::mcp::ReadResourceRequestParams v2_ReadResourceRequestParams(
      v1_ReadResourceRequestParams);
  cdd_cpp::mcp::ReadResourceRequestParams v3_ReadResourceRequestParams(
      std::move(v1_ReadResourceRequestParams));
  v2_ReadResourceRequestParams = v3_ReadResourceRequestParams;
  v3_ReadResourceRequestParams = std::move(v2_ReadResourceRequestParams);
  cdd_cpp::mcp::ReadResourceRequestParams v4_ReadResourceRequestParams =
      v3_ReadResourceRequestParams;
  (void)v4_ReadResourceRequestParams;
  cdd_cpp::mcp::ReadResourceRequest v1_ReadResourceRequest;
  cdd_cpp::mcp::ReadResourceRequest v2_ReadResourceRequest(
      v1_ReadResourceRequest);
  cdd_cpp::mcp::ReadResourceRequest v3_ReadResourceRequest(
      std::move(v1_ReadResourceRequest));
  v2_ReadResourceRequest = v3_ReadResourceRequest;
  v3_ReadResourceRequest = std::move(v2_ReadResourceRequest);
  cdd_cpp::mcp::ReadResourceRequest v4_ReadResourceRequest =
      v3_ReadResourceRequest;
  (void)v4_ReadResourceRequest;
  cdd_cpp::mcp::ReadResourceResult v1_ReadResourceResult;
  cdd_cpp::mcp::ReadResourceResult v2_ReadResourceResult(v1_ReadResourceResult);
  cdd_cpp::mcp::ReadResourceResult v3_ReadResourceResult(
      std::move(v1_ReadResourceResult));
  v2_ReadResourceResult = v3_ReadResourceResult;
  v3_ReadResourceResult = std::move(v2_ReadResourceResult);
  cdd_cpp::mcp::ReadResourceResult v4_ReadResourceResult =
      v3_ReadResourceResult;
  (void)v4_ReadResourceResult;
  cdd_cpp::mcp::ResourceUpdatedNotificationParams
      v1_ResourceUpdatedNotificationParams;
  cdd_cpp::mcp::ResourceUpdatedNotificationParams
      v2_ResourceUpdatedNotificationParams(
          v1_ResourceUpdatedNotificationParams);
  cdd_cpp::mcp::ResourceUpdatedNotificationParams
      v3_ResourceUpdatedNotificationParams(
          std::move(v1_ResourceUpdatedNotificationParams));
  v2_ResourceUpdatedNotificationParams = v3_ResourceUpdatedNotificationParams;
  v3_ResourceUpdatedNotificationParams =
      std::move(v2_ResourceUpdatedNotificationParams);
  cdd_cpp::mcp::ResourceUpdatedNotificationParams
      v4_ResourceUpdatedNotificationParams =
          v3_ResourceUpdatedNotificationParams;
  (void)v4_ResourceUpdatedNotificationParams;
  cdd_cpp::mcp::ResourceUpdatedNotification v1_ResourceUpdatedNotification;
  cdd_cpp::mcp::ResourceUpdatedNotification v2_ResourceUpdatedNotification(
      v1_ResourceUpdatedNotification);
  cdd_cpp::mcp::ResourceUpdatedNotification v3_ResourceUpdatedNotification(
      std::move(v1_ResourceUpdatedNotification));
  v2_ResourceUpdatedNotification = v3_ResourceUpdatedNotification;
  v3_ResourceUpdatedNotification = std::move(v2_ResourceUpdatedNotification);
  cdd_cpp::mcp::ResourceUpdatedNotification v4_ResourceUpdatedNotification =
      v3_ResourceUpdatedNotification;
  (void)v4_ResourceUpdatedNotification;
  cdd_cpp::mcp::ResourceListChangedNotificationParams
      v1_ResourceListChangedNotificationParams;
  cdd_cpp::mcp::ResourceListChangedNotificationParams
      v2_ResourceListChangedNotificationParams(
          v1_ResourceListChangedNotificationParams);
  cdd_cpp::mcp::ResourceListChangedNotificationParams
      v3_ResourceListChangedNotificationParams(
          std::move(v1_ResourceListChangedNotificationParams));
  v2_ResourceListChangedNotificationParams =
      v3_ResourceListChangedNotificationParams;
  v3_ResourceListChangedNotificationParams =
      std::move(v2_ResourceListChangedNotificationParams);
  cdd_cpp::mcp::ResourceListChangedNotificationParams
      v4_ResourceListChangedNotificationParams =
          v3_ResourceListChangedNotificationParams;
  (void)v4_ResourceListChangedNotificationParams;
  cdd_cpp::mcp::ResourceListChangedNotification
      v1_ResourceListChangedNotification;
  cdd_cpp::mcp::ResourceListChangedNotification
      v2_ResourceListChangedNotification(v1_ResourceListChangedNotification);
  cdd_cpp::mcp::ResourceListChangedNotification
      v3_ResourceListChangedNotification(
          std::move(v1_ResourceListChangedNotification));
  v2_ResourceListChangedNotification = v3_ResourceListChangedNotification;
  v3_ResourceListChangedNotification =
      std::move(v2_ResourceListChangedNotification);
  cdd_cpp::mcp::ResourceListChangedNotification
      v4_ResourceListChangedNotification = v3_ResourceListChangedNotification;
  (void)v4_ResourceListChangedNotification;
  cdd_cpp::mcp::ResourceReference v1_ResourceReference;
  cdd_cpp::mcp::ResourceReference v2_ResourceReference(v1_ResourceReference);
  cdd_cpp::mcp::ResourceReference v3_ResourceReference(
      std::move(v1_ResourceReference));
  v2_ResourceReference = v3_ResourceReference;
  v3_ResourceReference = std::move(v2_ResourceReference);
  cdd_cpp::mcp::ResourceReference v4_ResourceReference = v3_ResourceReference;
  (void)v4_ResourceReference;
  cdd_cpp::mcp::Result v1_Result;
  cdd_cpp::mcp::Result v2_Result(v1_Result);
  cdd_cpp::mcp::Result v3_Result(std::move(v1_Result));
  v2_Result = v3_Result;
  v3_Result = std::move(v2_Result);
  cdd_cpp::mcp::Result v4_Result = v3_Result;
  (void)v4_Result;
  cdd_cpp::mcp::Root v1_Root;
  cdd_cpp::mcp::Root v2_Root(v1_Root);
  cdd_cpp::mcp::Root v3_Root(std::move(v1_Root));
  v2_Root = v3_Root;
  v3_Root = std::move(v2_Root);
  cdd_cpp::mcp::Root v4_Root = v3_Root;
  (void)v4_Root;
  cdd_cpp::mcp::RootsListChangedNotificationParams
      v1_RootsListChangedNotificationParams;
  cdd_cpp::mcp::RootsListChangedNotificationParams
      v2_RootsListChangedNotificationParams(
          v1_RootsListChangedNotificationParams);
  cdd_cpp::mcp::RootsListChangedNotificationParams
      v3_RootsListChangedNotificationParams(
          std::move(v1_RootsListChangedNotificationParams));
  v2_RootsListChangedNotificationParams = v3_RootsListChangedNotificationParams;
  v3_RootsListChangedNotificationParams =
      std::move(v2_RootsListChangedNotificationParams);
  cdd_cpp::mcp::RootsListChangedNotificationParams
      v4_RootsListChangedNotificationParams =
          v3_RootsListChangedNotificationParams;
  (void)v4_RootsListChangedNotificationParams;
  cdd_cpp::mcp::RootsListChangedNotification v1_RootsListChangedNotification;
  cdd_cpp::mcp::RootsListChangedNotification v2_RootsListChangedNotification(
      v1_RootsListChangedNotification);
  cdd_cpp::mcp::RootsListChangedNotification v3_RootsListChangedNotification(
      std::move(v1_RootsListChangedNotification));
  v2_RootsListChangedNotification = v3_RootsListChangedNotification;
  v3_RootsListChangedNotification = std::move(v2_RootsListChangedNotification);
  cdd_cpp::mcp::RootsListChangedNotification v4_RootsListChangedNotification =
      v3_RootsListChangedNotification;
  (void)v4_RootsListChangedNotification;
  cdd_cpp::mcp::ListRootsRequestParams v1_ListRootsRequestParams;
  cdd_cpp::mcp::ListRootsRequestParams v2_ListRootsRequestParams(
      v1_ListRootsRequestParams);
  cdd_cpp::mcp::ListRootsRequestParams v3_ListRootsRequestParams(
      std::move(v1_ListRootsRequestParams));
  v2_ListRootsRequestParams = v3_ListRootsRequestParams;
  v3_ListRootsRequestParams = std::move(v2_ListRootsRequestParams);
  cdd_cpp::mcp::ListRootsRequestParams v4_ListRootsRequestParams =
      v3_ListRootsRequestParams;
  (void)v4_ListRootsRequestParams;
  cdd_cpp::mcp::ListRootsRequest v1_ListRootsRequest;
  cdd_cpp::mcp::ListRootsRequest v2_ListRootsRequest(v1_ListRootsRequest);
  cdd_cpp::mcp::ListRootsRequest v3_ListRootsRequest(
      std::move(v1_ListRootsRequest));
  v2_ListRootsRequest = v3_ListRootsRequest;
  v3_ListRootsRequest = std::move(v2_ListRootsRequest);
  cdd_cpp::mcp::ListRootsRequest v4_ListRootsRequest = v3_ListRootsRequest;
  (void)v4_ListRootsRequest;
  cdd_cpp::mcp::ListRootsResult v1_ListRootsResult;
  cdd_cpp::mcp::ListRootsResult v2_ListRootsResult(v1_ListRootsResult);
  cdd_cpp::mcp::ListRootsResult v3_ListRootsResult(
      std::move(v1_ListRootsResult));
  v2_ListRootsResult = v3_ListRootsResult;
  v3_ListRootsResult = std::move(v2_ListRootsResult);
  cdd_cpp::mcp::ListRootsResult v4_ListRootsResult = v3_ListRootsResult;
  (void)v4_ListRootsResult;
  cdd_cpp::mcp::SetLevelRequestParams v1_SetLevelRequestParams;
  cdd_cpp::mcp::SetLevelRequestParams v2_SetLevelRequestParams(
      v1_SetLevelRequestParams);
  cdd_cpp::mcp::SetLevelRequestParams v3_SetLevelRequestParams(
      std::move(v1_SetLevelRequestParams));
  v2_SetLevelRequestParams = v3_SetLevelRequestParams;
  v3_SetLevelRequestParams = std::move(v2_SetLevelRequestParams);
  cdd_cpp::mcp::SetLevelRequestParams v4_SetLevelRequestParams =
      v3_SetLevelRequestParams;
  (void)v4_SetLevelRequestParams;
  cdd_cpp::mcp::SetLevelRequest v1_SetLevelRequest;
  cdd_cpp::mcp::SetLevelRequest v2_SetLevelRequest(v1_SetLevelRequest);
  cdd_cpp::mcp::SetLevelRequest v3_SetLevelRequest(
      std::move(v1_SetLevelRequest));
  v2_SetLevelRequest = v3_SetLevelRequest;
  v3_SetLevelRequest = std::move(v2_SetLevelRequest);
  cdd_cpp::mcp::SetLevelRequest v4_SetLevelRequest = v3_SetLevelRequest;
  (void)v4_SetLevelRequest;
  cdd_cpp::mcp::LoggingMessageNotificationParams
      v1_LoggingMessageNotificationParams;
  cdd_cpp::mcp::LoggingMessageNotificationParams
      v2_LoggingMessageNotificationParams(v1_LoggingMessageNotificationParams);
  cdd_cpp::mcp::LoggingMessageNotificationParams
      v3_LoggingMessageNotificationParams(
          std::move(v1_LoggingMessageNotificationParams));
  v2_LoggingMessageNotificationParams = v3_LoggingMessageNotificationParams;
  v3_LoggingMessageNotificationParams =
      std::move(v2_LoggingMessageNotificationParams);
  cdd_cpp::mcp::LoggingMessageNotificationParams
      v4_LoggingMessageNotificationParams = v3_LoggingMessageNotificationParams;
  (void)v4_LoggingMessageNotificationParams;
  cdd_cpp::mcp::LoggingMessageNotification v1_LoggingMessageNotification;
  cdd_cpp::mcp::LoggingMessageNotification v2_LoggingMessageNotification(
      v1_LoggingMessageNotification);
  cdd_cpp::mcp::LoggingMessageNotification v3_LoggingMessageNotification(
      std::move(v1_LoggingMessageNotification));
  v2_LoggingMessageNotification = v3_LoggingMessageNotification;
  v3_LoggingMessageNotification = std::move(v2_LoggingMessageNotification);
  cdd_cpp::mcp::LoggingMessageNotification v4_LoggingMessageNotification =
      v3_LoggingMessageNotification;
  (void)v4_LoggingMessageNotification;
  cdd_cpp::mcp::ProgressNotificationParams v1_ProgressNotificationParams;
  cdd_cpp::mcp::ProgressNotificationParams v2_ProgressNotificationParams(
      v1_ProgressNotificationParams);
  cdd_cpp::mcp::ProgressNotificationParams v3_ProgressNotificationParams(
      std::move(v1_ProgressNotificationParams));
  v2_ProgressNotificationParams = v3_ProgressNotificationParams;
  v3_ProgressNotificationParams = std::move(v2_ProgressNotificationParams);
  cdd_cpp::mcp::ProgressNotificationParams v4_ProgressNotificationParams =
      v3_ProgressNotificationParams;
  (void)v4_ProgressNotificationParams;
  cdd_cpp::mcp::ProgressNotification v1_ProgressNotification;
  cdd_cpp::mcp::ProgressNotification v2_ProgressNotification(
      v1_ProgressNotification);
  cdd_cpp::mcp::ProgressNotification v3_ProgressNotification(
      std::move(v1_ProgressNotification));
  v2_ProgressNotification = v3_ProgressNotification;
  v3_ProgressNotification = std::move(v2_ProgressNotification);
  cdd_cpp::mcp::ProgressNotification v4_ProgressNotification =
      v3_ProgressNotification;
  (void)v4_ProgressNotification;
  cdd_cpp::mcp::EmptyResult v1_EmptyResult;
  cdd_cpp::mcp::EmptyResult v2_EmptyResult(v1_EmptyResult);
  cdd_cpp::mcp::EmptyResult v3_EmptyResult(std::move(v1_EmptyResult));
  v2_EmptyResult = v3_EmptyResult;
  v3_EmptyResult = std::move(v2_EmptyResult);
  cdd_cpp::mcp::EmptyResult v4_EmptyResult = v3_EmptyResult;
  (void)v4_EmptyResult;
  cdd_cpp::mcp::SamplingMessage v1_SamplingMessage;
  cdd_cpp::mcp::SamplingMessage v2_SamplingMessage(v1_SamplingMessage);
  cdd_cpp::mcp::SamplingMessage v3_SamplingMessage(
      std::move(v1_SamplingMessage));
  v2_SamplingMessage = v3_SamplingMessage;
  v3_SamplingMessage = std::move(v2_SamplingMessage);
  cdd_cpp::mcp::SamplingMessage v4_SamplingMessage = v3_SamplingMessage;
  (void)v4_SamplingMessage;
  cdd_cpp::mcp::PingRequestParams v1_PingRequestParams;
  cdd_cpp::mcp::PingRequestParams v2_PingRequestParams(v1_PingRequestParams);
  cdd_cpp::mcp::PingRequestParams v3_PingRequestParams(
      std::move(v1_PingRequestParams));
  v2_PingRequestParams = v3_PingRequestParams;
  v3_PingRequestParams = std::move(v2_PingRequestParams);
  cdd_cpp::mcp::PingRequestParams v4_PingRequestParams = v3_PingRequestParams;
  (void)v4_PingRequestParams;
  cdd_cpp::mcp::PingRequest v1_PingRequest;
  cdd_cpp::mcp::PingRequest v2_PingRequest(v1_PingRequest);
  cdd_cpp::mcp::PingRequest v3_PingRequest(std::move(v1_PingRequest));
  v2_PingRequest = v3_PingRequest;
  v3_PingRequest = std::move(v2_PingRequest);
  cdd_cpp::mcp::PingRequest v4_PingRequest = v3_PingRequest;
  (void)v4_PingRequest;
  cdd_cpp::mcp::SubscribeRequestParams v1_SubscribeRequestParams;
  cdd_cpp::mcp::SubscribeRequestParams v2_SubscribeRequestParams(
      v1_SubscribeRequestParams);
  cdd_cpp::mcp::SubscribeRequestParams v3_SubscribeRequestParams(
      std::move(v1_SubscribeRequestParams));
  v2_SubscribeRequestParams = v3_SubscribeRequestParams;
  v3_SubscribeRequestParams = std::move(v2_SubscribeRequestParams);
  cdd_cpp::mcp::SubscribeRequestParams v4_SubscribeRequestParams =
      v3_SubscribeRequestParams;
  (void)v4_SubscribeRequestParams;
  cdd_cpp::mcp::SubscribeRequest v1_SubscribeRequest;
  cdd_cpp::mcp::SubscribeRequest v2_SubscribeRequest(v1_SubscribeRequest);
  cdd_cpp::mcp::SubscribeRequest v3_SubscribeRequest(
      std::move(v1_SubscribeRequest));
  v2_SubscribeRequest = v3_SubscribeRequest;
  v3_SubscribeRequest = std::move(v2_SubscribeRequest);
  cdd_cpp::mcp::SubscribeRequest v4_SubscribeRequest = v3_SubscribeRequest;
  (void)v4_SubscribeRequest;
  cdd_cpp::mcp::UnsubscribeRequestParams v1_UnsubscribeRequestParams;
  cdd_cpp::mcp::UnsubscribeRequestParams v2_UnsubscribeRequestParams(
      v1_UnsubscribeRequestParams);
  cdd_cpp::mcp::UnsubscribeRequestParams v3_UnsubscribeRequestParams(
      std::move(v1_UnsubscribeRequestParams));
  v2_UnsubscribeRequestParams = v3_UnsubscribeRequestParams;
  v3_UnsubscribeRequestParams = std::move(v2_UnsubscribeRequestParams);
  cdd_cpp::mcp::UnsubscribeRequestParams v4_UnsubscribeRequestParams =
      v3_UnsubscribeRequestParams;
  (void)v4_UnsubscribeRequestParams;
  cdd_cpp::mcp::UnsubscribeRequest v1_UnsubscribeRequest;
  cdd_cpp::mcp::UnsubscribeRequest v2_UnsubscribeRequest(v1_UnsubscribeRequest);
  cdd_cpp::mcp::UnsubscribeRequest v3_UnsubscribeRequest(
      std::move(v1_UnsubscribeRequest));
  v2_UnsubscribeRequest = v3_UnsubscribeRequest;
  v3_UnsubscribeRequest = std::move(v2_UnsubscribeRequest);
  cdd_cpp::mcp::UnsubscribeRequest v4_UnsubscribeRequest =
      v3_UnsubscribeRequest;
  (void)v4_UnsubscribeRequest;
  cdd_cpp::mcp::ListToolsRequestParams v1_ListToolsRequestParams;
  cdd_cpp::mcp::ListToolsRequestParams v2_ListToolsRequestParams(
      v1_ListToolsRequestParams);
  cdd_cpp::mcp::ListToolsRequestParams v3_ListToolsRequestParams(
      std::move(v1_ListToolsRequestParams));
  v2_ListToolsRequestParams = v3_ListToolsRequestParams;
  v3_ListToolsRequestParams = std::move(v2_ListToolsRequestParams);
  cdd_cpp::mcp::ListToolsRequestParams v4_ListToolsRequestParams =
      v3_ListToolsRequestParams;
  (void)v4_ListToolsRequestParams;
  cdd_cpp::mcp::ListToolsRequest v1_ListToolsRequest;
  cdd_cpp::mcp::ListToolsRequest v2_ListToolsRequest(v1_ListToolsRequest);
  cdd_cpp::mcp::ListToolsRequest v3_ListToolsRequest(
      std::move(v1_ListToolsRequest));
  v2_ListToolsRequest = v3_ListToolsRequest;
  v3_ListToolsRequest = std::move(v2_ListToolsRequest);
  cdd_cpp::mcp::ListToolsRequest v4_ListToolsRequest = v3_ListToolsRequest;
  (void)v4_ListToolsRequest;
  cdd_cpp::mcp::ListToolsResult v1_ListToolsResult;
  cdd_cpp::mcp::ListToolsResult v2_ListToolsResult(v1_ListToolsResult);
  cdd_cpp::mcp::ListToolsResult v3_ListToolsResult(
      std::move(v1_ListToolsResult));
  v2_ListToolsResult = v3_ListToolsResult;
  v3_ListToolsResult = std::move(v2_ListToolsResult);
  cdd_cpp::mcp::ListToolsResult v4_ListToolsResult = v3_ListToolsResult;
  (void)v4_ListToolsResult;
}
} // namespace cdd_cpp::struct_tests
