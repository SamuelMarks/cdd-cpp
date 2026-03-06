#include "parse.hpp"
#include <iostream>
#include <stdexcept>

namespace cdd_cpp::openapi {

static std::optional<std::string> get_optional_string(simdjson::dom::object obj,
                                                      std::string_view key) {
  simdjson::dom::element el;
  if (obj[key].get(el) == simdjson::SUCCESS &&
      el.type() == simdjson::dom::element_type::STRING) {
    return std::string(el.get_string().value());
  }
  return std::nullopt;
}

static std::optional<bool> get_optional_bool(simdjson::dom::object obj,
                                             std::string_view key) {
  simdjson::dom::element el;
  if (obj[key].get(el) == simdjson::SUCCESS &&
      el.type() == simdjson::dom::element_type::BOOL) {
    return el.get_bool().value();
  }
  return std::nullopt;
}

static std::optional<std::vector<std::string>>
parse_vector_string(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element el;
  if (obj[key].get(el) == simdjson::SUCCESS &&
      el.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<std::string> vec;
    for (auto item : el.get_array()) {
      if (item.type() == simdjson::dom::element_type::STRING)
        vec.push_back(std::string(item.get_string().value()));
    }
    return vec;
  }
  return std::nullopt;
}

static std::optional<std::map<std::string, std::string>>
parse_map_string(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element el;
  if (obj[key].get(el) == simdjson::SUCCESS &&
      el.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, std::string> m;
    for (auto field : el.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::STRING)
        m[std::string(field.key)] =
            std::string(field.value.get_string().value());
    }
    return m;
  }
  return std::nullopt;
}

static std::optional<std::vector<SecurityRequirement>>
parse_security(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element sec_req_el;
  if (obj[key].get(sec_req_el) == simdjson::SUCCESS &&
      sec_req_el.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<SecurityRequirement> security;
    for (auto item : sec_req_el.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT) {
        SecurityRequirement req;
        for (auto field : item.get_object()) {
          std::vector<std::string> scopes;
          if (field.value.type() == simdjson::dom::element_type::ARRAY) {
            for (auto scope : field.value.get_array()) {
              if (scope.type() == simdjson::dom::element_type::STRING)
                scopes.push_back(std::string(scope.get_string().value()));
            }
          }
          req[std::string(field.key)] = scopes;
        }
        security.push_back(req);
      }
    }
    return security;
  }
  return std::nullopt;
}

static Reference parse_Reference(simdjson::dom::object obj) noexcept;
static Contact parse_Contact(simdjson::dom::object obj) noexcept;
static License parse_License(simdjson::dom::object obj) noexcept;
static Info parse_Info(simdjson::dom::object obj) noexcept;
static ServerVariable parse_ServerVariable(simdjson::dom::object obj) noexcept;
static Server parse_Server(simdjson::dom::object obj) noexcept;
static ExternalDocumentation
parse_ExternalDocumentation(simdjson::dom::object obj) noexcept;
static Tag parse_Tag(simdjson::dom::object obj) noexcept;
static Discriminator parse_Discriminator(simdjson::dom::object obj) noexcept;
static XML parse_XML(simdjson::dom::object obj) noexcept;
static Schema parse_Schema(simdjson::dom::object obj) noexcept;
static Example parse_Example(simdjson::dom::object obj) noexcept;
static Encoding parse_Encoding(simdjson::dom::object obj) noexcept;
static MediaType parse_MediaType(simdjson::dom::object obj) noexcept;
static Header parse_Header(simdjson::dom::object obj) noexcept;
static RequestBody parse_RequestBody(simdjson::dom::object obj) noexcept;
static Link parse_Link(simdjson::dom::object obj) noexcept;
static Response parse_Response(simdjson::dom::object obj) noexcept;
static Parameter parse_Parameter(simdjson::dom::object obj) noexcept;
static OAuthFlow parse_OAuthFlow(simdjson::dom::object obj) noexcept;
static OAuthFlows parse_OAuthFlows(simdjson::dom::object obj) noexcept;
static SecurityScheme parse_SecurityScheme(simdjson::dom::object obj) noexcept;
static Operation parse_Operation(simdjson::dom::object obj) noexcept;
static PathItem parse_PathItem(simdjson::dom::object obj) noexcept;
static Components parse_Components(simdjson::dom::object obj) noexcept;
static OpenAPI parse_OpenAPI(simdjson::dom::object obj) noexcept;

