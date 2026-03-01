#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cdd_cpp::openapi {

/// @brief Type alias for a security requirement map.
using SecurityRequirement = std::map<std::string, std::vector<std::string>>;

struct Encoding;
struct PathItem;
struct Header;
struct Schema;

/// @brief A simple object to allow referencing other components in the specification.
struct Reference {
  std::string ref; ///< The reference string.
  std::optional<std::string> summary; ///< A short summary of what the reference refers to.
  std::optional<std::string> description; ///< A description of what the reference refers to.
};

/// @brief Contact information for the exposed API.
struct Contact {
  std::optional<std::string> name; ///< The identifying name of the contact person/organization.
  std::optional<std::string> url; ///< The URL pointing to the contact information.
  std::optional<std::string> email; ///< The email address of the contact person/organization.
};

/// @brief License information for the exposed API.
struct License {
  std::string name; ///< The license name used for the API.
  std::optional<std::string> identifier; ///< An SPDX license expression for the API.
  std::optional<std::string> url; ///< A URL to the license used for the API.
};

/// @brief Provides metadata about the API.
struct Info {
  std::string title; ///< The title of the API.
  std::optional<std::string> summary; ///< A short summary of the API.
  std::optional<std::string> description; ///< A description of the API.
  std::optional<std::string> termsOfService; ///< A URL to the Terms of Service for the API.
  std::optional<Contact> contact; ///< The contact information for the exposed API.
  std::optional<License> license; ///< The license information for the exposed API.
  std::string version; ///< The version of the OpenAPI document.
};

/// @brief An object representing a Server Variable for server URL template substitution.
struct ServerVariable {
  std::optional<std::vector<std::string>> enum_values; ///< An enumeration of string values to be used if the substitution options are from a limited set.
  std::string default_value; ///< The default value to use for substitution.
  std::optional<std::string> description; ///< An optional description for the server variable.
};

/// @brief An object representing a Server.
struct Server {
  std::string url; ///< A URL to the target host.
  std::optional<std::string> description; ///< An optional string describing the host designated by the URL.
  std::optional<std::string> name; ///< An optional name for the server.
  std::optional<std::map<std::string, ServerVariable>> variables; ///< A map between a variable name and its value.
};

/// @brief Allows referencing an external resource for extended documentation.
struct ExternalDocumentation {
  std::optional<std::string> description; ///< A description of the target documentation.
  std::string url; ///< The URL for the target documentation.
};

/// @brief Adds metadata to a single tag that is used by the Operation Object.
struct Tag {
  std::string name; ///< The name of the tag.
  std::optional<std::string> description; ///< A short description for the tag.
  std::optional<std::string> parent; ///< A hierarchical parent for the tag.
  std::optional<std::string> kind; ///< The kind of tag (e.g., entity, action, namespace).
  std::optional<ExternalDocumentation> externalDocs; ///< Additional external documentation for this tag.
};

/// @brief When request bodies or response payloads may be one of a number of different schemas, a discriminator object can be used to aid in serialization, deserialization, and validation.
struct Discriminator {
  std::string propertyName; ///< The name of the property in the payload that will hold the discriminator value.
  std::optional<std::map<std::string, std::string>> mapping; ///< An object to hold mappings between payload values and schema names or references.
  std::optional<std::string> defaultMapping; ///< A default schema value to use if the discriminator property does not match any mapping.
};

/// @brief A metadata object that allows for more fine-tuned XML model definitions.
struct XML {
  std::optional<std::string> name; ///< Replaces the name of the element/attribute used for the described schema property.
  std::optional<std::string> namespace_url; ///< The URI of the namespace definition.
  std::optional<std::string> prefix; ///< The prefix to be used for the name.
  std::optional<bool> attribute; ///< Declares whether the property definition translates to an attribute instead of an element.
  std::optional<bool> wrapped; ///< MAY be used only for an array definition. Signifies whether the array is wrapped.
};

