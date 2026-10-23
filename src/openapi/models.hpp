/// @file models.hpp
/// @brief Strongly typed OpenAPI 3.2.0 and JSON Schema Draft 2020-12 data
/// models.

#pragma once
#include "../utils/type_mapping.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

/// @namespace cdd_cpp::openapi
/// @brief OpenAPI specification models, parsers, emitters, and upgraders.
namespace cdd_cpp::openapi {

/// @brief Type alias for a security requirement map (scheme name -> required
/// scopes).
using SecurityRequirement = std::map<std::string, std::vector<std::string>>;

/// @brief Location of an operation parameter in an HTTP request.
enum class ParameterLocation {
  Query,      ///< Parameter passed in the URL query string ("query").
  Header,     ///< Parameter passed in an HTTP request header ("header").
  Path,       ///< Parameter embedded in the URL path template ("path").
  Cookie,     ///< Parameter passed in the Cookie header ("cookie").
  QueryString ///< Entire raw query string parameter ("querystring",
              ///< OpenAPI 3.2).
};

/// @brief Convert a ParameterLocation enumerator to its OpenAPI specification
/// string.
/// @param loc The ParameterLocation enumerator.
/// @return String view representation ("query", "header", "path", "cookie", or
/// "querystring").
constexpr std::string_view to_string(ParameterLocation loc) noexcept {
  switch (loc) {
  case ParameterLocation::Query:
    return "query";
  case ParameterLocation::Header:
    return "header";
  case ParameterLocation::Path:
    return "path";
  case ParameterLocation::Cookie:
    return "cookie";
  case ParameterLocation::QueryString:
    return "querystring";
  }
  return "query";
}

/// @brief Parse a string into a strongly typed ParameterLocation enumerator.
/// @param str The string to parse.
/// @return Optional ParameterLocation if recognized, std::nullopt otherwise.
constexpr std::optional<ParameterLocation>
parameter_location_from_string(std::string_view str) noexcept {
  if (str == "query")
    return ParameterLocation::Query;
  if (str == "header")
    return ParameterLocation::Header;
  if (str == "path")
    return ParameterLocation::Path;
  if (str == "cookie")
    return ParameterLocation::Cookie;
  if (str == "querystring")
    return ParameterLocation::QueryString;
  return std::nullopt;
}

/// @brief Serialization style for parameters and headers.
enum class ParameterStyle {
  Matrix,         ///< Matrix-style parameters prefixed with ';' ("matrix").
  Label,          ///< Label-style parameters prefixed with '.' ("label").
  Form,           ///< Form-style parameters delimited by '&' ("form").
  Simple,         ///< Simple comma-delimited parameters ("simple").
  SpaceDelimited, ///< Space-delimited array/object values ("spaceDelimited").
  PipeDelimited,  ///< Pipe-delimited array/object values ("pipeDelimited").
  DeepObject,     ///< Nested bracket object notation ("deepObject").
  Cookie          ///< Cookie parameter serialization style ("cookie").
};

/// @brief Convert a ParameterStyle enumerator to its OpenAPI specification
/// string.
/// @param style The ParameterStyle enumerator.
/// @return String view representation of the parameter style.
constexpr std::string_view to_string(ParameterStyle style) noexcept {
  switch (style) {
  case ParameterStyle::Matrix:
    return "matrix";
  case ParameterStyle::Label:
    return "label";
  case ParameterStyle::Form:
    return "form";
  case ParameterStyle::Simple:
    return "simple";
  case ParameterStyle::SpaceDelimited:
    return "spaceDelimited";
  case ParameterStyle::PipeDelimited:
    return "pipeDelimited";
  case ParameterStyle::DeepObject:
    return "deepObject";
  case ParameterStyle::Cookie:
    return "cookie";
  }
  return "simple";
}

/// @brief Parse a string into a strongly typed ParameterStyle enumerator.
/// @param str The string to parse.
/// @return Optional ParameterStyle if recognized, std::nullopt otherwise.
constexpr std::optional<ParameterStyle>
parameter_style_from_string(std::string_view str) noexcept {
  if (str == "matrix")
    return ParameterStyle::Matrix;
  if (str == "label")
    return ParameterStyle::Label;
  if (str == "form")
    return ParameterStyle::Form;
  if (str == "simple")
    return ParameterStyle::Simple;
  if (str == "spaceDelimited")
    return ParameterStyle::SpaceDelimited;
  if (str == "pipeDelimited")
    return ParameterStyle::PipeDelimited;
  if (str == "deepObject")
    return ParameterStyle::DeepObject;
  if (str == "cookie")
    return ParameterStyle::Cookie;
  return std::nullopt;
}

/// @brief Type of an OpenAPI SecurityScheme.
enum class SecuritySchemeType {
  ApiKey,       ///< API key security scheme ("apiKey").
  Http,         ///< HTTP authentication scheme ("http").
  MutualTLS,    ///< Mutual TLS certificate authentication ("mutualTLS").
  OAuth2,       ///< OAuth 2.0 security scheme ("oauth2").
  OpenIdConnect ///< OpenID Connect Discovery scheme ("openIdConnect").
};

/// @brief Convert a SecuritySchemeType enumerator to its OpenAPI specification
/// string.
/// @param type The SecuritySchemeType enumerator.
/// @return String view representation of the security scheme type.
constexpr std::string_view to_string(SecuritySchemeType type) noexcept {
  switch (type) {
  case SecuritySchemeType::ApiKey:
    return "apiKey";
  case SecuritySchemeType::Http:
    return "http";
  case SecuritySchemeType::MutualTLS:
    return "mutualTLS";
  case SecuritySchemeType::OAuth2:
    return "oauth2";
  case SecuritySchemeType::OpenIdConnect:
    return "openIdConnect";
  }
  return "http";
}

/// @brief Parse a string into a strongly typed SecuritySchemeType enumerator.
/// @param str The string to parse.
/// @return Optional SecuritySchemeType if recognized, std::nullopt otherwise.
constexpr std::optional<SecuritySchemeType>
security_scheme_type_from_string(std::string_view str) noexcept {
  if (str == "apiKey")
    return SecuritySchemeType::ApiKey;
  if (str == "http")
    return SecuritySchemeType::Http;
  if (str == "mutualTLS")
    return SecuritySchemeType::MutualTLS;
  if (str == "oauth2")
    return SecuritySchemeType::OAuth2;
  if (str == "openIdConnect")
    return SecuritySchemeType::OpenIdConnect;
  return std::nullopt;
}

/// @brief Location of an API key for an apiKey security scheme.
enum class ApiKeyLocation {
  Query,  ///< API key passed in query parameter ("query").
  Header, ///< API key passed in HTTP header ("header").
  Cookie  ///< API key passed in cookie ("cookie").
};

/// @brief Convert an ApiKeyLocation enumerator to its OpenAPI specification
/// string.
/// @param loc The ApiKeyLocation enumerator.
/// @return String view representation ("query", "header", or "cookie").
constexpr std::string_view to_string(ApiKeyLocation loc) noexcept {
  switch (loc) {
  case ApiKeyLocation::Query:
    return "query";
  case ApiKeyLocation::Header:
    return "header";
  case ApiKeyLocation::Cookie:
    return "cookie";
  }
  return "header";
}

/// @brief Parse a string into a strongly typed ApiKeyLocation enumerator.
/// @param str The string to parse.
/// @return Optional ApiKeyLocation if recognized, std::nullopt otherwise.
constexpr std::optional<ApiKeyLocation>
api_key_location_from_string(std::string_view str) noexcept {
  if (str == "query")
    return ApiKeyLocation::Query;
  if (str == "header")
    return ApiKeyLocation::Header;
  if (str == "cookie")
    return ApiKeyLocation::Cookie;
  return std::nullopt;
}

/// @brief JSON Schema Draft 2020-12 primitive data types.
enum class SchemaPrimitiveType {
  String,  ///< UTF-8 string ("string").
  Number,  ///< Arbitrary floating-point or integer number ("number").
  Integer, ///< Integral number ("integer").
  Boolean, ///< Boolean true/false ("boolean").
  Array,   ///< Ordered list of items ("array").
  Object,  ///< Key-value map ("object").
  Null     ///< Null value ("null").
};

/// @brief Convert a SchemaPrimitiveType enumerator to its JSON Schema string.
/// @param type The SchemaPrimitiveType enumerator.
/// @return String view representation of the primitive type.
constexpr std::string_view to_string(SchemaPrimitiveType type) noexcept {
  switch (type) {
  case SchemaPrimitiveType::String:
    return "string";
  case SchemaPrimitiveType::Number:
    return "number";
  case SchemaPrimitiveType::Integer:
    return "integer";
  case SchemaPrimitiveType::Boolean:
    return "boolean";
  case SchemaPrimitiveType::Array:
    return "array";
  case SchemaPrimitiveType::Object:
    return "object";
  case SchemaPrimitiveType::Null:
    return "null";
  }
  return "string";
}

/// @brief Parse a string into a strongly typed SchemaPrimitiveType enumerator.
/// @param str The string to parse.
/// @return Optional SchemaPrimitiveType if recognized, std::nullopt otherwise.
constexpr std::optional<SchemaPrimitiveType>
schema_primitive_type_from_string(std::string_view str) noexcept {
  if (str == "string")
    return SchemaPrimitiveType::String;
  if (str == "number")
    return SchemaPrimitiveType::Number;
  if (str == "integer")
    return SchemaPrimitiveType::Integer;
  if (str == "boolean")
    return SchemaPrimitiveType::Boolean;
  if (str == "array")
    return SchemaPrimitiveType::Array;
  if (str == "object")
    return SchemaPrimitiveType::Object;
  if (str == "null")
    return SchemaPrimitiveType::Null;
  return std::nullopt;
}

/// @brief Variant representing either a single primitive schema type or a union
/// array of types (OpenAPI 3.1/3.2).
using SchemaTypeVariant =
    std::variant<SchemaPrimitiveType, std::vector<SchemaPrimitiveType>>;

/// @brief Standard HTTP methods supported on an OpenAPI PathItem.
enum class HttpMethod {
  Get,     ///< HTTP GET method.
  Put,     ///< HTTP PUT method.
  Post,    ///< HTTP POST method.
  Delete,  ///< HTTP DELETE method.
  Options, ///< HTTP OPTIONS method.
  Head,    ///< HTTP HEAD method.
  Patch,   ///< HTTP PATCH method.
  Trace,   ///< HTTP TRACE method.
  Query    ///< HTTP QUERY method (OpenAPI 3.2).
};

/// @brief Convert an HttpMethod enumerator to its uppercase HTTP verb string.
/// @param method The HttpMethod enumerator.
/// @return Uppercase string view ("GET", "PUT", "POST", "DELETE", "OPTIONS",
/// "HEAD", "PATCH", "TRACE", "QUERY").
constexpr std::string_view to_uppercase_string(HttpMethod method) noexcept {
  switch (method) {
  case HttpMethod::Get:
    return "GET";
  case HttpMethod::Put:
    return "PUT";
  case HttpMethod::Post:
    return "POST";
  case HttpMethod::Delete:
    return "DELETE";
  case HttpMethod::Options:
    return "OPTIONS";
  case HttpMethod::Head:
    return "HEAD";
  case HttpMethod::Patch:
    return "PATCH";
  case HttpMethod::Trace:
    return "TRACE";
  case HttpMethod::Query:
    return "QUERY";
  }
  return "GET";
}

/// @brief Convert an HttpMethod enumerator to its lowercase OpenAPI PathItem
/// key string.
/// @param method The HttpMethod enumerator.
/// @return Lowercase string view ("get", "put", "post", "delete", "options",
/// "head", "patch", "trace", "query").
constexpr std::string_view to_lowercase_string(HttpMethod method) noexcept {
  switch (method) {
  case HttpMethod::Get:
    return "get";
  case HttpMethod::Put:
    return "put";
  case HttpMethod::Post:
    return "post";
  case HttpMethod::Delete:
    return "delete";
  case HttpMethod::Options:
    return "options";
  case HttpMethod::Head:
    return "head";
  case HttpMethod::Patch:
    return "patch";
  case HttpMethod::Trace:
    return "trace";
  case HttpMethod::Query:
    return "query";
  }
  return "get";
}

/// @brief Parse a case-insensitive HTTP verb string into an HttpMethod
/// enumerator.
/// @param str The HTTP verb string (e.g., "GET", "get", "DELETE", "delete").
/// @return Optional HttpMethod if recognized, std::nullopt otherwise.
constexpr std::optional<HttpMethod>
http_method_from_string(std::string_view str) noexcept {
  if (str == "GET" || str == "get")
    return HttpMethod::Get;
  if (str == "PUT" || str == "put")
    return HttpMethod::Put;
  if (str == "POST" || str == "post")
    return HttpMethod::Post;
  if (str == "DELETE" || str == "delete")
    return HttpMethod::Delete;
  if (str == "OPTIONS" || str == "options")
    return HttpMethod::Options;
  if (str == "HEAD" || str == "head")
    return HttpMethod::Head;
  if (str == "PATCH" || str == "patch")
    return HttpMethod::Patch;
  if (str == "TRACE" || str == "trace")
    return HttpMethod::Trace;
  if (str == "QUERY" || str == "query")
    return HttpMethod::Query;
  return std::nullopt;
}

/// @brief Strongly typed recursive JSON value for examples, defaults, enums,
/// and const values.
struct JsonValue {
  /// @brief Variant holding any valid JSON value type.
  using ValueVariant =
      std::variant<std::nullptr_t, bool, int64_t, double, std::string,
                   std::vector<JsonValue>, std::map<std::string, JsonValue>>;