static Reference parse_Reference(simdjson::dom::object obj) noexcept {
  Reference res;
  res.ref = get_optional_string(obj, "$ref").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  return res;
}

static Contact parse_Contact(simdjson::dom::object obj) noexcept {
  Contact res;
  res.name = get_optional_string(obj, "name");
  res.url = get_optional_string(obj, "url");
  res.email = get_optional_string(obj, "email");
  return res;
}

static License parse_License(simdjson::dom::object obj) noexcept {
  License res;
  res.name = get_optional_string(obj, "name").value_or("");
  res.identifier = get_optional_string(obj, "identifier");
  res.url = get_optional_string(obj, "url");
  return res;
}

static Info parse_Info(simdjson::dom::object obj) noexcept {
  Info res;
  res.title = get_optional_string(obj, "title").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  res.termsOfService = get_optional_string(obj, "termsOfService");
  simdjson::dom::element el_contact;
  if (obj["contact"].get(el_contact) == simdjson::SUCCESS &&
      el_contact.type() == simdjson::dom::element_type::OBJECT) {
    res.contact = parse_Contact(el_contact.get_object());
  }
  simdjson::dom::element el_license;
  if (obj["license"].get(el_license) == simdjson::SUCCESS &&
      el_license.type() == simdjson::dom::element_type::OBJECT) {
    res.license = parse_License(el_license.get_object());
  }
  res.version = get_optional_string(obj, "version").value_or("");
  return res;
}

static ServerVariable parse_ServerVariable(simdjson::dom::object obj) noexcept {
  ServerVariable res;
  res.enum_values = parse_vector_string(obj, "enum");
  res.default_value = get_optional_string(obj, "default").value_or("");
  res.description = get_optional_string(obj, "description");
  return res;
}

static Server parse_Server(simdjson::dom::object obj) noexcept {
  Server res;
  res.url = get_optional_string(obj, "url").value_or("");
  res.description = get_optional_string(obj, "description");
  res.name = get_optional_string(obj, "name");
  simdjson::dom::element el_variables;
  if (obj["variables"].get(el_variables) == simdjson::SUCCESS &&
      el_variables.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, ServerVariable> m;
    for (auto field : el_variables.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] =
            parse_ServerVariable(field.value.get_object());
    }
    res.variables = m;
  }
  return res;
}

static ExternalDocumentation
parse_ExternalDocumentation(simdjson::dom::object obj) noexcept {
  ExternalDocumentation res;
  res.description = get_optional_string(obj, "description");
  res.url = get_optional_string(obj, "url").value_or("");
  return res;
}

static Tag parse_Tag(simdjson::dom::object obj) noexcept {
  Tag res;
  res.name = get_optional_string(obj, "name").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  res.parent = get_optional_string(obj, "parent");
  res.kind = get_optional_string(obj, "kind");
  simdjson::dom::element el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS &&
      el_externalDocs.type() == simdjson::dom::element_type::OBJECT) {
    res.externalDocs =
        parse_ExternalDocumentation(el_externalDocs.get_object());
  }
  return res;
}

static Discriminator parse_Discriminator(simdjson::dom::object obj) noexcept {
  Discriminator res;
  res.propertyName = get_optional_string(obj, "propertyName").value_or("");
  res.mapping = parse_map_string(obj, "mapping");
  res.defaultMapping = get_optional_string(obj, "defaultMapping");
  return res;
}

static XML parse_XML(simdjson::dom::object obj) noexcept {
  XML res;
  res.name = get_optional_string(obj, "name");
  res.namespace_url = get_optional_string(obj, "namespace");
  res.prefix = get_optional_string(obj, "prefix");
  res.attribute = get_optional_bool(obj, "attribute");
  res.wrapped = get_optional_bool(obj, "wrapped");
  return res;
}

