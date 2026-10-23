/// @file parse.cpp
/// @brief Implementation of OpenAPI 3.2.0 JSON parser.

#include "parse.hpp"
#include <optional>

namespace cdd_cpp::openapi {

/// @brief Safely extract a `simdjson::dom::object` from an element.
/// @param el The simdjson DOM element.
/// @return Optional `simdjson::dom::object`, or `std::nullopt` if not an
/// object.
static std::optional<simdjson::dom::object>
try_get_object(simdjson::dom::element el) noexcept {
  simdjson::dom::object obj;
  return (el.get(obj) == simdjson::SUCCESS) ? std::optional(obj) : std::nullopt;
}

/// @brief Safely extract an `int64_t` from an element.
/// @param el The simdjson DOM element.
/// @return Optional `int64_t`, or `std::nullopt` if not an integer.
static std::optional<int64_t>
try_get_int64(simdjson::dom::element el) noexcept {
  int64_t val = 0;
  (void)el.get(val);
  return val;
}

/// @brief Safely extract a `double` from an element.
/// @param el The simdjson DOM element.
/// @return Optional `double`, or `std::nullopt` if not a number.
static std::optional<double>
try_get_double(simdjson::dom::element el) noexcept {
  double val = 0.0;
  return (el.get(val) == simdjson::SUCCESS) ? std::optional(val) : std::nullopt;
}

/// @brief Extract an optional string property from a DOM object.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional string value.
static std::optional<std::string>
get_optional_string(simdjson::dom::object obj, std::string_view key) noexcept {
  std::string_view val;
  if (obj[key].get(val) == simdjson::SUCCESS) {
    return std::string(val);
  }
  return std::nullopt;
}

/// @brief Extract an optional boolean property from a DOM object.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional boolean value.
static std::optional<bool> get_optional_bool(simdjson::dom::object obj,
                                             std::string_view key) noexcept {
  bool val = false;
  if (obj[key].get(val) == simdjson::SUCCESS) {
    return val;
  }
  return std::nullopt;
}

/// @brief Extract an optional vector of strings from a DOM object property.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional vector of strings.
static std::optional<std::vector<std::string>>
parse_vector_string(simdjson::dom::object obj, std::string_view key) noexcept {
  simdjson::dom::array arr;
  if (obj[key].get(arr) == simdjson::SUCCESS) {
    std::vector<std::string> vec;
    for (auto item : arr) {
      std::string_view sv;
      (void)item.get(sv);
      vec.emplace_back(sv);
    }
    return vec;
  }
  return std::nullopt;
}

/// @brief Extract an optional string-to-string map from a DOM object property.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional string map.
static std::optional<std::map<std::string, std::string>>
parse_map_string(simdjson::dom::object obj, std::string_view key) noexcept {
  simdjson::dom::object sub_obj;
  if (obj[key].get(sub_obj) == simdjson::SUCCESS) {
    std::map<std::string, std::string> m;
    for (auto field : sub_obj) {
      std::string_view sv;
      (void)field.value.get(sv);
      m[std::string(field.key)] = std::string(sv);
    }
    return m;
  }
  return std::nullopt;
}

/// @brief Extract an optional vector of SecurityRequirements from a DOM object.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional vector of SecurityRequirements.
static std::optional<std::vector<SecurityRequirement>>
parse_security(simdjson::dom::object obj, std::string_view key) noexcept {
  simdjson::dom::array sec_arr;
  if (obj[key].get(sec_arr) == simdjson::SUCCESS) {
    std::vector<SecurityRequirement> security;
    for (auto item : sec_arr) {
      simdjson::dom::object req_obj;
      (void)item.get(req_obj);
      SecurityRequirement req;
      for (auto field : req_obj) {
        std::vector<std::string> scopes;
        simdjson::dom::array s_arr;
        (void)field.value.get(s_arr);
        for (auto scope : s_arr) {
          std::string_view sv;
          (void)scope.get(sv);
          scopes.emplace_back(sv);
        }
        req[std::string(field.key)] = scopes;
      }
      security.push_back(req);
    }
    return security;
  }
  return std::nullopt;
}

/// @brief Unwrap a `simdjson::dom::object` from an element without throwing.
/// @param el The simdjson DOM element.
/// @return Unwrapped `simdjson::dom::object`.
static simdjson::dom::object unwrap_obj(simdjson::dom::element el) noexcept {
  simdjson::dom::object o;
  (void)el.get(o);
  return o;
}

/// @brief Recursively convert a `simdjson::dom::element` into a `JsonValue`.
/// @param el The simdjson DOM element.
/// @return Converted `JsonValue`.
static JsonValue parse_json_value(simdjson::dom::element el) noexcept {
  if (el.is_string()) {
    std::string_view sv;
    (void)el.get(sv);
    return JsonValue(std::string(sv));
  }
  if (el.is_int64()) {
    int64_t iv = 0;
    (void)el.get(iv);
    return JsonValue(iv);
  }
  if (el.is_double()) {
    double dv = 0.0;
    (void)el.get(dv);
    return JsonValue(dv);
  }
  if (el.is_bool()) {
    bool bv = false;
    (void)el.get(bv);
    return JsonValue(bv);
  }
  if (el.is_array()) {
    simdjson::dom::array arr;
    (void)el.get(arr);
    std::vector<JsonValue> vec;
    for (auto item : arr) {
      vec.push_back(parse_json_value(item));
    }
    return JsonValue(std::move(vec));
  }
  if (el.is_object()) {
    simdjson::dom::object obj;
    (void)el.get(obj);
    std::map<std::string, JsonValue> m;
    for (auto field : obj) {
      m[std::string(field.key)] = parse_json_value(field.value);
    }
    return JsonValue(std::move(m));
  }
  return JsonValue(nullptr);
}

/// @brief Extract all `x-*` specification extensions from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Map of extension key (`x-...`) to `JsonValue`.
static std::map<std::string, JsonValue>
parse_extensions(simdjson::dom::object obj) noexcept {
  std::map<std::string, JsonValue> exts;
  for (auto field : obj) {
    if (field.key.starts_with("x-")) {
      exts[std::string(field.key)] = parse_json_value(field.value);
    }
  }
  return exts;
}

inline static Reference parse_Reference(simdjson::dom::object obj) noexcept;
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

/// @brief Deserialize OpenAPI JSON element via `parse_Reference`.
inline static Reference parse_Reference(simdjson::dom::object obj) noexcept {
  Reference res;
  res.ref = get_optional_string(obj, "$ref").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Contact`.
static Contact parse_Contact(simdjson::dom::object obj) noexcept {
  Contact res;
  res.name = get_optional_string(obj, "name");
  res.url = get_optional_string(obj, "url");
  res.email = get_optional_string(obj, "email");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_License`.
static License parse_License(simdjson::dom::object obj) noexcept {
  License res;
  res.name = get_optional_string(obj, "name").value_or("");
  res.identifier = get_optional_string(obj, "identifier");
  res.url = get_optional_string(obj, "url");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Info`.
static Info parse_Info(simdjson::dom::object obj) noexcept {
  Info res;
  res.title = get_optional_string(obj, "title").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  res.termsOfService = get_optional_string(obj, "termsOfService");
  simdjson::dom::object el_contact;
  if (obj["contact"].get(el_contact) == simdjson::SUCCESS) {
    res.contact = parse_Contact(el_contact);
  }
  simdjson::dom::object el_license;
  if (obj["license"].get(el_license) == simdjson::SUCCESS) {
    res.license = parse_License(el_license);
  }
  res.version = get_optional_string(obj, "version").value_or("");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_ServerVariable`.
static ServerVariable parse_ServerVariable(simdjson::dom::object obj) noexcept {
  ServerVariable res;
  res.enum_values = parse_vector_string(obj, "enum");
  res.default_value = get_optional_string(obj, "default").value_or("");
  res.description = get_optional_string(obj, "description");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Server`.
static Server parse_Server(simdjson::dom::object obj) noexcept {
  Server res;
  res.url = get_optional_string(obj, "url").value_or("");
  res.description = get_optional_string(obj, "description");
  res.name = get_optional_string(obj, "name");
  simdjson::dom::object el_variables;
  if (obj["variables"].get(el_variables) == simdjson::SUCCESS) {
    std::map<std::string, ServerVariable> m;
    for (auto field : el_variables) {
      m[std::string(field.key)] = parse_ServerVariable(unwrap_obj(field.value));
    }
    res.variables = m;
  }
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_ExternalDocumentation`.
static ExternalDocumentation
parse_ExternalDocumentation(simdjson::dom::object obj) noexcept {
  ExternalDocumentation res;
  res.description = get_optional_string(obj, "description");
  res.url = get_optional_string(obj, "url").value_or("");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Tag`.
static Tag parse_Tag(simdjson::dom::object obj) noexcept {
  Tag res;
  res.name = get_optional_string(obj, "name").value_or("");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  res.parent = get_optional_string(obj, "parent");
  res.kind = get_optional_string(obj, "kind");
  simdjson::dom::object el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS) {
    res.externalDocs = parse_ExternalDocumentation(el_externalDocs);
  }
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Discriminator`.
static Discriminator parse_Discriminator(simdjson::dom::object obj) noexcept {
  Discriminator res;
  res.propertyName = get_optional_string(obj, "propertyName").value_or("");
  res.mapping = parse_map_string(obj, "mapping");
  res.defaultMapping = get_optional_string(obj, "defaultMapping");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_XML`.
static XML parse_XML(simdjson::dom::object obj) noexcept {
  XML res;
  res.nodeType = get_optional_string(obj, "nodeType");
  res.name = get_optional_string(obj, "name");
  res.namespace_url = get_optional_string(obj, "namespace");
  res.prefix = get_optional_string(obj, "prefix");
  res.attribute = get_optional_bool(obj, "attribute");
  res.wrapped = get_optional_bool(obj, "wrapped");
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Schema`.
static Schema parse_Schema(simdjson::dom::object obj) noexcept {
  Schema res;
  res.type = get_optional_string(obj, "type");
  if (!res.type.has_value()) {
    res.types = parse_vector_string(obj, "type");
    if (res.types.has_value() && !res.types->empty()) {
      res.type = res.types->front();
    }
  }
  res.description = get_optional_string(obj, "description");
  res.format = get_optional_string(obj, "format");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  simdjson::dom::object el_discriminator;
  if (obj["discriminator"].get(el_discriminator) == simdjson::SUCCESS) {
    res.discriminator = parse_Discriminator(el_discriminator);
  }
  simdjson::dom::object el_xml;
  if (obj["xml"].get(el_xml) == simdjson::SUCCESS) {
    res.xml = parse_XML(el_xml);
  }
  simdjson::dom::object el_properties;
  if (obj["properties"].get(el_properties) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Schema>>();
    for (auto field : el_properties) {
      (*m)[std::string(field.key)] = parse_Schema(unwrap_obj(field.value));
    }
    res.properties = m;
  }
  simdjson::dom::object el_items;
  if (obj["items"].get(el_items) == simdjson::SUCCESS) {
    res.items = std::make_shared<Schema>(parse_Schema(el_items));
  }
  simdjson::dom::array el_allOf;
  if (obj["allOf"].get(el_allOf) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Schema>>();
    for (auto item : el_allOf) {
      v->push_back(parse_Schema(unwrap_obj(item)));
    }
    res.allOf = v;
  }
  simdjson::dom::array el_anyOf;
  if (obj["anyOf"].get(el_anyOf) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Schema>>();
    for (auto item : el_anyOf) {
      v->push_back(parse_Schema(unwrap_obj(item)));
    }
    res.anyOf = v;
  }
  simdjson::dom::array el_oneOf;
  if (obj["oneOf"].get(el_oneOf) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Schema>>();
    for (auto item : el_oneOf) {
      v->push_back(parse_Schema(unwrap_obj(item)));
    }
    res.oneOf = v;
  }
  simdjson::dom::object el_not;
  if (obj["not"].get(el_not) == simdjson::SUCCESS) {
    res.not_schema = std::make_shared<Schema>(parse_Schema(el_not));
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

  simdjson::dom::object el_defs;
  if (obj["$defs"].get(el_defs) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Schema>>();
    for (auto field : el_defs) {
      (*m)[std::string(field.key)] = parse_Schema(unwrap_obj(field.value));
    }
    res.defs = m;
  }

  simdjson::dom::element num_el;
  if (obj["multipleOf"].get(num_el) == simdjson::SUCCESS) {
    res.multipleOf = try_get_double(num_el);
  }
  if (obj["maximum"].get(num_el) == simdjson::SUCCESS) {
    res.maximum = try_get_double(num_el);
  }
  if (obj["exclusiveMaximum"].get(num_el) == simdjson::SUCCESS) {
    res.exclusiveMaximum = try_get_double(num_el);
  }
  if (obj["minimum"].get(num_el) == simdjson::SUCCESS) {
    res.minimum = try_get_double(num_el);
  }
  if (obj["exclusiveMinimum"].get(num_el) == simdjson::SUCCESS) {
    res.exclusiveMinimum = try_get_double(num_el);
  }

  if (obj["maxLength"].get(num_el) == simdjson::SUCCESS) {
    res.maxLength = try_get_int64(num_el);
  }
  if (obj["minLength"].get(num_el) == simdjson::SUCCESS) {
    res.minLength = try_get_int64(num_el);
  }
  if (obj["maxItems"].get(num_el) == simdjson::SUCCESS) {
    res.maxItems = try_get_int64(num_el);
  }
  if (obj["minItems"].get(num_el) == simdjson::SUCCESS) {
    res.minItems = try_get_int64(num_el);
  }
  if (obj["maxContains"].get(num_el) == simdjson::SUCCESS) {
    res.maxContains = try_get_int64(num_el);
  }
  if (obj["minContains"].get(num_el) == simdjson::SUCCESS) {
    res.minContains = try_get_int64(num_el);
  }
  if (obj["maxProperties"].get(num_el) == simdjson::SUCCESS) {
    res.maxProperties = try_get_int64(num_el);
  }
  if (obj["minProperties"].get(num_el) == simdjson::SUCCESS) {
    res.minProperties = try_get_int64(num_el);
  }

  res.pattern = get_optional_string(obj, "pattern");
  res.uniqueItems = get_optional_bool(obj, "uniqueItems");
  res.required = parse_vector_string(obj, "required");
  res.enum_values = parse_vector_string(obj, "enum");
  res.const_value = get_optional_string(obj, "const");
  res.examples = parse_vector_string(obj, "examples");
  res.default_value = get_optional_string(obj, "default");
  res.additionalPropertiesBool = get_optional_bool(obj, "additionalProperties");
  simdjson::dom::object el_addProps;
  if (obj["additionalProperties"].get(el_addProps) == simdjson::SUCCESS) {
    res.additionalProperties =
        std::make_shared<Schema>(parse_Schema(el_addProps));
  }
  simdjson::dom::array el_prefixItems;
  if (obj["prefixItems"].get(el_prefixItems) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Schema>>();
    for (auto item : el_prefixItems) {
      v->push_back(parse_Schema(unwrap_obj(item)));
    }
    res.prefixItems = v;
  }
  res.extensions = parse_extensions(obj);

  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Example`.
static Example parse_Example(simdjson::dom::object obj) noexcept {
  Example res;
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::element el_dv;
  if (obj["dataValue"].get(el_dv) == simdjson::SUCCESS) {
    res.dataValue = parse_json_value(el_dv);
  }
  res.serializedValue = get_optional_string(obj, "serializedValue");
  res.value = get_optional_string(obj, "value");
  res.externalValue = get_optional_string(obj, "externalValue");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_Encoding`.
static Encoding parse_Encoding(simdjson::dom::object obj) noexcept {
  Encoding res;
  res.contentType = get_optional_string(obj, "contentType");
  res.style = get_optional_string(obj, "style");
  res.explode = get_optional_bool(obj, "explode");
  res.allowReserved = get_optional_bool(obj, "allowReserved");
  simdjson::dom::object el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Header>>();
    for (auto field : el_headers) {
      (*m)[std::string(field.key)] = parse_Header(unwrap_obj(field.value));
    }
    res.headers = m;
  }

  simdjson::dom::object el_encoding;
  if (obj["encoding"].get(el_encoding) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Encoding>>();
    for (auto field : el_encoding) {
      (*m)[std::string(field.key)] = parse_Encoding(unwrap_obj(field.value));
    }
    res.encoding = m;
  }
  simdjson::dom::object el_itemSchema;
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS) {
    res.itemSchema = std::make_shared<Schema>(parse_Schema(el_itemSchema));
  }
  simdjson::dom::object el_itemEncoding;
  if (obj["itemEncoding"].get(el_itemEncoding) == simdjson::SUCCESS) {
    res.itemEncoding =
        std::make_shared<Encoding>(parse_Encoding(el_itemEncoding));
  }

  simdjson::dom::array el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Encoding>>();
    for (auto item : el_prefixEncoding) {
      v->push_back(parse_Encoding(unwrap_obj(item)));
    }
    res.prefixEncoding = v;
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_MediaType`.
static MediaType parse_MediaType(simdjson::dom::object obj) noexcept {
  MediaType res;
  simdjson::dom::object el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS) {
    res.schema = parse_Schema(el_schema);
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::object el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS) {
    std::map<std::string, Example> m;
    for (auto field : el_examples) {
      m[std::string(field.key)] = parse_Example(unwrap_obj(field.value));
    }
    res.examples = m;
  }
  simdjson::dom::object el_encoding;
  if (obj["encoding"].get(el_encoding) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Encoding>>();
    for (auto field : el_encoding) {
      (*m)[std::string(field.key)] = parse_Encoding(unwrap_obj(field.value));
    }
    res.encoding = m;
  }
  simdjson::dom::object el_itemSchema;
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS) {
    res.itemSchema = std::make_shared<Schema>(parse_Schema(el_itemSchema));
  }
  simdjson::dom::object el_itemEncoding;
  if (obj["itemEncoding"].get(el_itemEncoding) == simdjson::SUCCESS) {
    res.itemEncoding =
        std::make_shared<Encoding>(parse_Encoding(el_itemEncoding));
  }

  simdjson::dom::array el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS) {
    auto v = std::make_shared<std::vector<Encoding>>();
    for (auto item : el_prefixEncoding) {
      v->push_back(parse_Encoding(unwrap_obj(item)));
    }
    res.prefixEncoding = v;
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Header`.
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
  simdjson::dom::object el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS) {
    res.schema = parse_Schema(el_schema);
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::object el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS) {
    std::map<std::string, Example> m;
    for (auto field : el_examples) {
      m[std::string(field.key)] = parse_Example(unwrap_obj(field.value));
    }
    res.examples = m;
  }
  simdjson::dom::object el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content) {
      m[std::string(field.key)] = parse_MediaType(unwrap_obj(field.value));
    }
    res.content = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_RequestBody`.
static RequestBody parse_RequestBody(simdjson::dom::object obj) noexcept {
  RequestBody res;
  res.description = get_optional_string(obj, "description");
  simdjson::dom::object el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content) {
      m[std::string(field.key)] = parse_MediaType(unwrap_obj(field.value));
    }
    res.content = m;
  }
  res.required = get_optional_bool(obj, "required").value_or(false);
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Link`.
static Link parse_Link(simdjson::dom::object obj) noexcept {
  Link res;
  res.operationRef = get_optional_string(obj, "operationRef");
  res.operationId = get_optional_string(obj, "operationId");
  res.parameters = parse_map_string(obj, "parameters");
  res.requestBody = get_optional_string(obj, "requestBody");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::object el_server;
  if (obj["server"].get(el_server) == simdjson::SUCCESS) {
    res.server = parse_Server(el_server);
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Response`.
static Response parse_Response(simdjson::dom::object obj) noexcept {
  Response res;
  res.description = get_optional_string(obj, "description").value_or("");
  simdjson::dom::object el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS) {
    std::map<std::string, Header> m;
    for (auto field : el_headers) {
      m[std::string(field.key)] = parse_Header(unwrap_obj(field.value));
    }
    res.headers = m;
  }
  simdjson::dom::object el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content) {
      m[std::string(field.key)] = parse_MediaType(unwrap_obj(field.value));
    }
    res.content = m;
  }
  simdjson::dom::object el_links;
  if (obj["links"].get(el_links) == simdjson::SUCCESS) {
    std::map<std::string, Link> m;
    for (auto field : el_links) {
      m[std::string(field.key)] = parse_Link(unwrap_obj(field.value));
    }
    res.links = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Parameter`.
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
  simdjson::dom::object el_schema;
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS) {
    res.schema = parse_Schema(el_schema);
  }
  res.example = get_optional_string(obj, "example");
  simdjson::dom::object el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS) {
    std::map<std::string, Example> m;
    for (auto field : el_examples) {
      m[std::string(field.key)] = parse_Example(unwrap_obj(field.value));
    }
    res.examples = m;
  }
  simdjson::dom::object el_content;
  if (obj["content"].get(el_content) == simdjson::SUCCESS) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content) {
      m[std::string(field.key)] = parse_MediaType(unwrap_obj(field.value));
    }
    res.content = m;
  }
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse_OAuthFlow`.
static OAuthFlow parse_OAuthFlow(simdjson::dom::object obj) noexcept {
  OAuthFlow res;
  res.authorizationUrl = get_optional_string(obj, "authorizationUrl");
  res.tokenUrl = get_optional_string(obj, "tokenUrl");
  res.refreshUrl = get_optional_string(obj, "refreshUrl");
  res.deviceAuthorizationUrl =
      get_optional_string(obj, "deviceAuthorizationUrl");
  res.scopes = parse_map_string(obj, "scopes");
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_OAuthFlows`.
static OAuthFlows parse_OAuthFlows(simdjson::dom::object obj) noexcept {
  OAuthFlows res;
  auto get_flow = [&](std::string_view key) -> std::optional<OAuthFlow> {
    simdjson::dom::object fo;
    if (obj[key].get(fo) == simdjson::SUCCESS)
      return parse_OAuthFlow(fo);
    return std::nullopt;
  };
  res.implicit = get_flow("implicit");
  res.password = get_flow("password");
  res.clientCredentials = get_flow("clientCredentials");
  res.authorizationCode = get_flow("authorizationCode");
  res.deviceAuthorization = get_flow("deviceAuthorization");
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_SecurityScheme`.
static SecurityScheme parse_SecurityScheme(simdjson::dom::object obj) noexcept {
  SecurityScheme res;
  res.type = get_optional_string(obj, "type").value_or("");
  res.description = get_optional_string(obj, "description");
  res.name = get_optional_string(obj, "name");
  res.in = get_optional_string(obj, "in");
  res.scheme = get_optional_string(obj, "scheme");
  res.bearerFormat = get_optional_string(obj, "bearerFormat");
  simdjson::dom::object el_flows;
  if (obj["flows"].get(el_flows) == simdjson::SUCCESS) {
    res.flows = parse_OAuthFlows(el_flows);
  }
  res.openIdConnectUrl = get_optional_string(obj, "openIdConnectUrl");
  res.oauth2MetadataUrl = get_optional_string(obj, "oauth2MetadataUrl");
  res.deprecated = get_optional_bool(obj, "deprecated");
  simdjson::dom::element el_ref;
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) {
    res.ref = parse_Reference(obj);
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Operation`.
static Operation parse_Operation(simdjson::dom::object obj) noexcept {
  Operation res;
  res.tags = parse_vector_string(obj, "tags");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  simdjson::dom::object el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS) {
    res.externalDocs = parse_ExternalDocumentation(el_externalDocs);
  }
  res.operationId = get_optional_string(obj, "operationId");
  simdjson::dom::array el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS) {
    std::vector<Parameter> vec;
    for (auto item : el_parameters) {
      vec.push_back(parse_Parameter(unwrap_obj(item)));
    }
    res.parameters = vec;
  }
  simdjson::dom::object el_requestBody;
  if (obj["requestBody"].get(el_requestBody) == simdjson::SUCCESS) {
    res.requestBody = parse_RequestBody(el_requestBody);
  }
  simdjson::dom::object el_responses;
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS) {
    std::map<std::string, Response> m;
    for (auto field : el_responses) {
      m[std::string(field.key)] = parse_Response(unwrap_obj(field.value));
    }
    res.responses = m;
  }
  simdjson::dom::object el_callbacks;
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS) {
    auto m = std::make_shared<
        std::map<std::string, std::map<std::string, PathItem>>>();
    for (auto field : el_callbacks) {
      std::map<std::string, PathItem> inner_m;
      for (auto inner_field : unwrap_obj(field.value)) {
        inner_m[std::string(inner_field.key)] =
            parse_PathItem(unwrap_obj(inner_field.value));
      }
      (*m)[std::string(field.key)] = inner_m;
    }
    res.callbacks = m;
  }
  res.deprecated = get_optional_bool(obj, "deprecated").value_or(false);
  res.security = parse_security(obj, "security");
  simdjson::dom::array el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS) {
    std::vector<Server> vec;
    for (auto item : el_servers) {
      vec.push_back(parse_Server(unwrap_obj(item)));
    }
    res.servers = vec;
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_PathItem`.
static PathItem parse_PathItem(simdjson::dom::object obj) noexcept {
  PathItem res;
  res.ref = get_optional_string(obj, "$ref");
  res.summary = get_optional_string(obj, "summary");
  res.description = get_optional_string(obj, "description");
  auto get_op = [&](std::string_view key) -> std::optional<Operation> {
    simdjson::dom::object op_obj;
    if (obj[key].get(op_obj) == simdjson::SUCCESS)
      return parse_Operation(op_obj);
    return std::nullopt;
  };
  res.get = get_op("get");
  res.put = get_op("put");
  res.post = get_op("post");
  res.delete_op = get_op("delete");
  res.options = get_op("options");
  res.head = get_op("head");
  res.patch = get_op("patch");
  res.trace = get_op("trace");
  res.query = get_op("query");
  simdjson::dom::array el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS) {
    std::vector<Server> vec;
    for (auto item : el_servers) {
      vec.push_back(parse_Server(unwrap_obj(item)));
    }
    res.servers = vec;
  }
  simdjson::dom::array el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS) {
    std::vector<Parameter> vec;
    for (auto item : el_parameters) {
      vec.push_back(parse_Parameter(unwrap_obj(item)));
    }
    res.parameters = vec;
  }
  simdjson::dom::object el_addOps;
  if (obj["additionalOperations"].get(el_addOps) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, Operation>>();
    for (auto field : el_addOps) {
      (*m)[std::string(field.key)] = parse_Operation(unwrap_obj(field.value));
    }
    res.additionalOperations = m;
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_Components`.
static Components parse_Components(simdjson::dom::object obj) noexcept {
  Components res;
  simdjson::dom::object el_schemas;
  if (obj["schemas"].get(el_schemas) == simdjson::SUCCESS) {
    std::map<std::string, Schema> m;
    for (auto field : el_schemas) {
      m[std::string(field.key)] = parse_Schema(unwrap_obj(field.value));
    }
    res.schemas = m;
  }
  simdjson::dom::object el_responses;
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS) {
    std::map<std::string, Response> m;
    for (auto field : el_responses) {
      m[std::string(field.key)] = parse_Response(unwrap_obj(field.value));
    }
    res.responses = m;
  }
  simdjson::dom::object el_parameters;
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS) {
    std::map<std::string, Parameter> m;
    for (auto field : el_parameters) {
      m[std::string(field.key)] = parse_Parameter(unwrap_obj(field.value));
    }
    res.parameters = m;
  }
  simdjson::dom::object el_examples;
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS) {
    std::map<std::string, Example> m;
    for (auto field : el_examples) {
      m[std::string(field.key)] = parse_Example(unwrap_obj(field.value));
    }
    res.examples = m;
  }
  simdjson::dom::object el_requestBodies;
  if (obj["requestBodies"].get(el_requestBodies) == simdjson::SUCCESS) {
    std::map<std::string, RequestBody> m;
    for (auto field : el_requestBodies) {
      m[std::string(field.key)] = parse_RequestBody(unwrap_obj(field.value));
    }
    res.requestBodies = m;
  }
  simdjson::dom::object el_headers;
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS) {
    std::map<std::string, Header> m;
    for (auto field : el_headers) {
      m[std::string(field.key)] = parse_Header(unwrap_obj(field.value));
    }
    res.headers = m;
  }
  simdjson::dom::object el_securitySchemes;
  if (obj["securitySchemes"].get(el_securitySchemes) == simdjson::SUCCESS) {
    std::map<std::string, SecurityScheme> m;
    for (auto field : el_securitySchemes) {
      m[std::string(field.key)] = parse_SecurityScheme(unwrap_obj(field.value));
    }
    res.securitySchemes = m;
  }
  simdjson::dom::object el_links;
  if (obj["links"].get(el_links) == simdjson::SUCCESS) {
    std::map<std::string, Link> m;
    for (auto field : el_links) {
      m[std::string(field.key)] = parse_Link(unwrap_obj(field.value));
    }
    res.links = m;
  }
  simdjson::dom::object el_callbacks;
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS) {
    auto m = std::make_shared<
        std::map<std::string, std::map<std::string, PathItem>>>();
    for (auto field : el_callbacks) {
      std::map<std::string, PathItem> inner_m;
      for (auto inner_field : unwrap_obj(field.value)) {
        inner_m[std::string(inner_field.key)] =
            parse_PathItem(unwrap_obj(inner_field.value));
      }
      (*m)[std::string(field.key)] = inner_m;
    }
    res.callbacks = m;
  }
  simdjson::dom::object el_pathItems;
  if (obj["pathItems"].get(el_pathItems) == simdjson::SUCCESS) {
    std::map<std::string, PathItem> m;
    for (auto field : el_pathItems) {
      m[std::string(field.key)] = parse_PathItem(unwrap_obj(field.value));
    }
    res.pathItems = m;
  }
  simdjson::dom::object el_mediaTypes;
  if (obj["mediaTypes"].get(el_mediaTypes) == simdjson::SUCCESS) {
    auto m = std::make_shared<std::map<std::string, MediaType>>();
    for (auto field : el_mediaTypes) {
      (*m)[std::string(field.key)] = parse_MediaType(unwrap_obj(field.value));
    }
    res.mediaTypes = m;
  }
  res.extensions = parse_extensions(obj);
  return res;
}
/// @brief Deserialize OpenAPI JSON element via `parse_OpenAPI`.
static OpenAPI parse_OpenAPI(simdjson::dom::object obj) noexcept {
  OpenAPI res;
  res.openapi = get_optional_string(obj, "openapi").value_or("");
  res.self_link = get_optional_string(obj, "$self");
  simdjson::dom::object el_info;
  if (obj["info"].get(el_info) == simdjson::SUCCESS) {
    res.info = parse_Info(el_info);
  }
  res.jsonSchemaDialect = get_optional_string(obj, "jsonSchemaDialect");
  simdjson::dom::array el_servers;
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS) {
    std::vector<Server> vec;
    for (auto item : el_servers) {
      vec.push_back(parse_Server(unwrap_obj(item)));
    }
    res.servers = vec;
  }
  simdjson::dom::object el_paths;
  if (obj["paths"].get(el_paths) == simdjson::SUCCESS) {
    std::map<std::string, PathItem> m;
    for (auto field : el_paths) {
      m[std::string(field.key)] = parse_PathItem(unwrap_obj(field.value));
    }
    res.paths = m;
  }
  simdjson::dom::object el_webhooks;
  if (obj["webhooks"].get(el_webhooks) == simdjson::SUCCESS) {
    std::map<std::string, PathItem> m;
    for (auto field : el_webhooks) {
      m[std::string(field.key)] = parse_PathItem(unwrap_obj(field.value));
    }
    res.webhooks = m;
  }
  simdjson::dom::object el_components;
  if (obj["components"].get(el_components) == simdjson::SUCCESS) {
    res.components = parse_Components(el_components);
  }
  res.security = parse_security(obj, "security");
  simdjson::dom::array el_tags;
  if (obj["tags"].get(el_tags) == simdjson::SUCCESS) {
    std::vector<Tag> vec;
    for (auto item : el_tags) {
      vec.push_back(parse_Tag(unwrap_obj(item)));
    }
    res.tags = vec;
  }
  simdjson::dom::object el_externalDocs;
  if (obj["externalDocs"].get(el_externalDocs) == simdjson::SUCCESS) {
    res.externalDocs = parse_ExternalDocumentation(el_externalDocs);
  }
  res.extensions = parse_extensions(obj);
  return res;
}

/// @brief Deserialize OpenAPI JSON element via `parse`.
Result<OpenAPI> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  if (parser.parse(input).get(doc) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::JsonParseError,
                             "Failed to parse JSON");
  }
  auto root_obj = try_get_object(doc);
  if (!root_obj.has_value()) {
    return utils::make_error(utils::ErrorCode::InvalidOpenApiDocument,
                             "OpenAPI doc must be an object");
  }
  return parse_OpenAPI(*root_obj);
}
} // namespace cdd_cpp::openapi