/// @brief The Schema Object allows the definition of input and output data types. Draft 2020-12 compliant.
struct Schema {
  std::optional<std::string> type; ///< The type of the schema (e.g., string, integer, object, array).
  std::optional<std::string> description; ///< A description of the schema.
  std::optional<std::string> format; ///< The format of the schema.
  std::optional<Reference> ref; ///< A reference to another schema.
  std::optional<Discriminator> discriminator; ///< Adds support for polymorphism.
  std::optional<XML> xml; ///< This MAY be used only on properties schemas.

  std::shared_ptr<std::map<std::string, Schema>> properties; ///< Properties of an object.
  std::shared_ptr<Schema> items; ///< Items of an array.
  
  // JSON Schema Draft 2020-12 Additions
  std::optional<std::string> schema_dialect; ///< The $schema keyword.
  std::optional<std::string> id; ///< The $id keyword.
  std::optional<std::string> anchor; ///< The $anchor keyword.
  std::optional<std::string> dynamicAnchor; ///< The $dynamicAnchor keyword.
  std::optional<std::string> dynamicRef; ///< The $dynamicRef keyword.
  std::shared_ptr<std::map<std::string, Schema>> defs; ///< The $defs keyword.
  std::optional<std::string> comment; ///< The $comment keyword.
  std::optional<std::string> title; ///< The title of the schema.
  std::optional<std::string> default_value; ///< The default value.
  std::optional<bool> deprecated; ///< Whether the schema is deprecated.
  std::optional<bool> readOnly; ///< Whether the schema is readOnly.
  std::optional<bool> writeOnly; ///< Whether the schema is writeOnly.
  
  std::optional<double> multipleOf; ///< The multipleOf keyword.
  std::optional<double> maximum; ///< The maximum keyword.
  std::optional<double> exclusiveMaximum; ///< The exclusiveMaximum keyword.
  std::optional<double> minimum; ///< The minimum keyword.
  std::optional<double> exclusiveMinimum; ///< The exclusiveMinimum keyword.
  
  std::optional<int> maxLength; ///< The maxLength keyword.
  std::optional<int> minLength; ///< The minLength keyword.
  std::optional<std::string> pattern; ///< The pattern keyword.
  
  std::optional<int> maxItems; ///< The maxItems keyword.
  std::optional<int> minItems; ///< The minItems keyword.
  std::optional<bool> uniqueItems; ///< The uniqueItems keyword.
  
  std::optional<int> maxContains; ///< The maxContains keyword.
  std::optional<int> minContains; ///< The minContains keyword.
  
  std::optional<int> maxProperties; ///< The maxProperties keyword.
  std::optional<int> minProperties; ///< The minProperties keyword.
  std::optional<std::vector<std::string>> required; ///< The required keyword.
  
  std::shared_ptr<std::map<std::string, std::vector<std::string>>> dependentRequired; ///< The dependentRequired keyword.
  
  std::shared_ptr<std::vector<Schema>> allOf; ///< The allOf keyword.
  std::shared_ptr<std::vector<Schema>> anyOf; ///< The anyOf keyword.
  std::shared_ptr<std::vector<Schema>> oneOf; ///< The oneOf keyword.
  std::shared_ptr<Schema> not_schema; ///< The not keyword.
  
  std::shared_ptr<Schema> if_schema; ///< The if keyword.
  std::shared_ptr<Schema> then_schema; ///< The then keyword.
  std::shared_ptr<Schema> else_schema; ///< The else keyword.
  
  std::shared_ptr<std::vector<Schema>> prefixItems; ///< The prefixItems keyword.
  std::shared_ptr<Schema> contains; ///< The contains keyword.
  
  std::shared_ptr<std::map<std::string, Schema>> patternProperties; ///< The patternProperties keyword.
  std::shared_ptr<Schema> additionalProperties; ///< The additionalProperties keyword.
  std::shared_ptr<Schema> propertyNames; ///< The propertyNames keyword.
  
  std::shared_ptr<Schema> unevaluatedItems; ///< The unevaluatedItems keyword.
  std::shared_ptr<Schema> unevaluatedProperties; ///< The unevaluatedProperties keyword.
};

/// @brief Example object.
struct Example {
  std::optional<std::string> summary; ///< Short description for the example.
  std::optional<std::string> description; ///< Long description for the example.
  std::optional<std::string> value; ///< Embedded literal example.
  std::optional<std::string> externalValue; ///< A URL that points to the literal example.
  std::optional<Reference> ref; ///< A reference to an example.
};