static Schema parse_Schema(simdjson::dom::object obj) noexcept {
  Schema res;
  res.type = get_optional_string(obj, "type");
  res.description = get_optional_string(obj, "description");
  res.format = get_optional_string(obj, "format");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  simdjson::dom::element el_discriminator;
  if (obj["discriminator"].get(el_discriminator) == simdjson::SUCCESS &&
      el_discriminator.type() == simdjson::dom::element_type::OBJECT) {
    res.discriminator = parse_Discriminator(el_discriminator.get_object());
  }
  simdjson::dom::element el_xml;
  if (obj["xml"].get(el_xml) == simdjson::SUCCESS &&
      el_xml.type() == simdjson::dom::element_type::OBJECT) {
    res.xml = parse_XML(el_xml.get_object());
  }
  simdjson::dom::element el_properties;
  if (obj["properties"].get(el_properties) == simdjson::SUCCESS &&
      el_properties.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, Schema>>();
    for (auto field : el_properties.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] = parse_Schema(field.value.get_object());
    }
    res.properties = m;
  }
  simdjson::dom::element el_items;
  if (obj["items"].get(el_items) == simdjson::SUCCESS &&
      el_items.type() == simdjson::dom::element_type::OBJECT) {
    res.items = std::make_shared<Schema>(parse_Schema(el_items.get_object()));
  }
  res.schema_dialect = get_optional_string(obj, "$schema");
  res.id = get_optional_string(obj, "$id");
  res.anchor = get_optional_string(obj, "$anchor");
  res.dynamicAnchor = get_optional_string(obj, "$dynamicAnchor");
  res.dynamicRef = get_optional_string(obj, "$dynamicRef");
  res.comment = get_optional_string(obj, "$comment");
  res.title = get_optional_string(obj, "title");
  res.deprecated = get_optional_bool(obj, "deprecated");
  res.readOnly = get_optional_bool(obj, "readOnly");
  res.writeOnly = get_optional_bool(obj, "writeOnly");

  simdjson::dom::element el_defs;
  if (obj["$defs"].get(el_defs) == simdjson::SUCCESS &&
      el_defs.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, Schema>>();
    for (auto field : el_defs.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] = parse_Schema(field.value.get_object());
    }
    res.defs = m;
  }

  res.multipleOf = get_optional_bool(obj, "multipleOf"); // wait it's double
  simdjson::dom::element el_double;
  if (obj["multipleOf"].get(el_double) == simdjson::SUCCESS &&
      el_double.type() == simdjson::dom::element_type::DOUBLE) {
    res.multipleOf = el_double.get_double().value();
  }
  if (obj["maximum"].get(el_double) == simdjson::SUCCESS &&
      el_double.type() == simdjson::dom::element_type::DOUBLE) {
    res.maximum = el_double.get_double().value();
  }
  if (obj["exclusiveMaximum"].get(el_double) == simdjson::SUCCESS &&
      el_double.type() == simdjson::dom::element_type::DOUBLE) {
    res.exclusiveMaximum = el_double.get_double().value();
  }
  if (obj["minimum"].get(el_double) == simdjson::SUCCESS &&
      el_double.type() == simdjson::dom::element_type::DOUBLE) {
    res.minimum = el_double.get_double().value();
  }
  if (obj["exclusiveMinimum"].get(el_double) == simdjson::SUCCESS &&
      el_double.type() == simdjson::dom::element_type::DOUBLE) {
    res.exclusiveMinimum = el_double.get_double().value();
  }

  simdjson::dom::element el_int;
  if (obj["maxLength"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.maxLength = el_int.get_int64().value();
  }
  if (obj["minLength"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.minLength = el_int.get_int64().value();
  }
  if (obj["maxItems"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.maxItems = el_int.get_int64().value();
  }
  if (obj["minItems"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.minItems = el_int.get_int64().value();
  }
  if (obj["maxContains"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.maxContains = el_int.get_int64().value();
  }
  if (obj["minContains"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.minContains = el_int.get_int64().value();
  }
  if (obj["maxProperties"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.maxProperties = el_int.get_int64().value();
  }
  if (obj["minProperties"].get(el_int) == simdjson::SUCCESS &&
      el_int.type() == simdjson::dom::element_type::INT64) {
    res.minProperties = el_int.get_int64().value();
  }

  res.pattern = get_optional_string(obj, "pattern");
  res.uniqueItems = get_optional_bool(obj, "uniqueItems");
  res.required = parse_vector_string(obj, "required");

  return res;
}

static Example parse_Example(simdjson::dom::object obj) noexcept {
  Example res;
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  res.value = get_optional_string(obj, "value");
  res.externalValue = get_optional_string(obj, "externalValue");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static Encoding parse_Encoding(simdjson::dom::object obj) noexcept {
  Encoding res;
  res.contentType = get_optional_string(obj, "contentType");
  res.style = get_optional_string(obj, "style");
  res.explode = get_optional_bool(obj, "explode");
  res.allowReserved = get_optional_bool(obj, "allowReserved");
  simdjson::dom::element el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS &&
      el_headers.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, Header>>();
    for (auto field : el_headers.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] = parse_Header(field.value.get_object());
    }
    res.headers = m;
  }

  simdjson::dom::element el_itemSchema;
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS &&
      el_itemSchema.type() == simdjson::dom::element_type::OBJECT) {
    res.itemSchema =
        std::make_shared<Schema>(parse_Schema(el_itemSchema.get_object()));
  }

  simdjson::dom::element el_itemEncoding;
  if (obj["itemEncoding"].get(el_itemEncoding) == simdjson::SUCCESS &&
      el_itemEncoding.type() == simdjson::dom::element_type::OBJECT) {
    res.itemEncoding = std::make_shared<Encoding>(
        parse_Encoding(el_itemEncoding.get_object()));
  }

  simdjson::dom::element el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS &&
      el_prefixEncoding.type() == simdjson::dom::element_type::ARRAY) {
    auto v = std::make_shared<std::vector<Encoding>>();
    for (auto item : el_prefixEncoding.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        v->push_back(parse_Encoding(item.get_object()));
    }
    res.prefixEncoding = v;
  }

  return res;
}

static MediaType parse_MediaType(simdjson::dom::object obj) noexcept {
  MediaType res;
  simdjson::dom::element el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS &&
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object());
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::element el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Example> m;
    for (auto field : el_examples.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Example(field.value.get_object());
    }
    res.examples = m;
  }
  simdjson::dom::element el_encoding;
  if (obj["encoding"].get(el_encoding) == simdjson::SUCCESS &&
      el_encoding.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, Encoding>>();
    for (auto field : el_encoding.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] = parse_Encoding(field.value.get_object());
    }
    res.encoding = m;
  }

  simdjson::dom::element el_itemSchema;
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS &&
      el_itemSchema.type() == simdjson::dom::element_type::OBJECT) {
    res.itemSchema =
        std::make_shared<Schema>(parse_Schema(el_itemSchema.get_object()));
  }

  simdjson::dom::element el_itemEncoding;
  if (obj["itemEncoding"].get(el_itemEncoding) == simdjson::SUCCESS &&
      el_itemEncoding.type() == simdjson::dom::element_type::OBJECT) {
    res.itemEncoding = std::make_shared<Encoding>(
        parse_Encoding(el_itemEncoding.get_object()));
  }

  simdjson::dom::element el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS &&
      el_prefixEncoding.type() == simdjson::dom::element_type::ARRAY) {
    auto v = std::make_shared<std::vector<Encoding>>();
    for (auto item : el_prefixEncoding.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        v->push_back(parse_Encoding(item.get_object()));
    }
    res.prefixEncoding = v;
  }

  return res;
}