  ValueVariant data = nullptr; ///< The underlying JSON value variant.

  /// @brief Default constructor initializing to JSON null.
  JsonValue() = default;

  /// @brief Construct a JsonValue from a nullptr.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(std::nullptr_t) noexcept : data(nullptr) {}

  /// @brief Construct a JsonValue from a boolean.
  /// @param b The boolean value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(bool b) noexcept : data(b) {}

  /// @brief Construct a JsonValue from a 64-bit integer.
  /// @param i The integer value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(int64_t i) noexcept : data(i) {}

  /// @brief Construct a JsonValue from an int.
  /// @param i The integer value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(int i) noexcept : data(static_cast<int64_t>(i)) {}

  /// @brief Construct a JsonValue from a double.
  /// @param d The floating-point value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(double d) noexcept : data(d) {}

  /// @brief Construct a JsonValue from a std::string.
  /// @param s The string value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(std::string s) noexcept : data(std::move(s)) {}

  /// @brief Construct a JsonValue from a C-string.
  /// @param s The C-string value.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(const char *s) noexcept : data(std::string(s ? s : "")) {}

  /// @brief Construct a JsonValue from a vector of JsonValues.
  /// @param arr The JSON array.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(std::vector<JsonValue> arr) noexcept : data(std::move(arr)) {}

