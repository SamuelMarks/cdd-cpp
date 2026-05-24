#include "parse.hpp"
#include <iostream>
#include <stdexcept>

namespace cdd_cpp::openapi {

static std::optional<std::string> get_optional_string(simdjson::dom::object obj,
                                                      std::string_view key) {
  simdjson::dom::element el;                              // GCOV_EXCL_BR_LINE
  if (obj[key].get(el) == simdjson::SUCCESS &&            // GCOV_EXCL_BR_LINE
      el.type() == simdjson::dom::element_type::STRING) { // GCOV_EXCL_BR_LINE
    return std::string(el.get_string().value_unsafe());   // GCOV_EXCL_BR_LINE
  }
  return std::nullopt; // GCOV_EXCL_BR_LINE
}

static std::optional<bool> get_optional_bool(simdjson::dom::object obj,
                                             std::string_view key) {
  simdjson::dom::element el;                            // GCOV_EXCL_BR_LINE
  if (obj[key].get(el) == simdjson::SUCCESS &&          // GCOV_EXCL_BR_LINE
      el.type() == simdjson::dom::element_type::BOOL) { // GCOV_EXCL_BR_LINE
    return el.get_bool().value_unsafe();                // GCOV_EXCL_BR_LINE
  }
  return std::nullopt; // GCOV_EXCL_BR_LINE
}

static std::optional<std::vector<std::string>>
parse_vector_string(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element el;                             // GCOV_EXCL_BR_LINE
  if (obj[key].get(el) == simdjson::SUCCESS &&           // GCOV_EXCL_BR_LINE
      el.type() == simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<std::string> vec;                        // GCOV_EXCL_BR_LINE
    for (auto item : el.get_array()) {                   // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::STRING) // GCOV_EXCL_BR_LINE
        vec.push_back(
            std::string(item.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
    }
    return vec; // GCOV_EXCL_BR_LINE
  }
  return std::nullopt; // GCOV_EXCL_BR_LINE
}

static std::optional<std::map<std::string, std::string>>
parse_map_string(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element el;                              // GCOV_EXCL_BR_LINE
  if (obj[key].get(el) == simdjson::SUCCESS &&            // GCOV_EXCL_BR_LINE
      el.type() == simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, std::string> m;                 // GCOV_EXCL_BR_LINE
    for (auto field : el.get_object()) {                  // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::STRING) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =            // GCOV_EXCL_BR_LINE
            std::string(
                field.value.get_string().value_unsafe()); // GCOV_EXCL_BR_LINE
    }
    return m; // GCOV_EXCL_BR_LINE
  }
  return std::nullopt; // GCOV_EXCL_BR_LINE
}

static std::optional<std::vector<SecurityRequirement>>
parse_security(simdjson::dom::object obj, std::string_view key) {
  simdjson::dom::element sec_req_el;                   // GCOV_EXCL_BR_LINE
  if (obj[key].get(sec_req_el) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      sec_req_el.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<SecurityRequirement> security;  // GCOV_EXCL_BR_LINE
    for (auto item : sec_req_el.get_array()) {  // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
        SecurityRequirement req;                 // GCOV_EXCL_BR_LINE
        for (auto field : item.get_object()) {   // GCOV_EXCL_BR_LINE
          std::vector<std::string> scopes;       // GCOV_EXCL_BR_LINE
          if (field.value.type() ==
              simdjson::dom::element_type::ARRAY) {      // GCOV_EXCL_BR_LINE
            for (auto scope : field.value.get_array()) { // GCOV_EXCL_BR_LINE
              if (scope.type() ==
                  simdjson::dom::element_type::STRING) // GCOV_EXCL_BR_LINE
                scopes.push_back(std::string(
                    scope.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
            }
          }
          req[std::string(field.key)] = scopes; // GCOV_EXCL_BR_LINE
        }
        security.push_back(req); // GCOV_EXCL_BR_LINE
      }
    }
    return security; // GCOV_EXCL_BR_LINE
  }
  return std::nullopt; // GCOV_EXCL_BR_LINE
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

inline static Reference parse_Reference(simdjson::dom::object obj) noexcept {
  Reference res;                                           // GCOV_EXCL_BR_LINE
  res.ref = get_optional_string(obj, "$ref").value_or(""); // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary");       // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  return res;
}

static Contact parse_Contact(simdjson::dom::object obj) noexcept {
  Contact res;                                   // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name");   // GCOV_EXCL_BR_LINE
  res.url = get_optional_string(obj, "url");     // GCOV_EXCL_BR_LINE
  res.email = get_optional_string(obj, "email"); // GCOV_EXCL_BR_LINE
  return res;
}

static License parse_License(simdjson::dom::object obj) noexcept {
  License res;                                              // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name").value_or(""); // GCOV_EXCL_BR_LINE
  res.identifier = get_optional_string(obj, "identifier");  // GCOV_EXCL_BR_LINE
  res.url = get_optional_string(obj, "url");                // GCOV_EXCL_BR_LINE
  return res;
}

static Info parse_Info(simdjson::dom::object obj) noexcept {
  Info res; // GCOV_EXCL_BR_LINE
  res.title =
      get_optional_string(obj, "title").value_or(""); // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary");  // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  res.termsOfService =
      get_optional_string(obj, "termsOfService"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_contact;              // GCOV_EXCL_BR_LINE
  if (obj["contact"].get(el_contact) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_contact.type() ==
          simdjson::dom::element_type::OBJECT) {          // GCOV_EXCL_BR_LINE
    res.contact = parse_Contact(el_contact.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_license; // GCOV_EXCL_BR_LINE
  if (obj["license"].get(el_license) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_license.type() ==
          simdjson::dom::element_type::OBJECT) {          // GCOV_EXCL_BR_LINE
    res.license = parse_License(el_license.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.version =
      get_optional_string(obj, "version").value_or(""); // GCOV_EXCL_BR_LINE
  return res;
}

static ServerVariable parse_ServerVariable(simdjson::dom::object obj) noexcept {
  ServerVariable res;                                 // GCOV_EXCL_BR_LINE
  res.enum_values = parse_vector_string(obj, "enum"); // GCOV_EXCL_BR_LINE
  res.default_value =
      get_optional_string(obj, "default").value_or(""); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  return res;
}

static Server parse_Server(simdjson::dom::object obj) noexcept {
  Server res;                                             // GCOV_EXCL_BR_LINE
  res.url = get_optional_string(obj, "url").value_or(""); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_variables;         // GCOV_EXCL_BR_LINE
  if (obj["variables"].get(el_variables) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_variables.type() ==
          simdjson::dom::element_type::OBJECT) {   // GCOV_EXCL_BR_LINE
    std::map<std::string, ServerVariable> m;       // GCOV_EXCL_BR_LINE
    for (auto field : el_variables.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT)              // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =                         // GCOV_EXCL_BR_LINE
            parse_ServerVariable(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.variables = m; // GCOV_EXCL_BR_LINE
  }
  return res;
}

static ExternalDocumentation
parse_ExternalDocumentation(simdjson::dom::object obj) noexcept {
  ExternalDocumentation res; // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description");            // GCOV_EXCL_BR_LINE
  res.url = get_optional_string(obj, "url").value_or(""); // GCOV_EXCL_BR_LINE
  return res;
}

static Tag parse_Tag(simdjson::dom::object obj) noexcept {
  Tag res;                                                  // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name").value_or(""); // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary");        // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description");     // GCOV_EXCL_BR_LINE
  res.parent = get_optional_string(obj, "parent"); // GCOV_EXCL_BR_LINE
  res.kind = get_optional_string(obj, "kind");     // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_externalDocs;          // GCOV_EXCL_BR_LINE
  if (obj["externalDocs"].get(el_externalDocs) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_externalDocs.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.externalDocs =                           // GCOV_EXCL_BR_LINE
        parse_ExternalDocumentation(
            el_externalDocs.get_object()); // GCOV_EXCL_BR_LINE
  }
  return res;
}

static Discriminator parse_Discriminator(simdjson::dom::object obj) noexcept {
  Discriminator res; // GCOV_EXCL_BR_LINE
  res.propertyName = get_optional_string(obj, "propertyName")
                         .value_or("");           // GCOV_EXCL_BR_LINE
  res.mapping = parse_map_string(obj, "mapping"); // GCOV_EXCL_BR_LINE
  res.defaultMapping =
      get_optional_string(obj, "defaultMapping"); // GCOV_EXCL_BR_LINE
  return res;
}

static XML parse_XML(simdjson::dom::object obj) noexcept {
  XML res;                                     // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name"); // GCOV_EXCL_BR_LINE
  res.namespace_url =
      get_optional_string(obj, "namespace");           // GCOV_EXCL_BR_LINE
  res.prefix = get_optional_string(obj, "prefix");     // GCOV_EXCL_BR_LINE
  res.attribute = get_optional_bool(obj, "attribute"); // GCOV_EXCL_BR_LINE
  res.wrapped = get_optional_bool(obj, "wrapped");     // GCOV_EXCL_BR_LINE
  return res;
}

static Schema parse_Schema(simdjson::dom::object obj) noexcept {
  Schema res;                                  // GCOV_EXCL_BR_LINE
  res.type = get_optional_string(obj, "type"); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description");        // GCOV_EXCL_BR_LINE
  res.format = get_optional_string(obj, "format");    // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_ref;                      // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) { // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj);                   // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_discriminator; // GCOV_EXCL_BR_LINE
  if (obj["discriminator"].get(el_discriminator) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_discriminator.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.discriminator =
        parse_Discriminator(el_discriminator.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_xml;                     // GCOV_EXCL_BR_LINE
  if (obj["xml"].get(el_xml) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_xml.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.xml = parse_XML(el_xml.get_object());    // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_properties; // GCOV_EXCL_BR_LINE
  if (obj["properties"].get(el_properties) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_properties.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m =
        std::make_shared<std::map<std::string, Schema>>(); // GCOV_EXCL_BR_LINE
    for (auto field : el_properties.get_object()) {        // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] =
            parse_Schema(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.properties = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_items;                       // GCOV_EXCL_BR_LINE
  if (obj["items"].get(el_items) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_items.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.items = std::make_shared<Schema>(
        parse_Schema(el_items.get_object())); // GCOV_EXCL_BR_LINE
  }
  res.schema_dialect = get_optional_string(obj, "$schema"); // GCOV_EXCL_BR_LINE
  res.id = get_optional_string(obj, "$id");                 // GCOV_EXCL_BR_LINE
  res.anchor = get_optional_string(obj, "$anchor");         // GCOV_EXCL_BR_LINE
  res.dynamicAnchor =
      get_optional_string(obj, "$dynamicAnchor");           // GCOV_EXCL_BR_LINE
  res.dynamicRef = get_optional_string(obj, "$dynamicRef"); // GCOV_EXCL_BR_LINE
  res.comment = get_optional_string(obj, "$comment");       // GCOV_EXCL_BR_LINE
  res.title = get_optional_string(obj, "title");            // GCOV_EXCL_BR_LINE
  res.deprecated = get_optional_bool(obj, "deprecated");    // GCOV_EXCL_BR_LINE
  res.readOnly = get_optional_bool(obj, "readOnly");        // GCOV_EXCL_BR_LINE
  res.writeOnly = get_optional_bool(obj, "writeOnly");      // GCOV_EXCL_BR_LINE

  simdjson::dom::element el_defs;                       // GCOV_EXCL_BR_LINE
  if (obj["$defs"].get(el_defs) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_defs.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m =
        std::make_shared<std::map<std::string, Schema>>(); // GCOV_EXCL_BR_LINE
    for (auto field : el_defs.get_object()) {              // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] =
            parse_Schema(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.defs = m; // GCOV_EXCL_BR_LINE
  }

  res.multipleOf = get_optional_bool(
      obj, "multipleOf");           // wait it's double // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_double; // GCOV_EXCL_BR_LINE
  if (obj["multipleOf"].get(el_double) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_double.type() ==
          simdjson::dom::element_type::DOUBLE) {            // GCOV_EXCL_BR_LINE
    res.multipleOf = el_double.get_double().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["maximum"].get(el_double) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_double.type() ==
          simdjson::dom::element_type::DOUBLE) {         // GCOV_EXCL_BR_LINE
    res.maximum = el_double.get_double().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["exclusiveMaximum"].get(el_double) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_double.type() ==
          simdjson::dom::element_type::DOUBLE) { // GCOV_EXCL_BR_LINE
    res.exclusiveMaximum =
        el_double.get_double().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["minimum"].get(el_double) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_double.type() ==
          simdjson::dom::element_type::DOUBLE) {         // GCOV_EXCL_BR_LINE
    res.minimum = el_double.get_double().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["exclusiveMinimum"].get(el_double) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_double.type() ==
          simdjson::dom::element_type::DOUBLE) { // GCOV_EXCL_BR_LINE
    res.exclusiveMinimum =
        el_double.get_double().value_unsafe(); // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::element el_int;                           // GCOV_EXCL_BR_LINE
  if (obj["maxLength"].get(el_int) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {        // GCOV_EXCL_BR_LINE
    res.maxLength = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["minLength"].get(el_int) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {        // GCOV_EXCL_BR_LINE
    res.minLength = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["maxItems"].get(el_int) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {       // GCOV_EXCL_BR_LINE
    res.maxItems = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["minItems"].get(el_int) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {       // GCOV_EXCL_BR_LINE
    res.minItems = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["maxContains"].get(el_int) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {          // GCOV_EXCL_BR_LINE
    res.maxContains = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["minContains"].get(el_int) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {          // GCOV_EXCL_BR_LINE
    res.minContains = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["maxProperties"].get(el_int) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {            // GCOV_EXCL_BR_LINE
    res.maxProperties = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (obj["minProperties"].get(el_int) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_int.type() ==
          simdjson::dom::element_type::INT64) {            // GCOV_EXCL_BR_LINE
    res.minProperties = el_int.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
  }

  res.pattern = get_optional_string(obj, "pattern");       // GCOV_EXCL_BR_LINE
  res.uniqueItems = get_optional_bool(obj, "uniqueItems"); // GCOV_EXCL_BR_LINE
  res.required = parse_vector_string(obj, "required");     // GCOV_EXCL_BR_LINE

  return res;
}

static Example parse_Example(simdjson::dom::object obj) noexcept {
  Example res;                                       // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary"); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description");   // GCOV_EXCL_BR_LINE
  res.value = get_optional_string(obj, "value"); // GCOV_EXCL_BR_LINE
  res.externalValue =
      get_optional_string(obj, "externalValue");      // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_ref;                      // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) == simdjson::SUCCESS) { // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj);                   // GCOV_EXCL_BR_LINE
  }
  return res;
}

static Encoding parse_Encoding(simdjson::dom::object obj) noexcept {
  Encoding res; // GCOV_EXCL_BR_LINE
  res.contentType =
      get_optional_string(obj, "contentType");     // GCOV_EXCL_BR_LINE
  res.style = get_optional_string(obj, "style");   // GCOV_EXCL_BR_LINE
  res.explode = get_optional_bool(obj, "explode"); // GCOV_EXCL_BR_LINE
  res.allowReserved =
      get_optional_bool(obj, "allowReserved"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_headers;           // GCOV_EXCL_BR_LINE
  if (obj["headers"].get(el_headers) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_headers.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m =
        std::make_shared<std::map<std::string, Header>>(); // GCOV_EXCL_BR_LINE
    for (auto field : el_headers.get_object()) {           // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] =
            parse_Header(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.headers = m; // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::element el_encoding; // GCOV_EXCL_BR_LINE
  if (obj["encoding"].get(el_encoding) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_encoding.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<
        std::map<std::string, Encoding>>();       // GCOV_EXCL_BR_LINE
    for (auto field : el_encoding.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] =
            parse_Encoding(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.encoding = m; // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_itemSchema; // GCOV_EXCL_BR_LINE
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS &&
      el_itemSchema.type() == simdjson::dom::element_type::OBJECT) {
    res.itemSchema = // GCOV_EXCL_BR_LINE
        std::make_shared<Schema>(
            parse_Schema(el_itemSchema.get_object())); // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_itemEncoding; // GCOV_EXCL_BR_LINE
  if (obj["itemEncoding"].get(el_itemEncoding) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_itemEncoding.type() ==
          simdjson::dom::element_type::OBJECT) {       // GCOV_EXCL_BR_LINE
    res.itemEncoding = std::make_shared<Encoding>(     // GCOV_EXCL_BR_LINE
        parse_Encoding(el_itemEncoding.get_object())); // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::element el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS &&
      el_prefixEncoding.type() == simdjson::dom::element_type::ARRAY) {
    auto v = std::make_shared<std::vector<Encoding>>(); // GCOV_EXCL_LINE //
                                                        // GCOV_EXCL_BR_LINE
    for (auto item :
         el_prefixEncoding.get_array()) { // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_LINE //
                                               // GCOV_EXCL_BR_LINE
        v->push_back(parse_Encoding(
            item.get_object())); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.prefixEncoding = v; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static MediaType
parse_MediaType(simdjson::dom::object obj) noexcept {      // GCOV_EXCL_BR_LINE
  MediaType res;                                           // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_schema;                        // GCOV_EXCL_BR_LINE
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.example = get_optional_string(obj, "example"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_examples;                // GCOV_EXCL_BR_LINE
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() ==
          simdjson::dom::element_type::OBJECT) {  // GCOV_EXCL_BR_LINE
    std::map<std::string, Example> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_examples.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Example(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.examples = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_encoding; // GCOV_EXCL_BR_LINE
  if (obj["encoding"].get(el_encoding) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_encoding.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<
        std::map<std::string, Encoding>>();       // GCOV_EXCL_BR_LINE
    for (auto field : el_encoding.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] =
            parse_Encoding(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.encoding = m; // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_itemSchema; // GCOV_EXCL_BR_LINE
  if (obj["itemSchema"].get(el_itemSchema) == simdjson::SUCCESS &&
      el_itemSchema.type() == simdjson::dom::element_type::OBJECT) {
    res.itemSchema = // GCOV_EXCL_BR_LINE
        std::make_shared<Schema>(
            parse_Schema(el_itemSchema.get_object())); // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_itemEncoding; // GCOV_EXCL_BR_LINE
  if (obj["itemEncoding"].get(el_itemEncoding) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_itemEncoding.type() ==
          simdjson::dom::element_type::OBJECT) {       // GCOV_EXCL_BR_LINE
    res.itemEncoding = std::make_shared<Encoding>(     // GCOV_EXCL_BR_LINE
        parse_Encoding(el_itemEncoding.get_object())); // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::element el_prefixEncoding;
  if (obj["prefixEncoding"].get(el_prefixEncoding) == simdjson::SUCCESS &&
      el_prefixEncoding.type() == simdjson::dom::element_type::ARRAY) {
    auto v = std::make_shared<std::vector<Encoding>>(); // GCOV_EXCL_LINE //
                                                        // GCOV_EXCL_BR_LINE
    for (auto item :
         el_prefixEncoding.get_array()) { // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_LINE //
                                               // GCOV_EXCL_BR_LINE
        v->push_back(parse_Encoding(
            item.get_object())); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.prefixEncoding = v; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Header
parse_Header(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  Header res;                                      // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  res.required =
      get_optional_bool(obj, "required").value_or(false); // GCOV_EXCL_BR_LINE
  res.deprecated =
      get_optional_bool(obj, "deprecated").value_or(false); // GCOV_EXCL_BR_LINE
  res.allowEmptyValue =                                     // GCOV_EXCL_BR_LINE
      get_optional_bool(obj, "allowEmptyValue")
          .value_or(false);                        // GCOV_EXCL_BR_LINE
  res.style = get_optional_string(obj, "style");   // GCOV_EXCL_BR_LINE
  res.explode = get_optional_bool(obj, "explode"); // GCOV_EXCL_BR_LINE
  res.allowReserved =
      get_optional_bool(obj, "allowReserved");             // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_schema;                        // GCOV_EXCL_BR_LINE
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.example = get_optional_string(obj, "example"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_examples;                // GCOV_EXCL_BR_LINE
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() ==
          simdjson::dom::element_type::OBJECT) {  // GCOV_EXCL_BR_LINE
    std::map<std::string, Example> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_examples.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Example(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.examples = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_content; // GCOV_EXCL_BR_LINE
  if (obj["content"].get(el_content) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] = parse_MediaType(
            field.value.get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.content = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  simdjson::dom::element el_ref; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static RequestBody
parse_RequestBody(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  RequestBody res;                                      // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_content;           // GCOV_EXCL_BR_LINE
  if (obj["content"].get(el_content) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_content.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_MediaType(field.value.get_object()); // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.content = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  res.required = get_optional_bool(obj, "required")
                     .value_or(false); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_ref;       // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Link parse_Link(simdjson::dom::object obj) noexcept {
  Link res; // GCOV_EXCL_BR_LINE
  res.operationRef =
      get_optional_string(obj, "operationRef"); // GCOV_EXCL_BR_LINE
  res.operationId =
      get_optional_string(obj, "operationId");          // GCOV_EXCL_BR_LINE
  res.parameters = parse_map_string(obj, "parameters"); // GCOV_EXCL_BR_LINE
  res.requestBody =
      get_optional_string(obj, "requestBody"); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_server;
  if (obj["server"].get(el_server) == simdjson::SUCCESS && // GCOV_EXCL_LINE
      el_server.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_LINE //
                                                 // GCOV_EXCL_BR_LINE
    res.server = parse_Server(
        el_server.get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  simdjson::dom::element el_ref; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Response
parse_Response(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  Response res;                                      // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description").value_or(""); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_headers;                        // GCOV_EXCL_BR_LINE
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS &&
      el_headers.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, Header> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_headers.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Header(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.headers = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_content; // GCOV_EXCL_BR_LINE
  if (obj["content"].get(el_content) == simdjson::SUCCESS &&
      el_content.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, MediaType> m;          // GCOV_EXCL_BR_LINE
    for (auto field : el_content.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_MediaType(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.content = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_links;                       // GCOV_EXCL_BR_LINE
  if (obj["links"].get(el_links) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_links.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, Link> m;
    for (auto field : el_links.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] = parse_Link(
            field.value.get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.links = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  simdjson::dom::element el_ref; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Parameter
parse_Parameter(simdjson::dom::object obj) noexcept {       // GCOV_EXCL_BR_LINE
  Parameter res;                                            // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name").value_or(""); // GCOV_EXCL_BR_LINE
  res.in = get_optional_string(obj, "in").value_or("");     // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  res.required =
      get_optional_bool(obj, "required").value_or(false); // GCOV_EXCL_BR_LINE
  res.deprecated =
      get_optional_bool(obj, "deprecated").value_or(false); // GCOV_EXCL_BR_LINE
  res.allowEmptyValue =                                     // GCOV_EXCL_BR_LINE
      get_optional_bool(obj, "allowEmptyValue")
          .value_or(false);                        // GCOV_EXCL_BR_LINE
  res.style = get_optional_string(obj, "style");   // GCOV_EXCL_BR_LINE
  res.explode = get_optional_bool(obj, "explode"); // GCOV_EXCL_BR_LINE
  res.allowReserved =
      get_optional_bool(obj, "allowReserved");             // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_schema;                        // GCOV_EXCL_BR_LINE
  if (obj["schema"].get(el_schema) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_schema.type() == simdjson::dom::element_type::OBJECT) {
    res.schema = parse_Schema(el_schema.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.example = get_optional_string(obj, "example"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_examples;                // GCOV_EXCL_BR_LINE
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() ==
          simdjson::dom::element_type::OBJECT) {  // GCOV_EXCL_BR_LINE
    std::map<std::string, Example> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_examples.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Example(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.examples = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_content; // GCOV_EXCL_BR_LINE
  if (obj["content"].get(el_content) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_content.type() == simdjson::dom::element_type::OBJECT) {
    std::map<std::string, MediaType> m;
    for (auto field : el_content.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] = parse_MediaType(
            field.value.get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.content = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  simdjson::dom::element el_ref; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE
} // GCOV_EXCL_LINE

static OAuthFlow
parse_OAuthFlow(simdjson::dom::object obj) noexcept { // GCOV_EXCL_LINE
  OAuthFlow res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.authorizationUrl = get_optional_string(
      obj, "authorizationUrl"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.tokenUrl = get_optional_string(
      obj, "tokenUrl"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.refreshUrl = get_optional_string(
      obj, "refreshUrl");      // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.deviceAuthorizationUrl = // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      get_optional_string(
          obj, "deviceAuthorizationUrl"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.scopes =
      parse_map_string(obj, "scopes"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  return res;                          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static OAuthFlows parse_OAuthFlows(simdjson::dom::object obj) noexcept {
  OAuthFlows res;                     // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_implicit; // GCOV_EXCL_BR_LINE
  if (obj["implicit"].get(el_implicit) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_implicit.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.implicit =
        parse_OAuthFlow(el_implicit.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_password; // GCOV_EXCL_BR_LINE
  if (obj["password"].get(el_password) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_password.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.password =
        parse_OAuthFlow(el_password.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_clientCredentials; // GCOV_EXCL_BR_LINE
  if (obj["clientCredentials"].get(el_clientCredentials) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_clientCredentials.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.clientCredentials =
        parse_OAuthFlow(el_clientCredentials.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_authorizationCode;
  if (obj["authorizationCode"].get(el_authorizationCode) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_authorizationCode.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.authorizationCode =
        parse_OAuthFlow(el_authorizationCode.get_object()); // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  simdjson::dom::element
      el_deviceAuthorization; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["deviceAuthorization"].get(
          el_deviceAuthorization) == // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
          simdjson::SUCCESS &&       // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      el_deviceAuthorization.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_LINE //
                                                 // GCOV_EXCL_BR_LINE
    res.deviceAuthorization = // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
        parse_OAuthFlow(
            el_deviceAuthorization
                .get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static SecurityScheme
parse_SecurityScheme(simdjson::dom::object obj) noexcept {  // GCOV_EXCL_BR_LINE
  SecurityScheme res;                                       // GCOV_EXCL_BR_LINE
  res.type = get_optional_string(obj, "type").value_or(""); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description");     // GCOV_EXCL_BR_LINE
  res.name = get_optional_string(obj, "name");     // GCOV_EXCL_BR_LINE
  res.in = get_optional_string(obj, "in");         // GCOV_EXCL_BR_LINE
  res.scheme = get_optional_string(obj, "scheme"); // GCOV_EXCL_BR_LINE
  res.bearerFormat =
      get_optional_string(obj, "bearerFormat"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_flows;
  if (obj["flows"].get(el_flows) == simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_flows.type() ==
          simdjson::dom::element_type::OBJECT) {         // GCOV_EXCL_BR_LINE
    res.flows = parse_OAuthFlows(el_flows.get_object()); // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  res.openIdConnectUrl = get_optional_string(
      obj, "openIdConnectUrl"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.oauth2MetadataUrl = get_optional_string(
      obj, "oauth2MetadataUrl"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  res.deprecated = get_optional_bool(
      obj, "deprecated");        // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_ref; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  if (obj["$ref"].get(el_ref) ==
      simdjson::SUCCESS) {          // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    res.ref = parse_Reference(obj); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Operation
parse_Operation(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  Operation res;                                      // GCOV_EXCL_BR_LINE
  res.tags = parse_vector_string(obj, "tags");        // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary");  // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_externalDocs;      // GCOV_EXCL_BR_LINE
  if (obj["externalDocs"].get(el_externalDocs) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_externalDocs.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.externalDocs =                           // GCOV_EXCL_BR_LINE
        parse_ExternalDocumentation(
            el_externalDocs.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.operationId =
      get_optional_string(obj, "operationId"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_parameters;        // GCOV_EXCL_BR_LINE
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS &&
      el_parameters.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<Parameter> vec;
    for (auto item : el_parameters.get_array()) { // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT)             // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Parameter(item.get_object())); // GCOV_EXCL_BR_LINE
    }
    res.parameters = vec; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_requestBody; // GCOV_EXCL_BR_LINE
  if (obj["requestBody"].get(el_requestBody) == simdjson::SUCCESS &&
      el_requestBody.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.requestBody =
        parse_RequestBody(el_requestBody.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_responses; // GCOV_EXCL_BR_LINE
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS &&
      el_responses.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, Response> m;
    for (auto field : el_responses.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Response(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.responses = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_callbacks; // GCOV_EXCL_BR_LINE
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS &&
      el_callbacks.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<                   // GCOV_EXCL_BR_LINE
        std::map<std::string,
                 std::map<std::string, PathItem>>>(); // GCOV_EXCL_LINE
    for (auto field : el_callbacks.get_object()) {    // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) {            // GCOV_EXCL_BR_LINE
        std::map<std::string, PathItem> inner_m;            // GCOV_EXCL_BR_LINE
        for (auto inner_field : field.value.get_object()) { // GCOV_EXCL_BR_LINE
          if (inner_field.value.type() ==
              simdjson::dom::element_type::OBJECT)  // GCOV_EXCL_BR_LINE
            inner_m[std::string(inner_field.key)] = // GCOV_EXCL_BR_LINE
                parse_PathItem(
                    inner_field.value.get_object()); // GCOV_EXCL_BR_LINE
        }
        (*m)[std::string(field.key)] = inner_m; // GCOV_EXCL_BR_LINE
      }
    }
    res.callbacks = m; // GCOV_EXCL_BR_LINE
  }
  res.deprecated =
      get_optional_bool(obj, "deprecated").value_or(false); // GCOV_EXCL_BR_LINE
  res.security = parse_security(obj, "security");           // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_servers;                        // GCOV_EXCL_BR_LINE
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_LINE
    std::vector<Server> vec; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    for (auto item :
         el_servers.get_array()) { // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_LINE //
                                               // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Server(
            item.get_object())); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.servers = vec; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static PathItem
parse_PathItem(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  PathItem res;                                      // GCOV_EXCL_BR_LINE
  res.ref = get_optional_string(obj, "$ref");        // GCOV_EXCL_BR_LINE
  res.summary = get_optional_string(obj, "summary"); // GCOV_EXCL_BR_LINE
  res.description =
      get_optional_string(obj, "description"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_get;               // GCOV_EXCL_BR_LINE
  if (obj["get"].get(el_get) == simdjson::SUCCESS &&
      el_get.type() ==
          simdjson::dom::element_type::OBJECT) {    // GCOV_EXCL_BR_LINE
    res.get = parse_Operation(el_get.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_put; // GCOV_EXCL_BR_LINE
  if (obj["put"].get(el_put) == simdjson::SUCCESS &&
      el_put.type() ==
          simdjson::dom::element_type::OBJECT) {    // GCOV_EXCL_BR_LINE
    res.put = parse_Operation(el_put.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_post; // GCOV_EXCL_BR_LINE
  if (obj["post"].get(el_post) == simdjson::SUCCESS &&
      el_post.type() ==
          simdjson::dom::element_type::OBJECT) {      // GCOV_EXCL_BR_LINE
    res.post = parse_Operation(el_post.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_delete_op; // GCOV_EXCL_BR_LINE
  if (obj["delete"].get(el_delete_op) == simdjson::SUCCESS &&
      el_delete_op.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.delete_op =
        parse_Operation(el_delete_op.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_options; // GCOV_EXCL_BR_LINE
  if (obj["options"].get(el_options) == simdjson::SUCCESS &&
      el_options.type() ==
          simdjson::dom::element_type::OBJECT) {            // GCOV_EXCL_BR_LINE
    res.options = parse_Operation(el_options.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_head; // GCOV_EXCL_BR_LINE
  if (obj["head"].get(el_head) == simdjson::SUCCESS &&
      el_head.type() ==
          simdjson::dom::element_type::OBJECT) {      // GCOV_EXCL_BR_LINE
    res.head = parse_Operation(el_head.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_patch; // GCOV_EXCL_BR_LINE
  if (obj["patch"].get(el_patch) == simdjson::SUCCESS &&
      el_patch.type() ==
          simdjson::dom::element_type::OBJECT) {        // GCOV_EXCL_BR_LINE
    res.patch = parse_Operation(el_patch.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_trace; // GCOV_EXCL_BR_LINE
  if (obj["trace"].get(el_trace) == simdjson::SUCCESS &&
      el_trace.type() ==
          simdjson::dom::element_type::OBJECT) {        // GCOV_EXCL_BR_LINE
    res.trace = parse_Operation(el_trace.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_query; // GCOV_EXCL_BR_LINE
  if (obj["query"].get(el_query) == simdjson::SUCCESS &&
      el_query.type() ==
          simdjson::dom::element_type::OBJECT) {        // GCOV_EXCL_BR_LINE
    res.query = parse_Operation(el_query.get_object()); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_servers; // GCOV_EXCL_BR_LINE
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<Server> vec;                    // GCOV_EXCL_BR_LINE
    for (auto item : el_servers.get_array()) {  // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT)          // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Server(item.get_object())); // GCOV_EXCL_BR_LINE
    }
    res.servers = vec; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_parameters; // GCOV_EXCL_BR_LINE
  if (obj["parameters"].get(el_parameters) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_parameters.type() ==
          simdjson::dom::element_type::ARRAY) {   // GCOV_EXCL_BR_LINE
    std::vector<Parameter> vec;                   // GCOV_EXCL_BR_LINE
    for (auto item : el_parameters.get_array()) { // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT)             // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Parameter(item.get_object())); // GCOV_EXCL_BR_LINE
    }
    res.parameters = vec; // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_addOps;
  if (obj["additionalOperations"].get(el_addOps) == simdjson::SUCCESS &&
      el_addOps.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<
        std::map<std::string, Operation>>(); // GCOV_EXCL_BR_LINE
    for (auto field :
         el_addOps.get_object()) { // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_LINE //
                                               // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] = // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
            parse_Operation(
                field.value
                    .get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.additionalOperations = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static Components parse_Components(simdjson::dom::object obj) noexcept {
  Components res;                    // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_schemas; // GCOV_EXCL_BR_LINE
  if (obj["schemas"].get(el_schemas) == simdjson::SUCCESS &&
      el_schemas.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, Schema> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_schemas.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Schema(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.schemas = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_responses; // GCOV_EXCL_BR_LINE
  if (obj["responses"].get(el_responses) == simdjson::SUCCESS &&
      el_responses.type() ==
          simdjson::dom::element_type::OBJECT) {   // GCOV_EXCL_BR_LINE
    std::map<std::string, Response> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_responses.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Response(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.responses = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_parameters; // GCOV_EXCL_BR_LINE
  if (obj["parameters"].get(el_parameters) == simdjson::SUCCESS &&
      el_parameters.type() ==
          simdjson::dom::element_type::OBJECT) {    // GCOV_EXCL_BR_LINE
    std::map<std::string, Parameter> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_parameters.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Parameter(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.parameters = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_examples; // GCOV_EXCL_BR_LINE
  if (obj["examples"].get(el_examples) == simdjson::SUCCESS &&
      el_examples.type() ==
          simdjson::dom::element_type::OBJECT) {  // GCOV_EXCL_BR_LINE
    std::map<std::string, Example> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_examples.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Example(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.examples = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_requestBodies; // GCOV_EXCL_BR_LINE
  if (obj["requestBodies"].get(el_requestBodies) == simdjson::SUCCESS &&
      el_requestBodies.type() ==
          simdjson::dom::element_type::OBJECT) {       // GCOV_EXCL_BR_LINE
    std::map<std::string, RequestBody> m;              // GCOV_EXCL_BR_LINE
    for (auto field : el_requestBodies.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_RequestBody(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.requestBodies = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_headers; // GCOV_EXCL_BR_LINE
  if (obj["headers"].get(el_headers) == simdjson::SUCCESS &&
      el_headers.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, Header> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_headers.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Header(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.headers = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_securitySchemes; // GCOV_EXCL_BR_LINE
  if (obj["securitySchemes"].get(el_securitySchemes) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_securitySchemes.type() ==
          simdjson::dom::element_type::OBJECT) {         // GCOV_EXCL_BR_LINE
    std::map<std::string, SecurityScheme> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_securitySchemes.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT)              // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =                         // GCOV_EXCL_BR_LINE
            parse_SecurityScheme(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.securitySchemes = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_links; // GCOV_EXCL_BR_LINE
  if (obj["links"].get(el_links) == simdjson::SUCCESS &&
      el_links.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, Link> m;
    for (auto field : el_links.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_Link(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.links = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_callbacks; // GCOV_EXCL_BR_LINE
  if (obj["callbacks"].get(el_callbacks) == simdjson::SUCCESS &&
      el_callbacks.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<                   // GCOV_EXCL_BR_LINE
        std::map<std::string,
                 std::map<std::string, PathItem>>>(); // GCOV_EXCL_LINE
    for (auto field : el_callbacks.get_object()) {    // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) {            // GCOV_EXCL_BR_LINE
        std::map<std::string, PathItem> inner_m;            // GCOV_EXCL_BR_LINE
        for (auto inner_field : field.value.get_object()) { // GCOV_EXCL_BR_LINE
          if (inner_field.value.type() ==
              simdjson::dom::element_type::OBJECT)  // GCOV_EXCL_BR_LINE
            inner_m[std::string(inner_field.key)] = // GCOV_EXCL_BR_LINE
                parse_PathItem(
                    inner_field.value.get_object()); // GCOV_EXCL_BR_LINE
        }
        (*m)[std::string(field.key)] = inner_m; // GCOV_EXCL_BR_LINE
      }
    }
    res.callbacks = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_pathItems; // GCOV_EXCL_BR_LINE
  if (obj["pathItems"].get(el_pathItems) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_pathItems.type() ==
          simdjson::dom::element_type::OBJECT) {   // GCOV_EXCL_BR_LINE
    std::map<std::string, PathItem> m;             // GCOV_EXCL_BR_LINE
    for (auto field : el_pathItems.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_PathItem(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.pathItems = m; // GCOV_EXCL_BR_LINE
  }
  // GCOV_EXCL_LINE
  simdjson::dom::element el_mediaTypes;
  if (obj["mediaTypes"].get(el_mediaTypes) == simdjson::SUCCESS &&
      el_mediaTypes.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    auto m = std::make_shared<
        std::map<std::string, MediaType>>(); // GCOV_EXCL_BR_LINE
    for (auto field :
         el_mediaTypes.get_object()) { // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_LINE //
                                               // GCOV_EXCL_BR_LINE
        (*m)[std::string(field.key)] = // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
            parse_MediaType(
                field.value
                    .get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_LINE
    res.mediaTypes = m; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE
} // GCOV_EXCL_LINE
  // GCOV_EXCL_LINE
static OpenAPI
parse_OpenAPI(simdjson::dom::object obj) noexcept { // GCOV_EXCL_BR_LINE
  OpenAPI res;                                      // GCOV_EXCL_BR_LINE
  res.openapi =
      get_optional_string(obj, "openapi").value_or(""); // GCOV_EXCL_BR_LINE
  res.self_link = get_optional_string(obj, "$self");    // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_info;                       // GCOV_EXCL_BR_LINE
  if (obj["info"].get(el_info) == simdjson::SUCCESS &&  // GCOV_EXCL_BR_LINE
      el_info.type() == simdjson::dom::element_type::OBJECT) {
    res.info = parse_Info(el_info.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.jsonSchemaDialect =
      get_optional_string(obj, "jsonSchemaDialect"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_servers;                 // GCOV_EXCL_BR_LINE
  if (obj["servers"].get(el_servers) == simdjson::SUCCESS &&
      el_servers.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<Server> vec;                    // GCOV_EXCL_BR_LINE
    for (auto item : el_servers.get_array()) {  // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT)          // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Server(item.get_object())); // GCOV_EXCL_BR_LINE
    }
    res.servers = vec; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_paths; // GCOV_EXCL_BR_LINE
  if (obj["paths"].get(el_paths) == simdjson::SUCCESS &&
      el_paths.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, PathItem> m;           // GCOV_EXCL_BR_LINE
    for (auto field : el_paths.get_object()) {   // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_PathItem(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.paths = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_webhooks; // GCOV_EXCL_BR_LINE
  if (obj["webhooks"].get(el_webhooks) == simdjson::SUCCESS &&
      el_webhooks.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    std::map<std::string, PathItem> m;
    for (auto field : el_webhooks.get_object()) { // GCOV_EXCL_BR_LINE
      if (field.value.type() ==
          simdjson::dom::element_type::OBJECT) // GCOV_EXCL_BR_LINE
        m[std::string(field.key)] =
            parse_PathItem(field.value.get_object()); // GCOV_EXCL_BR_LINE
    }
    res.webhooks = m; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_components; // GCOV_EXCL_BR_LINE
  if (obj["components"].get(el_components) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_components.type() == simdjson::dom::element_type::OBJECT) {
    res.components =
        parse_Components(el_components.get_object()); // GCOV_EXCL_BR_LINE
  }
  res.security = parse_security(obj, "security"); // GCOV_EXCL_BR_LINE
  simdjson::dom::element el_tags;                 // GCOV_EXCL_BR_LINE
  if (obj["tags"].get(el_tags) == simdjson::SUCCESS &&
      el_tags.type() ==
          simdjson::dom::element_type::ARRAY) { // GCOV_EXCL_BR_LINE
    std::vector<Tag> vec;                       // GCOV_EXCL_BR_LINE
    for (auto item : el_tags.get_array()) {     // GCOV_EXCL_BR_LINE
      if (item.type() ==
          simdjson::dom::element_type::OBJECT)       // GCOV_EXCL_BR_LINE
        vec.push_back(parse_Tag(item.get_object())); // GCOV_EXCL_BR_LINE
    }
    res.tags = vec; // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element el_externalDocs; // GCOV_EXCL_BR_LINE
  if (obj["externalDocs"].get(el_externalDocs) ==
          simdjson::SUCCESS && // GCOV_EXCL_BR_LINE
      el_externalDocs.type() ==
          simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
    res.externalDocs =                           // GCOV_EXCL_BR_LINE
        parse_ExternalDocumentation(
            el_externalDocs
                .get_object()); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_LINE
  return res; // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE
} // GCOV_EXCL_LINE

std::expected<OpenAPI, std::string> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  simdjson::simdjson_result<simdjson::dom::element> res =
      parser.parse(input); // GCOV_EXCL_BR_LINE
  if (res.error() != simdjson::SUCCESS) {
    return std::unexpected("Failed to parse JSON"); // GCOV_EXCL_BR_LINE
  }
  simdjson::dom::element doc = res.value_unsafe(); // GCOV_EXCL_BR_LINE
  if (doc.type() != simdjson::dom::element_type::OBJECT)
    return std::unexpected(
        "OpenAPI doc must be an object"); // GCOV_EXCL_BR_LINE
  return parse_OpenAPI(doc.get_object()); // GCOV_EXCL_BR_LINE
}
} // namespace cdd_cpp::openapi