static Header parse_Header(simdjson::dom::object obj) noexcept {
  Header res;
  res.description = get_optional_string(obj, "description");
  res.required = get_optional_bool(obj, "required").value_or(false);
  res.deprecated = get_optional_bool(obj, "deprecated").value_or(false);
  res.allowEmptyValue =
      get_optional_bool(obj, "allowEmptyValue").value_or(false);
  res.style = get_optional_string(obj, "style");
  res.explode = get_optional_bool(obj, "explode");
  res.allowReserved = get_optional_bool(obj, "allowReserved");
  simdjson::dom::element el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS &&
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object());
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::element el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Example> m;
    for (auto field : el_examples.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Example(field.value.get_object());
    }
    res.examples = m;
  }
  simdjson::dom::element el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS &&
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_MediaType(field.value.get_object());
    }
    res.content = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static RequestBody parse_RequestBody(simdjson::dom::object obj) noexcept {
  RequestBody res;
  res.description = get_optional_string(obj, "description");
  simdjson::dom::element el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS &&
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_MediaType(field.value.get_object());
    }
    res.content = m;
  }
  res.required = get_optional_bool(obj, "required").value_or(false);
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static Link parse_Link(simdjson::dom::object obj) noexcept {
  Link res;
  res.operationRef = get_optional_string(obj, "operationRef");
  res.operationId = get_optional_string(obj, "operationId");
  res.parameters = parse_map_string(obj, "parameters");
  res.requestBody = get_optional_string(obj, "requestBody");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::element el_server;
  if (obj["server"].get(el_server) == simdjson::SUCCESS &&
      el_server.type() == simdjson::dom::element_type::OBJECT) {
    res.server = parse_Server(el_server.get_object());
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static Response parse_Response(simdjson::dom::object obj) noexcept {
  Response res;
  res.description = get_optional_string(obj, "description").value_or("");
  simdjson::dom::element el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS &&
      el_headers.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Header> m;
    for (auto field : el_headers.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Header(field.value.get_object());
    }
    res.headers = m;
  }
  simdjson::dom::element el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS &&
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_MediaType(field.value.get_object());
    }
    res.content = m;
  }
  simdjson::dom::element el_links;
  if (obj["links"].get(el_links) == simdjson::SUCCESS &&
      el_links.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Link> m;
    for (auto field : el_links.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Link(field.value.get_object());
    }
    res.links = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static Parameter parse_Parameter(simdjson::dom::object obj) noexcept {
  Parameter res;
  res.name = get_optional_string(obj, "name").value_or("");
  res.in = get_optional_string(obj, "in").value_or("");
  res.description = get_optional_string(obj, "description");
  res.required = get_optional_bool(obj, "required").value_or(false);
  res.deprecated = get_optional_bool(obj, "deprecated").value_or(false);
  res.allowEmptyValue =
      get_optional_bool(obj, "allowEmptyValue").value_or(false);
  res.style = get_optional_string(obj, "style");
  res.explode = get_optional_bool(obj, "explode");
  res.allowReserved = get_optional_bool(obj, "allowReserved");
  simdjson::dom::element el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS &&
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object());
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::element el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Example> m;
    for (auto field : el_examples.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Example(field.value.get_object());
    }
    res.examples = m;
  }
  simdjson::dom::element el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS &&
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_MediaType(field.value.get_object());
    }
    res.content = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static OAuthFlow parse_OAuthFlow(simdjson::dom::object obj) noexcept {
  OAuthFlow res;
  res.authorizationUrl = get_optional_string(obj, "authorizationUrl");
  res.tokenUrl = get_optional_string(obj, "tokenUrl");
  res.refreshUrl = get_optional_string(obj, "refreshUrl");
  res.deviceAuthorizationUrl = get_optional_string(obj, "deviceAuthorizationUrl");
  res.scopes = parse_map_string(obj, "scopes");
  return res;
}