  /// @brief Construct a JsonValue from a map of string to JsonValue.
  /// @param obj The JSON object.
  // NOLINTNEXTLINE(runtime/explicit)
  JsonValue(std::map<std::string, JsonValue> obj) noexcept
      : data(std::move(obj)) {}

  /// @brief Equality comparison between two JsonValue instances.
  /// @param other The other JsonValue to compare against.
  /// @return True if both hold the same type and value.
  bool operator==(const JsonValue &other) const noexcept = default;
};

struct Encoding;
struct PathItem;
struct Header;
struct Schema;

/// @brief A simple object to allow referencing other components in the
/// specification.
struct Reference {
  std::string ref = {}; ///< The reference string.
  std::optional<std::string> summary =
      {}; ///< A short summary of what the reference refers to.
  std::optional<std::string> description =
      {}; ///< A description of what the reference refers to.

  /// @brief Equality comparison operator.
  bool operator==(const Reference &) const noexcept = default;
};

/// @brief Contact information for the exposed API.
struct Contact {
  std::optional<std::string>
      name; ///< The identifying name of the contact person/organization.
  std::optional<std::string>
      url; ///< The URL pointing to the contact information.
  std::optional<std::string>
      email; ///< The email address of the contact person/organization.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Contact &) const noexcept = default;
};

/// @brief License information for the exposed API.
struct License {
  std::string name; ///< The license name used for the API.
  std::optional<std::string>
      identifier;                 ///< An SPDX license expression for the API.
  std::optional<std::string> url; ///< A URL to the license used for the API.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const License &) const noexcept = default;
};

/// @brief Provides metadata about the API.
struct Info {
  std::string title;                      ///< The title of the API.
  std::optional<std::string> summary;     ///< A short summary of the API.
  std::optional<std::string> description; ///< A description of the API.
  std::optional<std::string>
      termsOfService; ///< A URL to the Terms of Service for the API.
  std::optional<Contact>
      contact; ///< The contact information for the exposed API.
  std::optional<License>
      license;         ///< The license information for the exposed API.
  std::string version; ///< The version of the OpenAPI document.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Info &) const noexcept = default;
};

/// @brief An object representing a Server Variable for server URL template
/// substitution.
struct ServerVariable {
  std::optional<std::vector<std::string>>
      enum_values; ///< An enumeration of string values to be used if the
                   ///< substitution options are from a limited set.
  std::string default_value; ///< The default value to use for substitution.
  std::optional<std::string>
      description; ///< An optional description for the server variable.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const ServerVariable &) const noexcept = default;
};

/// @brief An object representing a Server.
struct Server {
  std::string url = {}; ///< A URL to the target host.
  std::optional<std::string> description =
      {};                               ///< An optional string describing the
                                        ///< host designated by the URL.
  std::optional<std::string> name = {}; ///< An optional name for the server.
  std::optional<std::map<std::string, ServerVariable>> variables =
      {}; ///< A map between a variable name and its value.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Server &) const noexcept = default;
};

/// @brief Allows referencing an external resource for extended documentation.
struct ExternalDocumentation {
  std::optional<std::string>
      description; ///< A description of the target documentation.
  std::string url; ///< The URL for the target documentation.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const ExternalDocumentation &) const noexcept = default;
};

/// @brief Adds metadata to a single tag that is used by the Operation Object.
struct Tag {
  std::string name;                       ///< The name of the tag.
  std::optional<std::string> summary;     ///< A short summary of the tag.
  std::optional<std::string> description; ///< A short description for the tag.
  std::optional<std::string> parent; ///< A hierarchical parent for the tag.
  std::optional<std::string>
      kind; ///< The kind of tag (e.g., entity, action, namespace).
  std::optional<ExternalDocumentation>
      externalDocs; ///< Additional external documentation for this tag.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Tag &) const noexcept = default;
};

/// @brief When request bodies or response payloads may be one of a number of
/// different schemas, a discriminator object can be used to aid in
/// serialization, deserialization, and validation.
struct Discriminator {
  std::string propertyName = {}; ///< The name of the property in the payload
                                 ///< that will hold the discriminator value.
  std::optional<std::map<std::string, std::string>> mapping =
      {}; ///< An object to hold mappings between payload values and schema
          ///< names or references.
  std::optional<std::string> defaultMapping =
      {}; ///< A default schema value to use if the discriminator
          ///< property does not match any mapping.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Discriminator &) const noexcept = default;
};

/// @brief A metadata object that allows for more fine-tuned XML model
/// definitions.
struct XML {
  std::optional<std::string>
      nodeType; ///< The XML node type ("element", "attribute", "text", "cdata",
                ///< "none", OpenAPI 3.2).
  std::optional<std::string>
      name; ///< Replaces the name of the element/attribute used for the
            ///< described schema property.
  std::optional<std::string>
      namespace_url;                 ///< The URI of the namespace definition.
  std::optional<std::string> prefix; ///< The prefix to be used for the name.
  std::optional<bool>
      attribute; ///< Declares whether the property definition translates to an
                 ///< attribute instead of an element.
  std::optional<bool> wrapped; ///< MAY be used only for an array definition.
                               ///< Signifies whether the array is wrapped.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const XML &) const noexcept = default;
};

/// @brief The Schema Object allows the definition of input and output data
/// types. Draft 2020-12 compliant.
struct Schema {
  std::optional<std::string> type =
      {}; ///< The type of the schema (e.g., string, integer, object, array).
  std::optional<std::vector<std::string>> types =
      {}; ///< Multi-type union array for OpenAPI 3.1/3.2 (e.g., ["string",
          ///< "null"]).
  std::optional<std::string> description = {}; ///< A description of the schema.
  std::optional<std::string> format = {};      ///< The format of the schema.
  std::optional<Reference> ref = {}; ///< A reference to another schema.
  std::optional<ExternalDocumentation> externalDocs =
      {}; ///< Additional external documentation.
  std::optional<std::string> example = {}; ///< Example of the schema.
  std::optional<std::vector<std::string>> examples =
      {}; ///< Array of schema examples (JSON Schema 2020-12).
  std::optional<Discriminator> discriminator =
      {};                      ///< Adds support for polymorphism.
  std::optional<XML> xml = {}; ///< This MAY be used only on properties schemas.