/// @brief A single encoding definition applied to a single schema property.
struct Encoding {
  std::optional<std::string> contentType; ///< The Content-Type for encoding a specific property.
  std::optional<std::string> style; ///< Describes how a specific property value will be serialized.
  std::optional<bool> explode; ///< When this is true, property values of type array or object generate separate parameters for each value of the array, or key-value-pair of the map.
  std::optional<bool> allowReserved; ///< Determines whether the parameter value SHOULD allow reserved characters.
  std::shared_ptr<std::map<std::string, Header>> headers; ///< An array of header objects.
  std::shared_ptr<std::map<std::string, Encoding>> encoding; ///< A map for encoding options.
  std::shared_ptr<Schema> itemSchema; ///< Schema for array items encoding.
  std::shared_ptr<std::vector<Encoding>> prefixEncoding; ///< Array of encodings for prefix items.
  std::shared_ptr<Encoding> itemEncoding; ///< Encoding for array items.
};

/// @brief Each Media Type Object provides schema and examples for the media type identified by its key.
struct MediaType {
  std::optional<Schema> schema; ///< The schema defining the content of the request, response, or parameter.
  std::optional<std::string> example; ///< Example of the media type.
  std::optional<std::map<std::string, Example>> examples; ///< Examples of the media type.
  std::shared_ptr<std::map<std::string, Encoding>> encoding; ///< A map between a property name and its encoding information.
  std::shared_ptr<Schema> itemSchema; ///< Item schema representation.
  std::shared_ptr<std::vector<Encoding>> prefixEncoding; ///< Prefix items encoding.
  std::shared_ptr<Encoding> itemEncoding; ///< Item encoding.
};

/// @brief The Header Object follows the structure of the Parameter Object.
struct Header {
  std::optional<std::string> description; ///< A brief description of the header.
  bool required = false; ///< Determines whether this header is mandatory.
  bool deprecated = false; ///< Specifies that a header is deprecated and SHOULD be transitioned out of usage.
  bool allowEmptyValue = false; ///< Sets the ability to pass empty-valued headers.
  std::optional<std::string> style; ///< Describes how the header value will be serialized.
  std::optional<bool> explode; ///< When this is true, header values of type array or object generate separate parameters.
  std::optional<bool> allowReserved; ///< Determines whether the parameter value SHOULD allow reserved characters.
  std::optional<Schema> schema; ///< The schema defining the type used for the header.
  std::optional<std::string> example; ///< Example of the header's potential value.
  std::optional<std::map<std::string, Example>> examples; ///< Examples of the header's potential value.
  std::optional<std::map<std::string, MediaType>> content; ///< A map containing the representations for the header.
  std::optional<Reference> ref; ///< Reference to another header.
};

/// @brief Describes a single request body.
struct RequestBody {
  std::optional<std::string> description; ///< A brief description of the request body.
  std::map<std::string, MediaType> content; ///< The content of the request body.
  bool required = false; ///< Determines if the request body is required in the request.
  std::optional<Reference> ref; ///< Reference to a request body.
};

/// @brief The Link object represents a possible design-time link for a response.
struct Link {
  std::optional<std::string> operationRef; ///< A relative or absolute URI reference to an OAS operation.
  std::optional<std::string> operationId; ///< The name of an existing, resolvable OAS operation.
  std::optional<std::map<std::string, std::string>> parameters; ///< A map representing parameters to pass to an operation as specified with operationId or identified via operationRef.
  std::optional<std::string> requestBody; ///< A literal value or expression to use as a request body.
  std::optional<std::string> description; ///< A description of the link.
  std::optional<Server> server; ///< A server object to be used by the target operation.
  std::optional<Reference> ref; ///< A reference to another link.
};

/// @brief Describes a single response from an API Operation.
struct Response {
  std::string description; ///< A short description of the response.
  std::optional<std::map<std::string, Header>> headers; ///< Maps a header name to its definition.
  std::optional<std::map<std::string, MediaType>> content; ///< A map containing descriptions of potential response payloads.
  std::optional<std::map<std::string, Link>> links; ///< A map of operations links that can be followed from the response.
  std::optional<Reference> ref; ///< A reference to another response.
};