static OAuthFlows parse_OAuthFlows(simdjson::dom::object obj) noexcept {
  OAuthFlows res;
  simdjson::dom::element el_implicit;
  if (obj["implicit"].get(el_implicit) == simdjson::SUCCESS &&
      el_implicit.type() == simdjson::dom::element_type::OBJECT) {
    res.implicit = parse_OAuthFlow(el_implicit.get_object());
  }
  simdjson::dom::element el_password;
  if (obj["password"].get(el_password) == simdjson::SUCCESS &&
      el_password.type() == simdjson::dom::element_type::OBJECT) {
    res.password = parse_OAuthFlow(el_password.get_object());
  }
  simdjson::dom::element el_clientCredentials;
  if (obj["clientCredentials"].get(el_clientCredentials) == simdjson::SUCCESS &&
      el_clientCredentials.type() == simdjson::dom::element_type::OBJECT) {
    res.clientCredentials = parse_OAuthFlow(el_clientCredentials.get_object());
  }
  simdjson::dom::element el_authorizationCode;
  if (obj["authorizationCode"].get(el_authorizationCode) == simdjson::SUCCESS &&
      el_authorizationCode.type() == simdjson::dom::element_type::OBJECT) {
    res.authorizationCode = parse_OAuthFlow(el_authorizationCode.get_object());
  }
  simdjson::dom::element el_deviceAuthorization;
  if (obj["deviceAuthorization"].get(el_deviceAuthorization) == simdjson::SUCCESS &&
      el_deviceAuthorization.type() == simdjson::dom::element_type::OBJECT) {
    res.deviceAuthorization = parse_OAuthFlow(el_deviceAuthorization.get_object());
  }
  return res;
}