  std::shared_ptr<std::map<std::string, Schema>> properties =
      {};                             ///< Properties of an object.
  std::shared_ptr<Schema> items = {}; ///< Items of an array.

  // JSON Schema Draft 2020-12 Additions
  std::optional<std::string> schema_dialect = {}; ///< The $schema keyword.
  std::optional<std::string> id = {};             ///< The $id keyword.
  std::optional<std::string> anchor = {};         ///< The $anchor keyword.
  std::optional<std::string> dynamicAnchor =
      {};                                     ///< The $dynamicAnchor keyword.
  std::optional<std::string> dynamicRef = {}; ///< The $dynamicRef keyword.
  std::shared_ptr<std::map<std::string, Schema>> defs =
      {};                                        ///< The $defs keyword.
  std::optional<std::string> comment = {};       ///< The $comment keyword.
  std::optional<std::string> title = {};         ///< The title of the schema.
  std::optional<std::string> default_value = {}; ///< The default value.
  std::optional<std::vector<std::string>> enum_values =
      {}; ///< Allowed enumerated string values.
  std::optional<std::string> const_value = {}; ///< Constant value constraint.
  std::optional<bool> deprecated = {}; ///< Whether the schema is deprecated.
  std::optional<bool> readOnly = {};   ///< Whether the schema is readOnly.
  std::optional<bool> writeOnly = {};  ///< Whether the schema is writeOnly.
  std::optional<bool> boolean_schema =
      {}; ///< If set, represents a pure boolean schema (true/false).

  std::optional<double> multipleOf = {}; ///< The multipleOf keyword.
  std::optional<double> maximum = {};    ///< The maximum keyword.
  std::optional<double> exclusiveMaximum =
      {};                             ///< The exclusiveMaximum keyword.
  std::optional<double> minimum = {}; ///< The minimum keyword.
  std::optional<double> exclusiveMinimum =
      {}; ///< The exclusiveMinimum keyword.

  std::optional<int> maxLength = {};       ///< The maxLength keyword.
  std::optional<int> minLength = {};       ///< The minLength keyword.
  std::optional<std::string> pattern = {}; ///< The pattern keyword.

  std::optional<int> maxItems = {};     ///< The maxItems keyword.
  std::optional<int> minItems = {};     ///< The minItems keyword.
  std::optional<bool> uniqueItems = {}; ///< The uniqueItems keyword.

  std::optional<int> maxContains = {}; ///< The maxContains keyword.
  std::optional<int> minContains = {}; ///< The minContains keyword.

  std::optional<int> maxProperties = {}; ///< The maxProperties keyword.
  std::optional<int> minProperties = {}; ///< The minProperties keyword.
  std::optional<std::vector<std::string>> required =
      {}; ///< The required keyword.

  std::shared_ptr<std::map<std::string, std::vector<std::string>>>
      dependentRequired = {}; ///< The dependentRequired keyword.

  std::shared_ptr<std::vector<Schema>> allOf = {}; ///< The allOf keyword.
  std::shared_ptr<std::vector<Schema>> anyOf = {}; ///< The anyOf keyword.
  std::shared_ptr<std::vector<Schema>> oneOf = {}; ///< The oneOf keyword.
  std::shared_ptr<Schema> not_schema = {};         ///< The not keyword.

  std::shared_ptr<Schema> if_schema = {};   ///< The if keyword.
  std::shared_ptr<Schema> then_schema = {}; ///< The then keyword.
  std::shared_ptr<Schema> else_schema = {}; ///< The else keyword.

  std::shared_ptr<std::vector<Schema>> prefixItems =
      {};                                ///< The prefixItems keyword.
  std::shared_ptr<Schema> contains = {}; ///< The contains keyword.

  std::shared_ptr<std::map<std::string, Schema>> patternProperties =
      {}; ///< The patternProperties keyword.
  std::shared_ptr<Schema> additionalProperties =
      {}; ///< The additionalProperties schema keyword.
  std::optional<bool> additionalPropertiesBool =
      {}; ///< Boolean additionalProperties (true/false).
  std::shared_ptr<Schema> propertyNames = {}; ///< The propertyNames keyword.

  std::shared_ptr<Schema> unevaluatedItems =
      {}; ///< The unevaluatedItems keyword.
  std::optional<bool> unevaluatedItemsBool =
      {}; ///< Boolean unevaluatedItems (true/false).
  std::shared_ptr<Schema> unevaluatedProperties =
      {}; ///< The unevaluatedProperties keyword.
  std::optional<bool> unevaluatedPropertiesBool =
      {}; ///< Boolean unevaluatedProperties (true/false).
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Get the strongly typed SchemaTypeVariant for this schema.
  /// @return Optional SchemaTypeVariant representing either a single primitive
  /// type or a vector of primitive types.
  std::optional<SchemaTypeVariant> typed_type() const noexcept {
    if (types.has_value() && !types->empty()) {
      std::vector<SchemaPrimitiveType> out;
      for (const auto &t : *types) {
        if (auto pt = schema_primitive_type_from_string(t)) {
          out.push_back(*pt);
        }
      }
      if (!out.empty())
        return out;
    }
    if (type.has_value()) {
      if (auto pt = schema_primitive_type_from_string(*type)) {
        return *pt;
      }
    }
    return std::nullopt;
  }

  /// @brief Set the schema type from a strongly typed SchemaPrimitiveType.
  /// @param pt The primitive schema type to set.
  void set_typed_type(SchemaPrimitiveType pt) noexcept {
    type = std::string(to_string(pt));
    types = std::nullopt;
  }

  /// @brief Set the schema type from a vector of SchemaPrimitiveType
  /// (OpenAPI 3.1/3.2 type union).
  /// @param pts Vector of primitive schema types.
  void set_typed_types(const std::vector<SchemaPrimitiveType> &pts) noexcept {
    std::vector<std::string> str_types;
    str_types.reserve(pts.size());
    for (auto pt : pts) {
      str_types.emplace_back(to_string(pt));
    }
    types = std::move(str_types);
    if (!types->empty()) {
      type = types->front();
    }
  }