/// @brief Describes a single operation parameter.
struct Parameter {
  std::string name; ///< The name of the parameter.
  std::string in; ///< The location of the parameter. Possible values are "query", "header", "path" or "cookie".
  std::optional<std::string> description; ///< A brief description of the parameter.
  bool required = false; ///< Determines whether this parameter is mandatory.
  bool deprecated = false; ///< Specifies that a parameter is deprecated and SHOULD be transitioned out of usage.
  bool allowEmptyValue = false; ///< Sets the ability to pass empty-valued parameters.
  std::optional<std::string> style; ///< Describes how the parameter value will be serialized.
  std::optional<bool> explode; ///< When this is true, parameter values of type array or object generate separate parameters.
  std::optional<bool> allowReserved; ///< Determines whether the parameter value SHOULD allow reserved characters.
  std::optional<Schema> schema; ///< The schema defining the type used for the parameter.
  std::optional<std::string> example; ///< Example of the parameter's potential value.
  std::optional<std::map<std::string, Example>> examples; ///< Examples of the parameter's potential value.
  std::optional<std::map<std::string, MediaType>> content; ///< A map containing the representations for the parameter.
  std::optional<Reference> ref; ///< A reference to another parameter.
};

/// @brief Configuration details for a supported OAuth Flow.
struct OAuthFlow {
  std::optional<std::string> authorizationUrl; ///< The authorization URL to be used for this flow.
  std::optional<std::string> tokenUrl; ///< The token URL to be used for this flow.
  std::optional<std::string> refreshUrl; ///< The URL to be used for obtaining refresh tokens.
  std::optional<std::map<std::string, std::string>> scopes; ///< The available scopes for the OAuth2 security scheme.
};

/// @brief Allows configuration of the supported OAuth Flows.
struct OAuthFlows {
  std::optional<OAuthFlow> implicit; ///< Configuration for the OAuth Implicit flow.
  std::optional<OAuthFlow> password; ///< Configuration for the OAuth Resource Owner Password flow.
  std::optional<OAuthFlow> clientCredentials; ///< Configuration for the OAuth Client Credentials flow.
  std::optional<OAuthFlow> authorizationCode; ///< Configuration for the OAuth Authorization Code flow.
};

/// @brief Defines a security scheme that can be used by the operations.
struct SecurityScheme {
  std::string type; ///< The type of the security scheme.
  std::optional<std::string> description; ///< A short description for security scheme.
  std::optional<std::string> name; ///< The name of the header, query or cookie parameter to be used.
  std::optional<std::string> in; ///< The location of the API key.
  std::optional<std::string> scheme; ///< The name of the HTTP Authorization scheme to be used.
  std::optional<std::string> bearerFormat; ///< A hint to the client to identify how the bearer token is formatted.
  std::optional<OAuthFlows> flows; ///< An object containing configuration information for the flow types supported.
  std::optional<std::string> openIdConnectUrl; ///< OpenId Connect URL to discover OAuth2 configuration values.
  std::optional<Reference> ref; ///< Reference to another security scheme.
};

/// @brief Describes a single API operation on a path.
struct Operation {
  std::optional<std::vector<std::string>> tags; ///< A list of tags for API documentation control.
  std::optional<std::string> summary; ///< A short summary of what the operation does.
  std::optional<std::string> description; ///< A verbose explanation of the operation behavior.
  std::optional<ExternalDocumentation> externalDocs; ///< Additional external documentation for this operation.
  std::optional<std::string> operationId; ///< Unique string used to identify the operation.
  std::optional<std::vector<Parameter>> parameters; ///< A list of parameters that are applicable for this operation.
  std::optional<RequestBody> requestBody; ///< The request body applicable for this operation.
  std::optional<std::map<std::string, Response>> responses; ///< The list of possible responses as they are returned from executing this operation.
  std::shared_ptr<std::map<std::string, std::map<std::string, PathItem>>> callbacks; ///< A map of possible out-of band callbacks related to the parent operation.
  bool deprecated = false; ///< Declares this operation to be deprecated.
  std::optional<std::vector<SecurityRequirement>> security; ///< A declaration of which security mechanisms can be used for this operation.
  std::optional<std::vector<Server>> servers; ///< An alternative server array to service this operation.
};