static SecurityScheme parse_SecurityScheme(simdjson::dom::object obj) noexcept {
  SecurityScheme res;
  res.type = get_optional_string(obj, "type").value_or("");
  res.description = get_optional_string(obj, "description");
  res.name = get_optional_string(obj, "name");
  res.in = get_optional_string(obj, "in");
  res.scheme = get_optional_string(obj, "scheme");
  res.bearerFormat = get_optional_string(obj, "bearerFormat");
  simdjson::dom::element el_flows;
  if (obj["flows"].get(el_flows) == simdjson::SUCCESS &&
      el_flows.type() == simdjson::dom::element_type::OBJECT) {
    res.flows = parse_OAuthFlows(el_flows.get_object());
  }
  res.openIdConnectUrl = get_optional_string(obj, "openIdConnectUrl");
  res.oauth2MetadataUrl = get_optional_string(obj, "oauth2MetadataUrl");
  res.deprecated = get_optional_bool(obj, "deprecated");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS &&
      el_ref.type() == simdjson::dom::element_type::OBJECT) {
    res.ref = parse_Reference(el_ref.get_object());
  }
  return res;
}

static Operation parse_Operation(simdjson::dom::object obj) noexcept {
  Operation res;
  res.tags = parse_vector_string(obj, "tags");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::element el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS &&
      el_externalDocs.type() == simdjson::dom::element_type::OBJECT) {
    res.externalDocs =
        parse_ExternalDocumentation(el_externalDocs.get_object());
  }
  res.operationId = get_optional_string(obj, "operationId");
  simdjson::dom::element el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS &&
      el_parameters.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Parameter> vec;
    for (auto item : el_parameters.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Parameter(item.get_object()));
    }
    res.parameters = vec;
  }
  simdjson::dom::element el_requestBody;
  if (obj["requestBody"].get(el_requestBody) == simdjson::SUCCESS &&
      el_requestBody.type() == simdjson::dom::element_type::OBJECT) {
    res.requestBody = parse_RequestBody(el_requestBody.get_object());
  }
  simdjson::dom::element el_responses;
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS &&
      el_responses.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Response> m;
    for (auto field : el_responses.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Response(field.value.get_object());
    }
    res.responses = m;
  }
  simdjson::dom::element el_callbacks;
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS &&
      el_callbacks.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<
        std::map<std::string, std::map<std::string, PathItem>>>();
    for (auto field : el_callbacks.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT) {
        std::map<std::string, PathItem> inner_m;
        for (auto inner_field : field.value.get_object()) {
          if (inner_field.value.type() == simdjson::dom::element_type::OBJECT)
            inner_m[std::string(inner_field.key)] =
                parse_PathItem(inner_field.value.get_object());
        }
        (*m)[std::string(field.key)] = inner_m;
      }
    }
    res.callbacks = m;
  }
  res.deprecated = get_optional_bool(obj, "deprecated").value_or(false);
  res.security = parse_security(obj, "security");
  simdjson::dom::element el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Server> vec;
    for (auto item : el_servers.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Server(item.get_object()));
    }
    res.servers = vec;
  }
  return res;
}