  /// @brief Equality comparison operator.
  bool operator==(const Schema &other) const noexcept;
};

/// @brief Example object.
struct Example {
  std::optional<std::string> summary; ///< Short description for the example.
  std::optional<std::string> description; ///< Long description for the example.
  std::optional<JsonValue>
      dataValue; ///< Embedded example of the data structure MUST be valid
                 ///< according to the schema (OpenAPI 3.2).
  std::optional<std::string>
      serializedValue; ///< Embedded example of the serialized form of the value
                       ///< (OpenAPI 3.2).
  std::optional<std::string> value; ///< Embedded literal example.
  std::optional<JsonValue>
      typed_value; ///< Strongly typed JSON value for non-string examples.
  std::optional<std::string>
      externalValue;            ///< A URL that points to the literal example.
  std::optional<Reference> ref; ///< A reference to an example.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Example &) const noexcept = default;
};

/// @brief A single encoding definition applied to a single schema property.
struct Encoding {
  std::optional<std::string>
      contentType; ///< The Content-Type for encoding a specific property.
  std::optional<std::string>
      style; ///< Describes how a specific property value will be serialized.
  std::optional<bool>
      explode; ///< When this is true, property values of type array or object
               ///< generate separate parameters for each value of the array, or
               ///< key-value-pair of the map.
  std::optional<bool> allowReserved; ///< Determines whether the parameter value
                                     ///< SHOULD allow reserved characters.
  std::shared_ptr<std::map<std::string, Header>>
      headers; ///< An array of header objects.
  std::shared_ptr<std::map<std::string, Encoding>>
      encoding;                       ///< A map for encoding options.
  std::shared_ptr<Schema> itemSchema; ///< Schema for array items encoding.
  std::shared_ptr<std::vector<Encoding>>
      prefixEncoding; ///< Array of encodings for prefix items.
  std::shared_ptr<Encoding> itemEncoding; ///< Encoding for array items.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Get the strongly typed ParameterStyle for this encoding.
  /// @return Optional ParameterStyle enumerator.
  std::optional<ParameterStyle> typed_style() const noexcept {
    if (!style.has_value())
      return std::nullopt;
    return parameter_style_from_string(*style);
  }

  /// @brief Set the encoding style from a strongly typed ParameterStyle
  /// enumerator.
  /// @param s The ParameterStyle to set.
  void set_typed_style(ParameterStyle s) noexcept {
    style = std::string(to_string(s));
  }

  /// @brief Equality comparison operator.
  bool operator==(const Encoding &other) const noexcept;
};

/// @brief Each Media Type Object provides schema and examples for the media
/// type identified by its key.
struct MediaType {
  std::optional<Schema> schema; ///< The schema defining the content of the
                                ///< request, response, or parameter.
  std::optional<std::string> example; ///< Example of the media type.
  std::optional<std::map<std::string, Example>>
      examples; ///< Examples of the media type.
  std::shared_ptr<std::map<std::string, Encoding>>
      encoding; ///< A map between a property name and its encoding information.
  std::shared_ptr<Schema> itemSchema; ///< Item schema representation.
  std::shared_ptr<std::vector<Encoding>>
      prefixEncoding;                     ///< Prefix items encoding.
  std::shared_ptr<Encoding> itemEncoding; ///< Item encoding.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const MediaType &other) const noexcept;
};

/// @brief The Header Object follows the structure of the Parameter Object.
struct Header {
  std::optional<std::string>
      description;         ///< A brief description of the header.
  bool required = false;   ///< Determines whether this header is mandatory.
  bool deprecated = false; ///< Specifies that a header is deprecated and SHOULD
                           ///< be transitioned out of usage.
  bool allowEmptyValue =
      false; ///< Sets the ability to pass empty-valued headers.
  std::optional<std::string>
      style; ///< Describes how the header value will be serialized.
  std::optional<bool>
      explode; ///< When this is true, header values of type array or object
               ///< generate separate parameters.
  std::optional<bool> allowReserved; ///< Determines whether the parameter value
                                     ///< SHOULD allow reserved characters.
  std::optional<Schema>
      schema; ///< The schema defining the type used for the header.
  std::optional<std::string>
      example; ///< Example of the header's potential value.
  std::optional<std::map<std::string, Example>>
      examples; ///< Examples of the header's potential value.
  std::optional<std::map<std::string, MediaType>>
      content; ///< A map containing the representations for the header.
  std::optional<Reference> ref; ///< Reference to another header.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Get the strongly typed ParameterStyle for this header.
  /// @return Optional ParameterStyle enumerator.
  std::optional<ParameterStyle> typed_style() const noexcept {
    if (!style.has_value())
      return std::nullopt;
    return parameter_style_from_string(*style);
  }

  /// @brief Set the header style from a strongly typed ParameterStyle
  /// enumerator.
  /// @param s The ParameterStyle to set.
  void set_typed_style(ParameterStyle s) noexcept {
    style = std::string(to_string(s));
  }

  /// @brief Equality comparison operator.
  bool operator==(const Header &) const noexcept = default;
};

/// @brief Describes a single request body.
struct RequestBody {
  std::optional<std::string>
      description; ///< A brief description of the request body.
  std::map<std::string, MediaType>
      content; ///< The content of the request body.
  bool required =
      false; ///< Determines if the request body is required in the request.
  std::optional<Reference> ref; ///< Reference to a request body.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const RequestBody &) const noexcept = default;
};

/// @brief The Link object represents a possible design-time link for a
/// response.
struct Link {
  std::optional<std::string> operationRef; ///< A relative or absolute URI
                                           ///< reference to an OAS operation.
  std::optional<std::string>
      operationId; ///< The name of an existing, resolvable OAS operation.
  std::optional<std::map<std::string, std::string>>
      parameters; ///< A map representing parameters to pass to an operation as
                  ///< specified with operationId or identified via
                  ///< operationRef.
  std::optional<std::string>
      requestBody; ///< A literal value or expression to use as a request body.
  std::optional<std::string> description; ///< A description of the link.
  std::optional<Server>
      server; ///< A server object to be used by the target operation.
  std::optional<Reference> ref; ///< A reference to another link.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Link &) const noexcept = default;
};

/// @brief Describes a single response from an API Operation.
struct Response {
  std::string description; ///< A short description of the response.
  std::optional<std::map<std::string, Header>>
      headers; ///< Maps a header name to its definition.
  std::optional<std::map<std::string, MediaType>>
      content; ///< A map containing descriptions of potential response
               ///< payloads.
  std::optional<std::map<std::string, Link>>
      links; ///< A map of operations links that can be followed from the
             ///< response.
  std::optional<Reference> ref; ///< A reference to another response.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Response &) const noexcept = default;
};

/// @brief Describes a single operation parameter.
struct Parameter {
  std::string name; ///< The name of the parameter.
  std::string in; ///< The location of the parameter ("query", "header", "path",
                  ///< "cookie", "querystring").
  std::optional<std::string> description =
      std::nullopt;        ///< A brief description of the parameter.
  bool required = false;   ///< Determines whether this parameter is mandatory.
  bool deprecated = false; ///< Specifies that a parameter is deprecated and
                           ///< SHOULD be transitioned out of usage.
  bool allowEmptyValue =
      false; ///< Sets the ability to pass empty-valued parameters.
  std::optional<std::string> style =
      std::nullopt; ///< Describes how the parameter value will be serialized.
  std::optional<bool> explode =
      std::nullopt; ///< When this is true, parameter values of type array or
                    ///< object generate separate parameters.
  std::optional<bool> allowReserved =
      std::nullopt; ///< Determines whether the parameter value
                    ///< SHOULD allow reserved characters.
  std::optional<Schema> schema =
      std::nullopt; ///< The schema defining the type used for the parameter.
  std::optional<std::string> example =
      std::nullopt; ///< Example of the parameter's potential value.
  std::optional<std::map<std::string, Example>> examples =
      std::nullopt; ///< Examples of the parameter's potential value.
  std::optional<std::map<std::string, MediaType>> content =
      std::nullopt; ///< A map containing the representations for the parameter.
  std::optional<Reference> ref =
      std::nullopt; ///< A reference to another parameter.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Get the strongly typed ParameterLocation for this parameter.
  /// @return Optional ParameterLocation enumerator.
  std::optional<ParameterLocation> typed_in() const noexcept {
    return parameter_location_from_string(in);
  }