/// @brief Describes the operations available on a single path.
struct PathItem {
  std::optional<std::string> ref; ///< Allows for an external definition of this path item.
  std::optional<std::string> summary; ///< An optional, string summary, intended to apply to all operations in this path.
  std::optional<std::string> description; ///< An optional, string description, intended to apply to all operations in this path.
  std::optional<Operation> get; ///< A definition of a GET operation on this path.
  std::optional<Operation> put; ///< A definition of a PUT operation on this path.
  std::optional<Operation> post; ///< A definition of a POST operation on this path.
  std::optional<Operation> delete_op; ///< A definition of a DELETE operation on this path.
  std::optional<Operation> options; ///< A definition of a OPTIONS operation on this path.
  std::optional<Operation> head; ///< A definition of a HEAD operation on this path.
  std::optional<Operation> patch; ///< A definition of a PATCH operation on this path.
  std::optional<Operation> trace; ///< A definition of a TRACE operation on this path.
  std::optional<Operation> query; ///< A definition of a QUERY operation on this path.
  std::shared_ptr<std::map<std::string, Operation>> additionalOperations; ///< Additional custom HTTP methods.
  std::optional<std::vector<Server>> servers; ///< An alternative server array to service all operations in this path.
  std::optional<std::vector<Parameter>> parameters; ///< A list of parameters that are applicable for all the operations described under this path.
};

/// @brief Holds a set of reusable objects for different aspects of the OAS.
struct Components {
  std::optional<std::map<std::string, Schema>> schemas; ///< An object to hold reusable Schema Objects.
  std::optional<std::map<std::string, Response>> responses; ///< An object to hold reusable Response Objects.
  std::optional<std::map<std::string, Parameter>> parameters; ///< An object to hold reusable Parameter Objects.
  std::optional<std::map<std::string, Example>> examples; ///< An object to hold reusable Example Objects.
  std::optional<std::map<std::string, RequestBody>> requestBodies; ///< An object to hold reusable Request Body Objects.
  std::optional<std::map<std::string, Header>> headers; ///< An object to hold reusable Header Objects.
  std::optional<std::map<std::string, SecurityScheme>> securitySchemes; ///< An object to hold reusable Security Scheme Objects.
  std::optional<std::map<std::string, Link>> links; ///< An object to hold reusable Link Objects.
  std::shared_ptr<std::map<std::string, std::map<std::string, PathItem>>> callbacks; ///< An object to hold reusable Callback Objects.
  std::optional<std::map<std::string, PathItem>> pathItems; ///< An object to hold reusable Path Item Objects.
  std::shared_ptr<std::map<std::string, MediaType>> mediaTypes; ///< Media types.
};

/// @brief This is the root document object of the OpenAPI document.
struct OpenAPI {
  std::string openapi; ///< This string MUST be the version number of the OpenAPI Specification.
  std::optional<std::string> self_link; ///< The $self field provides the self-assigned URI of this document.
  Info info; ///< Provides metadata about the API.
  std::optional<std::string> jsonSchemaDialect; ///< The default value for the $schema keyword within Schema Objects.
  std::optional<std::vector<Server>> servers; ///< An array of Server Objects.
  std::optional<std::map<std::string, PathItem>> paths; ///< The available paths and operations for the API.
  std::optional<std::map<std::string, PathItem>> webhooks; ///< The incoming webhooks that MAY be received as part of this API.
  std::optional<Components> components; ///< An element to hold various schemas for the document.
  std::optional<std::vector<SecurityRequirement>> security; ///< A declaration of which security mechanisms can be used across the API.
  std::optional<std::vector<Tag>> tags; ///< A list of tags used by the document with additional metadata.
  std::optional<ExternalDocumentation> externalDocs; ///< Additional external documentation.
};

} // namespace cdd_cpp::openapi