static PathItem parse_PathItem(simdjson::dom::object obj) noexcept {
  PathItem res;
  res.ref = get_optional_string(obj, "$ref");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::element el_get;
  if (obj["get"].get(el_get) == simdjson::SUCCESS &&
      el_get.type() == simdjson::dom::element_type::OBJECT) {
    res.get = parse_Operation(el_get.get_object());
  }
  simdjson::dom::element el_put;
  if (obj["put"].get(el_put) == simdjson::SUCCESS &&
      el_put.type() == simdjson::dom::element_type::OBJECT) {
    res.put = parse_Operation(el_put.get_object());
  }
  simdjson::dom::element el_post;
  if (obj["post"].get(el_post) == simdjson::SUCCESS &&
      el_post.type() == simdjson::dom::element_type::OBJECT) {
    res.post = parse_Operation(el_post.get_object());
  }
  simdjson::dom::element el_delete_op;
  if (obj["delete"].get(el_delete_op) == simdjson::SUCCESS &&
      el_delete_op.type() == simdjson::dom::element_type::OBJECT) {
    res.delete_op = parse_Operation(el_delete_op.get_object());
  }
  simdjson::dom::element el_options;
  if (obj["options"].get(el_options) == simdjson::SUCCESS &&
      el_options.type() == simdjson::dom::element_type::OBJECT) {
    res.options = parse_Operation(el_options.get_object());
  }
  simdjson::dom::element el_head;
  if (obj["head"].get(el_head) == simdjson::SUCCESS &&
      el_head.type() == simdjson::dom::element_type::OBJECT) {
    res.head = parse_Operation(el_head.get_object());
  }
  simdjson::dom::element el_patch;
  if (obj["patch"].get(el_patch) == simdjson::SUCCESS &&
      el_patch.type() == simdjson::dom::element_type::OBJECT) {
    res.patch = parse_Operation(el_patch.get_object());
  }
  simdjson::dom::element el_trace;
  if (obj["trace"].get(el_trace) == simdjson::SUCCESS &&
      el_trace.type() == simdjson::dom::element_type::OBJECT) {
    res.trace = parse_Operation(el_trace.get_object());
  }
  simdjson::dom::element el_query;
  if (obj["query"].get(el_query) == simdjson::SUCCESS &&
      el_query.type() == simdjson::dom::element_type::OBJECT) {
    res.query = parse_Operation(el_query.get_object());
  }
  simdjson::dom::element el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Server> vec;
    for (auto item : el_servers.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Server(item.get_object()));
    }
    res.servers = vec;
  }
  simdjson::dom::element el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS &&
      el_parameters.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Parameter> vec;
    for (auto item : el_parameters.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Parameter(item.get_object()));
    }
    res.parameters = vec;
  }

  simdjson::dom::element el_addOps;
  if (obj["additionalOperations"].get(el_addOps) == simdjson::SUCCESS &&
      el_addOps.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, Operation>>();
    for (auto field : el_addOps.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] =
            parse_Operation(field.value.get_object());
    }
    res.additionalOperations = m;
  }

  return res;
}

static Components parse_Components(simdjson::dom::object obj) noexcept {
  Components res;
  simdjson::dom::element el_schemas;
  if (obj["schemas"].get(el_schemas) == simdjson::SUCCESS &&
      el_schemas.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Schema> m;
    for (auto field : el_schemas.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Schema(field.value.get_object());
    }
    res.schemas = m;
  }
  simdjson::dom::element el_responses;
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS &&
      el_responses.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Response> m;
    for (auto field : el_responses.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Response(field.value.get_object());
    }
    res.responses = m;
  }
  simdjson::dom::element el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS &&
      el_parameters.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Parameter> m;
    for (auto field : el_parameters.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Parameter(field.value.get_object());
    }
    res.parameters = m;
  }
  simdjson::dom::element el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Example> m;
    for (auto field : el_examples.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Example(field.value.get_object());
    }
    res.examples = m;
  }
  simdjson::dom::element el_requestBodies;
  if (obj["requestBodies"].get(el_requestBodies) == simdjson::SUCCESS &&
      el_requestBodies.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, RequestBody> m;
    for (auto field : el_requestBodies.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_RequestBody(field.value.get_object());
    }
    res.requestBodies = m;
  }
  simdjson::dom::element el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS &&
      el_headers.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Header> m;
    for (auto field : el_headers.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Header(field.value.get_object());
    }
    res.headers = m;
  }
  simdjson::dom::element el_securitySchemes;
  if (obj["securitySchemes"].get(el_securitySchemes) == simdjson::SUCCESS &&
      el_securitySchemes.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, SecurityScheme> m;
    for (auto field : el_securitySchemes.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] =
            parse_SecurityScheme(field.value.get_object());
    }
    res.securitySchemes = m;
  }
  simdjson::dom::element el_links;
  if (obj["links"].get(el_links) == simdjson::SUCCESS &&
      el_links.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Link> m;
    for (auto field : el_links.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_Link(field.value.get_object());
    }
    res.links = m;
  }
  simdjson::dom::element el_callbacks;
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS &&
      el_callbacks.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<
        std::map<std::string, std::map<std::string, PathItem>>>();
    for (auto field : el_callbacks.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT) {
        std::map<std::string, PathItem> inner_m;
        for (auto inner_field : field.value.get_object()) {
          if (inner_field.value.type() == simdjson::dom::element_type::OBJECT)
            inner_m[std::string(inner_field.key)] =
                parse_PathItem(inner_field.value.get_object());
        }
        (*m)[std::string(field.key)] = inner_m;
      }
    }
    res.callbacks = m;
  }
  simdjson::dom::element el_pathItems;
  if (obj["pathItems"].get(el_pathItems) == simdjson::SUCCESS &&
      el_pathItems.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, PathItem> m;
    for (auto field : el_pathItems.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_PathItem(field.value.get_object());
    }
    res.pathItems = m;
  }

  simdjson::dom::element el_mediaTypes;
  if (obj["mediaTypes"].get(el_mediaTypes) == simdjson::SUCCESS &&
      el_mediaTypes.type() == simdjson::dom::element_type::OBJECT) {
    auto m = std::make_shared<std::map<std::string, MediaType>>();
    for (auto field : el_mediaTypes.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        (*m)[std::string(field.key)] =
            parse_MediaType(field.value.get_object());
    }
    res.mediaTypes = m;
  }

  return res;
}