  /// @brief Set the parameter location from a strongly typed ParameterLocation
  /// enumerator.
  /// @param loc The ParameterLocation to set.
  void set_typed_in(ParameterLocation loc) noexcept {
    in = std::string(to_string(loc));
  }

  /// @brief Get the strongly typed ParameterStyle for this parameter.
  /// @return Optional ParameterStyle enumerator.
  std::optional<ParameterStyle> typed_style() const noexcept {
    if (!style.has_value())
      return std::nullopt;
    return parameter_style_from_string(*style);
  }

  /// @brief Set the parameter style from a strongly typed ParameterStyle
  /// enumerator.
  /// @param s The ParameterStyle to set.
  void set_typed_style(ParameterStyle s) noexcept {
    style = std::string(to_string(s));
  }

  /// @brief Equality comparison operator.
  bool operator==(const Parameter &) const noexcept = default;
};

/// @brief Configuration details for a supported OAuth Flow.
struct OAuthFlow {
  std::optional<std::string>
      authorizationUrl; ///< The authorization URL to be used for this flow.
  std::optional<std::string>
      tokenUrl; ///< The token URL to be used for this flow.
  std::optional<std::string>
      refreshUrl; ///< The URL to be used for obtaining refresh tokens.
  std::optional<std::string>
      deviceAuthorizationUrl; ///< The device authorization URL to be used for
                              ///< this flow.
  std::optional<std::map<std::string, std::string>>
      scopes; ///< The available scopes for the OAuth2 security scheme.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const OAuthFlow &) const noexcept = default;
};

/// @brief Allows configuration of the supported OAuth Flows.
struct OAuthFlows {
  std::optional<OAuthFlow>
      implicit; ///< Configuration for the OAuth Implicit flow.
  std::optional<OAuthFlow>
      password; ///< Configuration for the OAuth Resource Owner Password flow.
  std::optional<OAuthFlow> clientCredentials;   ///< Configuration for the OAuth
                                                ///< Client Credentials flow.
  std::optional<OAuthFlow> authorizationCode;   ///< Configuration for the OAuth
                                                ///< Authorization Code flow.
  std::optional<OAuthFlow> deviceAuthorization; ///< Configuration for the OAuth
                                                ///< Device Authorization flow.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const OAuthFlows &) const noexcept = default;
};

/// @brief Defines a security scheme that can be used by the operations.
struct SecurityScheme {
  std::string type; ///< The type of the security scheme.
  std::optional<std::string>
      description; ///< A short description for security scheme.
  std::optional<std::string>
      name; ///< The name of the header, query or cookie parameter to be used.
  std::optional<std::string> in; ///< The location of the API key.
  std::optional<std::string>
      scheme; ///< The name of the HTTP Authorization scheme to be used.
  std::optional<std::string>
      bearerFormat; ///< A hint to the client to identify how the bearer token
                    ///< is formatted.
  std::optional<OAuthFlows>
      flows; ///< An object containing configuration information for the flow
             ///< types supported.
  std::optional<std::string>
      openIdConnectUrl; ///< OpenId Connect URL to discover OAuth2 configuration
                        ///< values.
  std::optional<std::string>
      oauth2MetadataUrl; ///< OpenId Connect URL to discover OAuth2
                         ///< configuration values.
  std::optional<bool>
      deprecated; ///< Specifies whether the security scheme is deprecated.
  std::optional<Reference> ref; ///< Reference to another security scheme.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Get the strongly typed SecuritySchemeType.
  /// @return Optional SecuritySchemeType enumerator.
  std::optional<SecuritySchemeType> typed_type() const noexcept {
    return security_scheme_type_from_string(type);
  }

  /// @brief Set the security scheme type from a strongly typed
  /// SecuritySchemeType enumerator.
  /// @param t The SecuritySchemeType to set.
  void set_typed_type(SecuritySchemeType t) noexcept {
    type = std::string(to_string(t));
  }

  /// @brief Get the strongly typed ApiKeyLocation.
  /// @return Optional ApiKeyLocation enumerator.
  std::optional<ApiKeyLocation> typed_in() const noexcept {
    if (!in.has_value())
      return std::nullopt;
    return api_key_location_from_string(*in);
  }

  /// @brief Set the API key location from a strongly typed ApiKeyLocation
  /// enumerator.
  /// @param loc The ApiKeyLocation to set.
  void set_typed_in(ApiKeyLocation loc) noexcept {
    in = std::string(to_string(loc));
  }

  /// @brief Equality comparison operator.
  bool operator==(const SecurityScheme &) const noexcept = default;
};

/// @brief Describes a single API operation on a path.
struct Operation {
  std::optional<std::vector<std::string>>
      tags; ///< A list of tags for API documentation control.
  std::optional<std::string>
      summary; ///< A short summary of what the operation does.
  std::optional<std::string>
      description; ///< A verbose explanation of the operation behavior.
  std::optional<ExternalDocumentation>
      externalDocs; ///< Additional external documentation for this operation.
  std::optional<std::string>
      operationId; ///< Unique string used to identify the operation.
  std::optional<std::vector<Parameter>>
      parameters; ///< A list of parameters that are applicable for this
                  ///< operation.
  std::optional<RequestBody>
      requestBody; ///< The request body applicable for this operation.
  std::optional<std::map<std::string, Response>>
      responses; ///< The list of possible responses as they are returned from
                 ///< executing this operation.
  std::shared_ptr<std::map<std::string, std::map<std::string, PathItem>>>
      callbacks; ///< A map of possible out-of band callbacks related to the
                 ///< parent operation.
  bool deprecated = false; ///< Declares this operation to be deprecated.
  std::optional<std::vector<SecurityRequirement>>
      security; ///< A declaration of which security mechanisms can be used for
                ///< this operation.
  std::optional<std::vector<Server>>
      servers; ///< An alternative server array to service this operation.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Operation &) const noexcept;
};

/// @brief Describes the operations available on a single path.
struct PathItem {
  std::optional<std::string>
      ref; ///< Allows for an external definition of this path item.
  std::optional<std::string>
      summary; ///< An optional, string summary, intended to apply to all
               ///< operations in this path.
  std::optional<std::string>
      description; ///< An optional, string description, intended to apply to
                   ///< all operations in this path.
  std::optional<Operation>
      get; ///< A definition of a GET operation on this path.
  std::optional<Operation>
      put; ///< A definition of a PUT operation on this path.
  std::optional<Operation>
      post; ///< A definition of a POST operation on this path.
  std::optional<Operation>
      delete_op; ///< A definition of a DELETE operation on this path.
  std::optional<Operation>
      options; ///< A definition of a OPTIONS operation on this path.
  std::optional<Operation>
      head; ///< A definition of a HEAD operation on this path.
  std::optional<Operation>
      patch; ///< A definition of a PATCH operation on this path.
  std::optional<Operation>
      trace; ///< A definition of a TRACE operation on this path.
  std::optional<Operation>
      query; ///< A definition of a QUERY operation on this path.
  std::shared_ptr<std::map<std::string, Operation>>
      additionalOperations; ///< Additional custom HTTP methods.
  std::optional<std::vector<Server>>
      servers; ///< An alternative server array to service all operations in
               ///< this path.
  std::optional<std::vector<Parameter>>
      parameters; ///< A list of parameters that are applicable for all the
                  ///< operations described under this path.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Retrieve a mutable reference to the optional Operation slot for a
  /// given HttpMethod.
  /// @param method The HttpMethod to access.
  /// @return Reference to the corresponding std::optional<Operation> member.
  std::optional<Operation> &operation_for(HttpMethod method) noexcept {
    switch (method) {
    case HttpMethod::Get:
      return get;
    case HttpMethod::Put:
      return put;
    case HttpMethod::Post:
      return post;
    case HttpMethod::Delete:
      return delete_op;
    case HttpMethod::Options:
      return options;
    case HttpMethod::Head:
      return head;
    case HttpMethod::Patch:
      return patch;
    case HttpMethod::Trace:
      return trace;
    case HttpMethod::Query:
      return query;
    }
    return get;
  }

  /// @brief Retrieve a const reference to the optional Operation slot for a
  /// given HttpMethod.
  /// @param method The HttpMethod to access.
  /// @return Const reference to the corresponding std::optional<Operation>
  /// member.
  const std::optional<Operation> &
  operation_for(HttpMethod method) const noexcept {
    switch (method) {
    case HttpMethod::Get:
      return get;
    case HttpMethod::Put:
      return put;
    case HttpMethod::Post:
      return post;
    case HttpMethod::Delete:
      return delete_op;
    case HttpMethod::Options:
      return options;
    case HttpMethod::Head:
      return head;
    case HttpMethod::Patch:
      return patch;
    case HttpMethod::Trace:
      return trace;
    case HttpMethod::Query:
      return query;
    }
    return get;
  }

  /// @brief Set an operation on this PathItem by HTTP method string (e.g.,
  /// "GET", "POST", or custom).
  /// @param method_str The HTTP method name.
  /// @param op The Operation to assign.
  void set_operation(std::string_view method_str, Operation op) noexcept {
    if (auto m = http_method_from_string(method_str)) {
      operation_for(*m) = std::move(op);
    } else {
      if (!additionalOperations) {
        additionalOperations =
            std::make_shared<std::map<std::string, Operation>>();
      }
      (*additionalOperations)[std::string(method_str)] = std::move(op);
    }
  }

  /// @brief Equality comparison operator.
  bool operator==(const PathItem &other) const noexcept;
};

/// @brief Holds a set of reusable objects for different aspects of the OAS.
struct Components {
  std::optional<std::map<std::string, Schema>>
      schemas; ///< An object to hold reusable Schema Objects.
  std::optional<std::map<std::string, Response>>
      responses; ///< An object to hold reusable Response Objects.
  std::optional<std::map<std::string, Parameter>>
      parameters; ///< An object to hold reusable Parameter Objects.
  std::optional<std::map<std::string, Example>>
      examples; ///< An object to hold reusable Example Objects.
  std::optional<std::map<std::string, RequestBody>>
      requestBodies; ///< An object to hold reusable Request Body Objects.
  std::optional<std::map<std::string, Header>>
      headers; ///< An object to hold reusable Header Objects.
  std::optional<std::map<std::string, SecurityScheme>>
      securitySchemes; ///< An object to hold reusable Security Scheme Objects.
  std::optional<std::map<std::string, Link>>
      links; ///< An object to hold reusable Link Objects.
  std::shared_ptr<std::map<std::string, std::map<std::string, PathItem>>>
      callbacks; ///< An object to hold reusable Callback Objects.
  std::optional<std::map<std::string, PathItem>>
      pathItems; ///< An object to hold reusable Path Item Objects.
  std::shared_ptr<std::map<std::string, MediaType>>
      mediaTypes; ///< Media types.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const Components &other) const noexcept;
};

/// @brief This is the root document object of the OpenAPI document.
struct OpenAPI {
  std::string openapi; ///< This string MUST be the version number of the
                       ///< OpenAPI Specification.
  std::optional<std::string> self_link; ///< The $self field provides the
                                        ///< self-assigned URI of this document.
  Info info;                            ///< Provides metadata about the API.
  std::optional<std::string>
      jsonSchemaDialect; ///< The default value for the $schema keyword within
                         ///< Schema Objects.
  std::optional<std::vector<Server>> servers; ///< An array of Server Objects.
  std::optional<std::map<std::string, PathItem>>
      paths; ///< The available paths and operations for the API.
  std::optional<std::map<std::string, PathItem>>
      webhooks; ///< The incoming webhooks that MAY be received as part of this
                ///< API.
  std::optional<Components>
      components; ///< An element to hold various schemas for the document.
  std::optional<std::vector<SecurityRequirement>>
      security; ///< A declaration of which security mechanisms can be used
                ///< across the API.
  std::optional<std::vector<Tag>>
      tags; ///< A list of tags used by the document with additional metadata.
  std::optional<ExternalDocumentation>
      externalDocs; ///< Additional external documentation.
  std::map<std::string, JsonValue> extensions =
      {}; ///< Specification extensions (x-*).