static OpenAPI parse_OpenAPI(simdjson::dom::object obj) noexcept {
  OpenAPI res;
  res.openapi = get_optional_string(obj, "openapi").value_or("");
  res.self_link = get_optional_string(obj, "$self");
  simdjson::dom::element el_info;
  if (obj["info"].get(el_info) == simdjson::SUCCESS &&
      el_info.type() == simdjson::dom::element_type::OBJECT) {
    res.info = parse_Info(el_info.get_object());
  }
  res.jsonSchemaDialect = get_optional_string(obj, "jsonSchemaDialect");
  simdjson::dom::element el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Server> vec;
    for (auto item : el_servers.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Server(item.get_object()));
    }
    res.servers = vec;
  }
  simdjson::dom::element el_paths;
  if (obj["paths"].get(el_paths) == simdjson::SUCCESS &&
      el_paths.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, PathItem> m;
    for (auto field : el_paths.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_PathItem(field.value.get_object());
    }
    res.paths = m;
  }
  simdjson::dom::element el_webhooks;
  if (obj["webhooks"].get(el_webhooks) == simdjson::SUCCESS &&
      el_webhooks.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, PathItem> m;
    for (auto field : el_webhooks.get_object()) {
      if (field.value.type() == simdjson::dom::element_type::OBJECT)
        m[std::string(field.key)] = parse_PathItem(field.value.get_object());
    }
    res.webhooks = m;
  }
  simdjson::dom::element el_components;
  if (obj["components"].get(el_components) == simdjson::SUCCESS &&
      el_components.type() == simdjson::dom::element_type::OBJECT) {
    res.components = parse_Components(el_components.get_object());
  }
  res.security = parse_security(obj, "security");
  simdjson::dom::element el_tags;
  if (obj["tags"].get(el_tags) == simdjson::SUCCESS &&
      el_tags.type() == simdjson::dom::element_type::ARRAY) {
    std::vector<Tag> vec;
    for (auto item : el_tags.get_array()) {
      if (item.type() == simdjson::dom::element_type::OBJECT)
        vec.push_back(parse_Tag(item.get_object()));
    }
    res.tags = vec;
  }
  simdjson::dom::element el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS &&
      el_externalDocs.type() == simdjson::dom::element_type::OBJECT) {
    res.externalDocs =
        parse_ExternalDocumentation(el_externalDocs.get_object());
  }
  return res;
}

std::expected<OpenAPI, std::string> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc = parser.parse(input);
  if (doc.type() != simdjson::dom::element_type::OBJECT)
    return std::unexpected("OpenAPI doc must be an object");
  return parse_OpenAPI(doc.get_object());
}
} // namespace cdd_cpp::openapi