  /// @brief Equality comparison operator.
  bool operator==(const OpenAPI &) const noexcept = default;
};

/// @brief Deep-compare two std::shared_ptr instances by dereferenced value.
/// @tparam T The pointed-to type.
/// @param a First shared pointer.
/// @param b Second shared pointer.
/// @return True if both are null or both are non-null with equal values.
template <typename T>
inline bool deep_eq(const std::shared_ptr<T> &a,
                    const std::shared_ptr<T> &b) noexcept {
  if (a == b)
    return true;
  if (!a || !b)
    return false;
  return *a == *b;
}

/// @brief Map an OpenAPI primitive type string to its C++ primitive/standard
/// library type.
/// @param openapi_type The OpenAPI type string (e.g., "integer", "number",
/// "boolean", "string").
/// @return C++ type string ("int", "double", "bool", or "std::string").
inline std::string openapi_type_to_cpp(std::string_view openapi_type) noexcept {
  return utils::openapi_type_to_cpp(openapi_type);
}

/// @brief Map a C++ primitive type string to its OpenAPI primitive type string.
/// @param cpp_type The C++ type string (e.g., "int", "double", "bool",
/// "std::string").
/// @return OpenAPI primitive type string ("integer", "number", "boolean", or
/// "string").
inline std::string cpp_type_to_openapi(std::string_view cpp_type) noexcept {
  return utils::cpp_type_to_openapi(cpp_type);
}

/// @brief Deep value equality comparison for Schema.
/// @param other The Schema to compare against.
/// @return True if all fields and recursive sub-schemas are equal by value.
inline bool Schema::operator==(const Schema &other) const noexcept {
  return type == other.type && types == other.types &&
         description == other.description && format == other.format &&
         ref == other.ref && externalDocs == other.externalDocs &&
         example == other.example && examples == other.examples &&
         discriminator == other.discriminator && xml == other.xml &&
         deep_eq(properties, other.properties) && deep_eq(items, other.items) &&
         schema_dialect == other.schema_dialect && id == other.id &&
         anchor == other.anchor && dynamicAnchor == other.dynamicAnchor &&
         dynamicRef == other.dynamicRef && deep_eq(defs, other.defs) &&
         comment == other.comment && title == other.title &&
         default_value == other.default_value &&
         enum_values == other.enum_values && const_value == other.const_value &&
         deprecated == other.deprecated && readOnly == other.readOnly &&
         writeOnly == other.writeOnly &&
         boolean_schema == other.boolean_schema &&
         multipleOf == other.multipleOf && maximum == other.maximum &&
         exclusiveMaximum == other.exclusiveMaximum &&
         minimum == other.minimum &&
         exclusiveMinimum == other.exclusiveMinimum &&
         maxLength == other.maxLength && minLength == other.minLength &&
         pattern == other.pattern && maxItems == other.maxItems &&
         minItems == other.minItems && uniqueItems == other.uniqueItems &&
         maxContains == other.maxContains && minContains == other.minContains &&
         maxProperties == other.maxProperties &&
         minProperties == other.minProperties && required == other.required &&
         deep_eq(dependentRequired, other.dependentRequired) &&
         deep_eq(allOf, other.allOf) && deep_eq(anyOf, other.anyOf) &&
         deep_eq(oneOf, other.oneOf) && deep_eq(not_schema, other.not_schema) &&
         deep_eq(if_schema, other.if_schema) &&
         deep_eq(then_schema, other.then_schema) &&
         deep_eq(else_schema, other.else_schema) &&
         deep_eq(prefixItems, other.prefixItems) &&
         deep_eq(contains, other.contains) &&
         deep_eq(patternProperties, other.patternProperties) &&
         deep_eq(additionalProperties, other.additionalProperties) &&
         additionalPropertiesBool == other.additionalPropertiesBool &&
         deep_eq(propertyNames, other.propertyNames) &&
         deep_eq(unevaluatedItems, other.unevaluatedItems) &&
         unevaluatedItemsBool == other.unevaluatedItemsBool &&
         deep_eq(unevaluatedProperties, other.unevaluatedProperties) &&
         unevaluatedPropertiesBool == other.unevaluatedPropertiesBool &&
         extensions == other.extensions;
}

/// @brief Deep value equality comparison for Encoding.
/// @param other The Encoding to compare against.
/// @return True if all fields and nested encodings/schemas are equal by value.
inline bool Encoding::operator==(const Encoding &other) const noexcept {
  return contentType == other.contentType && style == other.style &&
         explode == other.explode && allowReserved == other.allowReserved &&
         deep_eq(headers, other.headers) && deep_eq(encoding, other.encoding) &&
         deep_eq(itemSchema, other.itemSchema) &&
         deep_eq(prefixEncoding, other.prefixEncoding) &&
         deep_eq(itemEncoding, other.itemEncoding) &&
         extensions == other.extensions;
}

/// @brief Deep value equality comparison for MediaType.
/// @param other The MediaType to compare against.
/// @return True if all fields and nested encodings/schemas are equal by value.
inline bool MediaType::operator==(const MediaType &other) const noexcept {
  return schema == other.schema && example == other.example &&
         examples == other.examples && deep_eq(encoding, other.encoding) &&
         deep_eq(itemSchema, other.itemSchema) &&
         deep_eq(prefixEncoding, other.prefixEncoding) &&
         deep_eq(itemEncoding, other.itemEncoding) &&
         extensions == other.extensions;
}

/// @brief Deep value equality comparison for Operation.
/// @param other The Operation to compare against.
/// @return True if all fields and callbacks are equal by value.
inline bool Operation::operator==(const Operation &other) const noexcept {
  return tags == other.tags && summary == other.summary &&
         description == other.description &&
         externalDocs == other.externalDocs &&
         operationId == other.operationId && parameters == other.parameters &&
         requestBody == other.requestBody && responses == other.responses &&
         deep_eq(callbacks, other.callbacks) &&
         deprecated == other.deprecated && security == other.security &&
         servers == other.servers && extensions == other.extensions;
}

/// @brief Deep value equality comparison for PathItem.
/// @param other The PathItem to compare against.
/// @return True if all operations and fields are equal by value.
inline bool PathItem::operator==(const PathItem &other) const noexcept {
  return ref == other.ref && summary == other.summary &&
         description == other.description && get == other.get &&
         put == other.put && post == other.post &&
         delete_op == other.delete_op && options == other.options &&
         head == other.head && patch == other.patch && trace == other.trace &&
         query == other.query &&
         deep_eq(additionalOperations, other.additionalOperations) &&
         servers == other.servers && parameters == other.parameters &&
         extensions == other.extensions;
}

/// @brief Deep value equality comparison for Components.
/// @param other The Components to compare against.
/// @return True if all component maps and shared pointers are equal by value.
inline bool Components::operator==(const Components &other) const noexcept {
  return schemas == other.schemas && responses == other.responses &&
         parameters == other.parameters && examples == other.examples &&
         requestBodies == other.requestBodies && headers == other.headers &&
         securitySchemes == other.securitySchemes && links == other.links &&
         deep_eq(callbacks, other.callbacks) && pathItems == other.pathItems &&
         deep_eq(mediaTypes, other.mediaTypes) &&
         extensions == other.extensions;
}

} // namespace cdd_cpp::openapi
