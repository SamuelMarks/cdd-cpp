#include "emit.hpp"
#include "../utils/json_writer.hpp"

namespace cdd_cpp::openapi {

void emit_Contact(utils::JsonWriter &jw, const Contact &obj) noexcept;
void emit_License(utils::JsonWriter &jw, const License &obj) noexcept;
void emit_Info(utils::JsonWriter &jw, const Info &obj) noexcept;
void emit_ServerVariable(utils::JsonWriter &jw,
                         const ServerVariable &obj) noexcept;
void emit_Server(utils::JsonWriter &jw, const Server &obj) noexcept;
void emit_ExternalDocumentation(utils::JsonWriter &jw,
                                const ExternalDocumentation &obj) noexcept;
void emit_Tag(utils::JsonWriter &jw, const Tag &obj) noexcept;
void emit_Discriminator(utils::JsonWriter &jw,
                        const Discriminator &obj) noexcept;
void emit_XML(utils::JsonWriter &jw, const XML &obj) noexcept;
void emit_Schema(utils::JsonWriter &jw, const Schema &obj) noexcept;
void emit_Example(utils::JsonWriter &jw, const Example &obj) noexcept;
void emit_Encoding(utils::JsonWriter &jw, const Encoding &obj) noexcept;
void emit_MediaType(utils::JsonWriter &jw, const MediaType &obj) noexcept;
void emit_Header(utils::JsonWriter &jw, const Header &obj) noexcept;
void emit_RequestBody(utils::JsonWriter &jw, const RequestBody &obj) noexcept;
void emit_Link(utils::JsonWriter &jw, const Link &obj) noexcept;
void emit_Response(utils::JsonWriter &jw, const Response &obj) noexcept;
void emit_Parameter(utils::JsonWriter &jw, const Parameter &obj) noexcept;
void emit_OAuthFlow(utils::JsonWriter &jw, const OAuthFlow &obj) noexcept;
void emit_OAuthFlows(utils::JsonWriter &jw, const OAuthFlows &obj) noexcept;
void emit_SecurityScheme(utils::JsonWriter &jw,
                         const SecurityScheme &obj) noexcept;
void emit_Operation(utils::JsonWriter &jw, const Operation &obj) noexcept;
void emit_PathItem(utils::JsonWriter &jw, const PathItem &obj) noexcept;
void emit_Components(utils::JsonWriter &jw, const Components &obj) noexcept;
void emit_OpenAPI(utils::JsonWriter &jw, const OpenAPI &obj) noexcept;

void emit_Contact(utils::JsonWriter &jw, const Contact &obj) noexcept {
  jw.start_object();                   // GCOV_EXCL_BR_LINE
  jw.key_optional("name", obj.name);   // GCOV_EXCL_BR_LINE
  jw.key_optional("url", obj.url);     // GCOV_EXCL_BR_LINE
  jw.key_optional("email", obj.email); // GCOV_EXCL_BR_LINE
  jw.end_object();                     // GCOV_EXCL_BR_LINE
}

void emit_License(utils::JsonWriter &jw, const License &obj) noexcept {
  jw.start_object();                             // GCOV_EXCL_BR_LINE
  jw.key_value("name", obj.name);                // GCOV_EXCL_BR_LINE
  jw.key_optional("identifier", obj.identifier); // GCOV_EXCL_BR_LINE
  jw.key_optional("url", obj.url);               // GCOV_EXCL_BR_LINE
  jw.end_object();                               // GCOV_EXCL_BR_LINE
}

void emit_Info(utils::JsonWriter &jw, const Info &obj) noexcept {
  jw.start_object();                                     // GCOV_EXCL_BR_LINE
  jw.key_value("title", obj.title);                      // GCOV_EXCL_BR_LINE
  jw.key_optional("summary", obj.summary);               // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description);       // GCOV_EXCL_BR_LINE
  jw.key_optional("termsOfService", obj.termsOfService); // GCOV_EXCL_BR_LINE
  if (obj.contact.has_value()) {                         // GCOV_EXCL_BR_LINE
    jw.key("contact");                                   // GCOV_EXCL_BR_LINE
    emit_Contact(jw, obj.contact.value());               // GCOV_EXCL_BR_LINE
  }
  if (obj.license.has_value()) {           // GCOV_EXCL_BR_LINE
    jw.key("license");                     // GCOV_EXCL_BR_LINE
    emit_License(jw, obj.license.value()); // GCOV_EXCL_BR_LINE
  }
  jw.key_value("version", obj.version); // GCOV_EXCL_BR_LINE
  jw.end_object();                      // GCOV_EXCL_BR_LINE
}

void emit_ServerVariable(utils::JsonWriter &jw,
                         const ServerVariable &obj) noexcept {
  jw.start_object(); // GCOV_EXCL_BR_LINE
  if (obj.enum_values.has_value() &&
      !obj.enum_values->empty()) {                   // GCOV_EXCL_BR_LINE
    jw.key("enum");                                  // GCOV_EXCL_BR_LINE
    jw.start_array();                                // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.enum_values.value()) // GCOV_EXCL_BR_LINE
      jw.value(item);                                // GCOV_EXCL_BR_LINE
    jw.end_array();                                  // GCOV_EXCL_BR_LINE
  }
  jw.key_value("default", obj.default_value);      // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  jw.end_object();                                 // GCOV_EXCL_BR_LINE
}

void emit_Server(utils::JsonWriter &jw, const Server &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_value("url", obj.url);                    // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  jw.key_optional("name", obj.name);               // GCOV_EXCL_BR_LINE
  if (obj.variables.has_value() &&
      !obj.variables->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("variables");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                 // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.variables.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                       // GCOV_EXCL_BR_LINE
      emit_ServerVariable(jw, v);                      // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_ExternalDocumentation(utils::JsonWriter &jw,
                                const ExternalDocumentation &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  jw.key_value("url", obj.url);                    // GCOV_EXCL_BR_LINE
  jw.end_object();                                 // GCOV_EXCL_BR_LINE
}

void emit_Tag(utils::JsonWriter &jw, const Tag &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_value("name", obj.name);                  // GCOV_EXCL_BR_LINE
  jw.key_optional("summary", obj.summary);         // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  jw.key_optional("parent", obj.parent);           // GCOV_EXCL_BR_LINE
  jw.key_optional("kind", obj.kind);               // GCOV_EXCL_BR_LINE
  if (obj.externalDocs.has_value()) {              // GCOV_EXCL_BR_LINE
    jw.key("externalDocs");                        // GCOV_EXCL_BR_LINE
    emit_ExternalDocumentation(jw,
                               obj.externalDocs.value()); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}
void emit_Discriminator(utils::JsonWriter &jw,
                        const Discriminator &obj) noexcept {
  jw.start_object();                                      // GCOV_EXCL_BR_LINE
  jw.key_value("propertyName", obj.propertyName);         // GCOV_EXCL_BR_LINE
  if (obj.mapping.has_value() && !obj.mapping->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("mapping");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.mapping.value()) {      // GCOV_EXCL_BR_LINE
      jw.key_value(k, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("defaultMapping", obj.defaultMapping); // GCOV_EXCL_BR_LINE
  jw.end_object();                                       // GCOV_EXCL_BR_LINE
}

void emit_XML(utils::JsonWriter &jw, const XML &obj) noexcept {
  jw.start_object();                                  // GCOV_EXCL_BR_LINE
  jw.key_optional("name", obj.name);                  // GCOV_EXCL_BR_LINE
  jw.key_optional("namespace", obj.namespace_url);    // GCOV_EXCL_BR_LINE
  jw.key_optional("prefix", obj.prefix);              // GCOV_EXCL_BR_LINE
  if (obj.attribute.has_value())                      // GCOV_EXCL_BR_LINE
    jw.key_value("attribute", obj.attribute.value()); // GCOV_EXCL_BR_LINE
  if (obj.wrapped.has_value())                        // GCOV_EXCL_BR_LINE
    jw.key_value("wrapped", obj.wrapped.value());     // GCOV_EXCL_BR_LINE
  jw.end_object();                                    // GCOV_EXCL_BR_LINE
}

void emit_Schema(utils::JsonWriter &jw, const Schema &obj) noexcept {
  jw.start_object();                                     // GCOV_EXCL_BR_LINE
  jw.key_optional("type", obj.type);                     // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description);       // GCOV_EXCL_BR_LINE
  jw.key_optional("format", obj.format);                 // GCOV_EXCL_BR_LINE
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  if (obj.discriminator.has_value()) {                 // GCOV_EXCL_BR_LINE
    jw.key("discriminator");                           // GCOV_EXCL_BR_LINE
    emit_Discriminator(jw, obj.discriminator.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.xml.has_value()) {       // GCOV_EXCL_BR_LINE
    jw.key("xml");                 // GCOV_EXCL_BR_LINE
    emit_XML(jw, obj.xml.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.properties != nullptr &&
      !obj.properties->empty()) {                // GCOV_EXCL_BR_LINE
    jw.key("properties");                        // GCOV_EXCL_BR_LINE
    jw.start_object();                           // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.properties) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                 // GCOV_EXCL_BR_LINE
      emit_Schema(jw, v);                        // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.items != nullptr) {    // GCOV_EXCL_BR_LINE
    jw.key("items");             // GCOV_EXCL_BR_LINE
    emit_Schema(jw, *obj.items); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Example(utils::JsonWriter &jw, const Example &obj) noexcept {
  jw.start_object();                                     // GCOV_EXCL_BR_LINE
  jw.key_optional("summary", obj.summary);               // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description);       // GCOV_EXCL_BR_LINE
  jw.key_optional("value", obj.value);                   // GCOV_EXCL_BR_LINE
  jw.key_optional("externalValue", obj.externalValue);   // GCOV_EXCL_BR_LINE
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Encoding(utils::JsonWriter &jw, const Encoding &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_optional("contentType", obj.contentType); // GCOV_EXCL_BR_LINE
  jw.key_optional("style", obj.style);             // GCOV_EXCL_BR_LINE
  if (obj.explode.has_value())                     // GCOV_EXCL_BR_LINE
    jw.key_value("explode", obj.explode.value());  // GCOV_EXCL_BR_LINE
  if (obj.allowReserved.has_value())               // GCOV_EXCL_BR_LINE
    jw.key_value("allowReserved",
                 obj.allowReserved.value());             // GCOV_EXCL_BR_LINE
  if (obj.headers != nullptr && !obj.headers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("headers");                                   // GCOV_EXCL_BR_LINE
    jw.start_object();                                   // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.headers) {            // GCOV_EXCL_BR_LINE
      jw.key(k);                                         // GCOV_EXCL_BR_LINE
      emit_Header(jw, v);                                // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.encoding != nullptr && !obj.encoding->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("encoding");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                     // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.encoding) {             // GCOV_EXCL_BR_LINE
      jw.key(k);                                           // GCOV_EXCL_BR_LINE
      emit_Encoding(jw, v);                                // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.itemSchema != nullptr) {    // GCOV_EXCL_BR_LINE
    jw.key("itemSchema");             // GCOV_EXCL_BR_LINE
    emit_Schema(jw, *obj.itemSchema); // GCOV_EXCL_BR_LINE
  }
  if (obj.prefixEncoding != nullptr &&
      !obj.prefixEncoding->empty()) {           // GCOV_EXCL_BR_LINE
    jw.key("prefixEncoding");                   // GCOV_EXCL_BR_LINE
    jw.start_array();                           // GCOV_EXCL_BR_LINE
    for (const auto &e : *obj.prefixEncoding) { // GCOV_EXCL_BR_LINE
      emit_Encoding(jw, e);                     // GCOV_EXCL_BR_LINE
    }
    jw.end_array(); // GCOV_EXCL_BR_LINE
  }
  if (obj.itemEncoding != nullptr) {      // GCOV_EXCL_BR_LINE
    jw.key("itemEncoding");               // GCOV_EXCL_BR_LINE
    emit_Encoding(jw, *obj.itemEncoding); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_MediaType(utils::JsonWriter &jw, const MediaType &obj) noexcept {
  jw.start_object();                     // GCOV_EXCL_BR_LINE
  if (obj.schema.has_value()) {          // GCOV_EXCL_BR_LINE
    jw.key("schema");                    // GCOV_EXCL_BR_LINE
    emit_Schema(jw, obj.schema.value()); // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("example", obj.example);                  // GCOV_EXCL_BR_LINE
  if (obj.examples.has_value() && !obj.examples->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("examples");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.examples.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_Example(jw, v);                                  // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.encoding != nullptr && !obj.encoding->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("encoding");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                     // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.encoding) {             // GCOV_EXCL_BR_LINE
      jw.key(k);                                           // GCOV_EXCL_BR_LINE
      emit_Encoding(jw, v);                                // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.itemSchema != nullptr) {    // GCOV_EXCL_BR_LINE
    jw.key("itemSchema");             // GCOV_EXCL_BR_LINE
    emit_Schema(jw, *obj.itemSchema); // GCOV_EXCL_BR_LINE
  }
  if (obj.prefixEncoding != nullptr &&
      !obj.prefixEncoding->empty()) {           // GCOV_EXCL_BR_LINE
    jw.key("prefixEncoding");                   // GCOV_EXCL_BR_LINE
    jw.start_array();                           // GCOV_EXCL_BR_LINE
    for (const auto &e : *obj.prefixEncoding) { // GCOV_EXCL_BR_LINE
      emit_Encoding(jw, e);                     // GCOV_EXCL_BR_LINE
    }
    jw.end_array(); // GCOV_EXCL_BR_LINE
  }
  if (obj.itemEncoding != nullptr) {      // GCOV_EXCL_BR_LINE
    jw.key("itemEncoding");               // GCOV_EXCL_BR_LINE
    emit_Encoding(jw, *obj.itemEncoding); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Header(utils::JsonWriter &jw, const Header &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (obj.required)                                // GCOV_EXCL_BR_LINE
    jw.key_value("required", true);                // GCOV_EXCL_BR_LINE
  if (obj.deprecated)                              // GCOV_EXCL_BR_LINE
    jw.key_value("deprecated", true);              // GCOV_EXCL_BR_LINE
  if (obj.allowEmptyValue)                         // GCOV_EXCL_BR_LINE
    jw.key_value("allowEmptyValue", true);         // GCOV_EXCL_BR_LINE
  jw.key_optional("style", obj.style);             // GCOV_EXCL_BR_LINE
  if (obj.explode.has_value())                     // GCOV_EXCL_BR_LINE
    jw.key_value("explode", obj.explode.value());  // GCOV_EXCL_BR_LINE
  if (obj.allowReserved.has_value())               // GCOV_EXCL_BR_LINE
    jw.key_value("allowReserved",
                 obj.allowReserved.value()); // GCOV_EXCL_BR_LINE
  if (obj.schema.has_value()) {              // GCOV_EXCL_BR_LINE
    jw.key("schema");                        // GCOV_EXCL_BR_LINE
    emit_Schema(jw, obj.schema.value());     // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("example", obj.example);                  // GCOV_EXCL_BR_LINE
  if (obj.examples.has_value() && !obj.examples->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("examples");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.examples.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_Example(jw, v);                                  // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.content.has_value() && !obj.content->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("content");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.content.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_MediaType(jw, v);                              // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_RequestBody(utils::JsonWriter &jw, const RequestBody &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (!obj.content.empty()) {                      // GCOV_EXCL_BR_LINE
    jw.key("content");                             // GCOV_EXCL_BR_LINE
    jw.start_object();                             // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.content) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                   // GCOV_EXCL_BR_LINE
      emit_MediaType(jw, v);                       // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.required)                                      // GCOV_EXCL_BR_LINE
    jw.key_value("required", true);                      // GCOV_EXCL_BR_LINE
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Link(utils::JsonWriter &jw, const Link &obj) noexcept {
  jw.start_object();                                 // GCOV_EXCL_BR_LINE
  jw.key_optional("operationRef", obj.operationRef); // GCOV_EXCL_BR_LINE
  jw.key_optional("operationId", obj.operationId);   // GCOV_EXCL_BR_LINE
  if (obj.parameters.has_value() &&
      !obj.parameters->empty()) {                     // GCOV_EXCL_BR_LINE
    jw.key("parameters");                             // GCOV_EXCL_BR_LINE
    jw.start_object();                                // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.parameters.value()) // GCOV_EXCL_BR_LINE
      jw.key_value(k, v);                             // GCOV_EXCL_BR_LINE
    jw.end_object();                                  // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("requestBody", obj.requestBody); // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (obj.server.has_value()) {                    // GCOV_EXCL_BR_LINE
    jw.key("server");                              // GCOV_EXCL_BR_LINE
    emit_Server(jw, obj.server.value());           // GCOV_EXCL_BR_LINE
  }
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Response(utils::JsonWriter &jw, const Response &obj) noexcept {
  jw.start_object();                                      // GCOV_EXCL_BR_LINE
  jw.key_value("description", obj.description);           // GCOV_EXCL_BR_LINE
  if (obj.headers.has_value() && !obj.headers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("headers");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.headers.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_Header(jw, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.content.has_value() && !obj.content->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("content");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.content.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_MediaType(jw, v);                              // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.links.has_value() && !obj.links->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("links");                                  // GCOV_EXCL_BR_LINE
    jw.start_object();                                // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.links.value()) {    // GCOV_EXCL_BR_LINE
      jw.key(k);                                      // GCOV_EXCL_BR_LINE
      emit_Link(jw, v);                               // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Parameter(utils::JsonWriter &jw, const Parameter &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_value("name", obj.name);                  // GCOV_EXCL_BR_LINE
  jw.key_value("in", obj.in);                      // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (obj.required)                                // GCOV_EXCL_BR_LINE
    jw.key_value("required", true);                // GCOV_EXCL_BR_LINE
  if (obj.deprecated)                              // GCOV_EXCL_BR_LINE
    jw.key_value("deprecated", true);              // GCOV_EXCL_BR_LINE
  if (obj.allowEmptyValue)                         // GCOV_EXCL_BR_LINE
    jw.key_value("allowEmptyValue", true);         // GCOV_EXCL_BR_LINE
  jw.key_optional("style", obj.style);             // GCOV_EXCL_BR_LINE
  if (obj.explode.has_value())                     // GCOV_EXCL_BR_LINE
    jw.key_value("explode", obj.explode.value());  // GCOV_EXCL_BR_LINE
  if (obj.allowReserved.has_value())               // GCOV_EXCL_BR_LINE
    jw.key_value("allowReserved",
                 obj.allowReserved.value()); // GCOV_EXCL_BR_LINE
  if (obj.schema.has_value()) {              // GCOV_EXCL_BR_LINE
    jw.key("schema");                        // GCOV_EXCL_BR_LINE
    emit_Schema(jw, obj.schema.value());     // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("example", obj.example);                  // GCOV_EXCL_BR_LINE
  if (obj.examples.has_value() && !obj.examples->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("examples");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.examples.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_Example(jw, v);                                  // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.content.has_value() && !obj.content->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("content");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.content.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_MediaType(jw, v);                              // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_OAuthFlow(utils::JsonWriter &jw, const OAuthFlow &obj) noexcept {
  jw.start_object(); // GCOV_EXCL_BR_LINE
  jw.key_optional("authorizationUrl",
                  obj.authorizationUrl);         // GCOV_EXCL_BR_LINE
  jw.key_optional("tokenUrl", obj.tokenUrl);     // GCOV_EXCL_BR_LINE
  jw.key_optional("refreshUrl", obj.refreshUrl); // GCOV_EXCL_BR_LINE
  jw.key_optional("deviceAuthorizationUrl",
                  obj.deviceAuthorizationUrl);          // GCOV_EXCL_BR_LINE
  if (obj.scopes.has_value() && !obj.scopes->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("scopes");                                   // GCOV_EXCL_BR_LINE
    jw.start_object();                                  // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.scopes.value())       // GCOV_EXCL_BR_LINE
      jw.key_value(k, v);                               // GCOV_EXCL_BR_LINE
    jw.end_object();                                    // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_OAuthFlows(utils::JsonWriter &jw, const OAuthFlows &obj) noexcept {
  jw.start_object();                          // GCOV_EXCL_BR_LINE
  if (obj.implicit.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("implicit");                       // GCOV_EXCL_BR_LINE
    emit_OAuthFlow(jw, obj.implicit.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.password.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("password");                       // GCOV_EXCL_BR_LINE
    emit_OAuthFlow(jw, obj.password.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.clientCredentials.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("clientCredentials");                       // GCOV_EXCL_BR_LINE
    emit_OAuthFlow(jw, obj.clientCredentials.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.authorizationCode.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("authorizationCode");                       // GCOV_EXCL_BR_LINE
    emit_OAuthFlow(jw, obj.authorizationCode.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.deviceAuthorization.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("deviceAuthorization");                       // GCOV_EXCL_BR_LINE
    emit_OAuthFlow(jw, obj.deviceAuthorization.value()); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_SecurityScheme(utils::JsonWriter &jw,
                         const SecurityScheme &obj) noexcept {
  jw.start_object();                                 // GCOV_EXCL_BR_LINE
  jw.key_value("type", obj.type);                    // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description);   // GCOV_EXCL_BR_LINE
  jw.key_optional("name", obj.name);                 // GCOV_EXCL_BR_LINE
  jw.key_optional("in", obj.in);                     // GCOV_EXCL_BR_LINE
  jw.key_optional("scheme", obj.scheme);             // GCOV_EXCL_BR_LINE
  jw.key_optional("bearerFormat", obj.bearerFormat); // GCOV_EXCL_BR_LINE
  if (obj.flows.has_value()) {                       // GCOV_EXCL_BR_LINE
    jw.key("flows");                                 // GCOV_EXCL_BR_LINE
    emit_OAuthFlows(jw, obj.flows.value());          // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("openIdConnectUrl",
                  obj.openIdConnectUrl); // GCOV_EXCL_BR_LINE
  jw.key_optional("oauth2MetadataUrl",
                  obj.oauth2MetadataUrl);                // GCOV_EXCL_BR_LINE
  jw.key_optional("deprecated", obj.deprecated);         // GCOV_EXCL_BR_LINE
  if (obj.ref.has_value()) {                             // GCOV_EXCL_BR_LINE
    jw.key_value("$ref", obj.ref.value().ref);           // GCOV_EXCL_BR_LINE
    jw.key_optional("summary", obj.ref.value().summary); // GCOV_EXCL_BR_LINE
    jw.key_optional("description",
                    obj.ref.value().description); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Operation(utils::JsonWriter &jw, const Operation &obj) noexcept {
  jw.start_object();                                // GCOV_EXCL_BR_LINE
  if (obj.tags.has_value() && !obj.tags->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("tags");                                 // GCOV_EXCL_BR_LINE
    jw.start_array();                               // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.tags.value())       // GCOV_EXCL_BR_LINE
      jw.value(item);                               // GCOV_EXCL_BR_LINE
    jw.end_array();                                 // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("summary", obj.summary);         // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (obj.externalDocs.has_value()) {              // GCOV_EXCL_BR_LINE
    jw.key("externalDocs");                        // GCOV_EXCL_BR_LINE
    emit_ExternalDocumentation(jw,
                               obj.externalDocs.value()); // GCOV_EXCL_BR_LINE
  }
  jw.key_optional("operationId", obj.operationId); // GCOV_EXCL_BR_LINE
  if (obj.parameters.has_value() &&
      !obj.parameters->empty()) {                   // GCOV_EXCL_BR_LINE
    jw.key("parameters");                           // GCOV_EXCL_BR_LINE
    jw.start_array();                               // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.parameters.value()) // GCOV_EXCL_BR_LINE
      emit_Parameter(jw, item);                     // GCOV_EXCL_BR_LINE
    jw.end_array();                                 // GCOV_EXCL_BR_LINE
  }
  if (obj.requestBody.has_value()) {               // GCOV_EXCL_BR_LINE
    jw.key("requestBody");                         // GCOV_EXCL_BR_LINE
    emit_RequestBody(jw, obj.requestBody.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.responses.has_value() &&
      !obj.responses->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("responses");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                 // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.responses.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                       // GCOV_EXCL_BR_LINE
      emit_Response(jw, v);                            // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.callbacks != nullptr &&
      !obj.callbacks->empty()) {                // GCOV_EXCL_BR_LINE
    jw.key("callbacks");                        // GCOV_EXCL_BR_LINE
    jw.start_object();                          // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.callbacks) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                // GCOV_EXCL_BR_LINE
      jw.start_object();                        // GCOV_EXCL_BR_LINE
      for (const auto &[ik, iv] : v) {          // GCOV_EXCL_BR_LINE
        jw.key(ik);                             // GCOV_EXCL_BR_LINE
        emit_PathItem(jw, iv);                  // GCOV_EXCL_BR_LINE
      }
      jw.end_object(); // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.deprecated)                                       // GCOV_EXCL_BR_LINE
    jw.key_value("deprecated", true);                       // GCOV_EXCL_BR_LINE
  if (obj.security.has_value() && !obj.security->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("security");                                     // GCOV_EXCL_BR_LINE
    jw.start_array();                                       // GCOV_EXCL_BR_LINE
    for (const auto &req : obj.security.value()) {          // GCOV_EXCL_BR_LINE
      jw.start_object();                                    // GCOV_EXCL_BR_LINE
      for (const auto &[k, scopes] : req) {                 // GCOV_EXCL_BR_LINE
        jw.key(k);                                          // GCOV_EXCL_BR_LINE
        jw.start_array();                                   // GCOV_EXCL_BR_LINE
        for (const auto &s : scopes)                        // GCOV_EXCL_BR_LINE
          jw.value(s);                                      // GCOV_EXCL_BR_LINE
        jw.end_array();                                     // GCOV_EXCL_BR_LINE
      }
      jw.end_object(); // GCOV_EXCL_BR_LINE
    }
    jw.end_array(); // GCOV_EXCL_BR_LINE
  }
  if (obj.servers.has_value() && !obj.servers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("servers");                                    // GCOV_EXCL_BR_LINE
    jw.start_array();                                     // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.servers.value())          // GCOV_EXCL_BR_LINE
      emit_Server(jw, item);                              // GCOV_EXCL_BR_LINE
    jw.end_array();                                       // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_PathItem(utils::JsonWriter &jw, const PathItem &obj) noexcept {
  jw.start_object();                               // GCOV_EXCL_BR_LINE
  jw.key_optional("$ref", obj.ref);                // GCOV_EXCL_BR_LINE
  jw.key_optional("summary", obj.summary);         // GCOV_EXCL_BR_LINE
  jw.key_optional("description", obj.description); // GCOV_EXCL_BR_LINE
  if (obj.get.has_value()) {                       // GCOV_EXCL_BR_LINE
    jw.key("get");                                 // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.get.value());           // GCOV_EXCL_BR_LINE
  }
  if (obj.put.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("put");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.put.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.post.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("post");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.post.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.delete_op.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("delete");                          // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.delete_op.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.options.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("options");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.options.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.head.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("head");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.head.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.patch.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("patch");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.patch.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.trace.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("trace");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.trace.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.query.has_value()) {             // GCOV_EXCL_BR_LINE
    jw.key("query");                       // GCOV_EXCL_BR_LINE
    emit_Operation(jw, obj.query.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.servers.has_value() && !obj.servers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("servers");                                    // GCOV_EXCL_BR_LINE
    jw.start_array();                                     // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.servers.value())          // GCOV_EXCL_BR_LINE
      emit_Server(jw, item);                              // GCOV_EXCL_BR_LINE
    jw.end_array();                                       // GCOV_EXCL_BR_LINE
  }
  if (obj.parameters.has_value() &&
      !obj.parameters->empty()) {                   // GCOV_EXCL_BR_LINE
    jw.key("parameters");                           // GCOV_EXCL_BR_LINE
    jw.start_array();                               // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.parameters.value()) // GCOV_EXCL_BR_LINE
      emit_Parameter(jw, item);                     // GCOV_EXCL_BR_LINE
    jw.end_array();                                 // GCOV_EXCL_BR_LINE
  }
  if (obj.additionalOperations) {                          // GCOV_EXCL_BR_LINE
    jw.key("additionalOperations");                        // GCOV_EXCL_BR_LINE
    jw.start_object();                                     // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.additionalOperations) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                           // GCOV_EXCL_BR_LINE
      emit_Operation(jw, v);                               // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_Components(utils::JsonWriter &jw, const Components &obj) noexcept {
  jw.start_object();                                      // GCOV_EXCL_BR_LINE
  if (obj.schemas.has_value() && !obj.schemas->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("schemas");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.schemas.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_Schema(jw, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.responses.has_value() &&
      !obj.responses->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("responses");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                 // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.responses.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                       // GCOV_EXCL_BR_LINE
      emit_Response(jw, v);                            // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.parameters.has_value() &&
      !obj.parameters->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("parameters");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                  // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.parameters.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                        // GCOV_EXCL_BR_LINE
      emit_Parameter(jw, v);                            // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.examples.has_value() && !obj.examples->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("examples");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.examples.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_Example(jw, v);                                  // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.requestBodies.has_value() &&
      !obj.requestBodies->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("requestBodies");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                     // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.requestBodies.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                           // GCOV_EXCL_BR_LINE
      emit_RequestBody(jw, v);                             // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.headers.has_value() && !obj.headers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("headers");                                    // GCOV_EXCL_BR_LINE
    jw.start_object();                                    // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.headers.value()) {      // GCOV_EXCL_BR_LINE
      jw.key(k);                                          // GCOV_EXCL_BR_LINE
      emit_Header(jw, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.securitySchemes.has_value() &&
      !obj.securitySchemes->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("securitySchemes");         // GCOV_EXCL_BR_LINE
    jw.start_object();                 // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] :
         obj.securitySchemes.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                        // GCOV_EXCL_BR_LINE
      emit_SecurityScheme(jw, v);       // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.links.has_value() && !obj.links->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("links");                                  // GCOV_EXCL_BR_LINE
    jw.start_object();                                // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.links.value()) {    // GCOV_EXCL_BR_LINE
      jw.key(k);                                      // GCOV_EXCL_BR_LINE
      emit_Link(jw, v);                               // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.callbacks != nullptr &&
      !obj.callbacks->empty()) {                // GCOV_EXCL_BR_LINE
    jw.key("callbacks");                        // GCOV_EXCL_BR_LINE
    jw.start_object();                          // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.callbacks) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                // GCOV_EXCL_BR_LINE
      jw.start_object();                        // GCOV_EXCL_BR_LINE
      for (const auto &[ik, iv] : v) {          // GCOV_EXCL_BR_LINE
        jw.key(ik);                             // GCOV_EXCL_BR_LINE
        emit_PathItem(jw, iv);                  // GCOV_EXCL_BR_LINE
      }
      jw.end_object(); // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.pathItems.has_value() &&
      !obj.pathItems->empty()) {                       // GCOV_EXCL_BR_LINE
    jw.key("pathItems");                               // GCOV_EXCL_BR_LINE
    jw.start_object();                                 // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.pathItems.value()) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                       // GCOV_EXCL_BR_LINE
      emit_PathItem(jw, v);                            // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.mediaTypes != nullptr &&
      !obj.mediaTypes->empty()) {                // GCOV_EXCL_BR_LINE
    jw.key("mediaTypes");                        // GCOV_EXCL_BR_LINE
    jw.start_object();                           // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : *obj.mediaTypes) { // GCOV_EXCL_BR_LINE
      jw.key(k);                                 // GCOV_EXCL_BR_LINE
      emit_MediaType(jw, v);                     // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void emit_OpenAPI(utils::JsonWriter &jw, const OpenAPI &obj) noexcept {
  jw.start_object();                       // GCOV_EXCL_BR_LINE
  jw.key_value("openapi", obj.openapi);    // GCOV_EXCL_BR_LINE
  jw.key_optional("$self", obj.self_link); // GCOV_EXCL_BR_LINE
  jw.key("info");                          // GCOV_EXCL_BR_LINE
  emit_Info(jw, obj.info);                 // GCOV_EXCL_BR_LINE
  jw.key_optional("jsonSchemaDialect",
                  obj.jsonSchemaDialect);                 // GCOV_EXCL_BR_LINE
  if (obj.servers.has_value() && !obj.servers->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("servers");                                    // GCOV_EXCL_BR_LINE
    jw.start_array();                                     // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.servers.value())          // GCOV_EXCL_BR_LINE
      emit_Server(jw, item);                              // GCOV_EXCL_BR_LINE
    jw.end_array();                                       // GCOV_EXCL_BR_LINE
  }
  if (obj.paths.has_value() && !obj.paths->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("paths");                                  // GCOV_EXCL_BR_LINE
    jw.start_object();                                // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.paths.value()) {    // GCOV_EXCL_BR_LINE
      jw.key(k);                                      // GCOV_EXCL_BR_LINE
      emit_PathItem(jw, v);                           // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.webhooks.has_value() && !obj.webhooks->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("webhooks");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.webhooks.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_PathItem(jw, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.webhooks.has_value() && !obj.webhooks->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("webhooks");                                     // GCOV_EXCL_BR_LINE
    jw.start_object();                                      // GCOV_EXCL_BR_LINE
    for (const auto &[k, v] : obj.webhooks.value()) {       // GCOV_EXCL_BR_LINE
      jw.key(k);                                            // GCOV_EXCL_BR_LINE
      emit_PathItem(jw, v);                                 // GCOV_EXCL_BR_LINE
    }
    jw.end_object(); // GCOV_EXCL_BR_LINE
  }
  if (obj.components.has_value()) {              // GCOV_EXCL_BR_LINE
    jw.key("components");                        // GCOV_EXCL_BR_LINE
    emit_Components(jw, obj.components.value()); // GCOV_EXCL_BR_LINE
  }
  if (obj.security.has_value() && !obj.security->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("security");                                     // GCOV_EXCL_BR_LINE
    jw.start_array();                                       // GCOV_EXCL_BR_LINE
    for (const auto &req : obj.security.value()) {          // GCOV_EXCL_BR_LINE
      jw.start_object();                                    // GCOV_EXCL_BR_LINE
      for (const auto &[k, scopes] : req) {                 // GCOV_EXCL_BR_LINE
        jw.key(k);                                          // GCOV_EXCL_BR_LINE
        jw.start_array();                                   // GCOV_EXCL_BR_LINE
        for (const auto &s : scopes)                        // GCOV_EXCL_BR_LINE
          jw.value(s);                                      // GCOV_EXCL_BR_LINE
        jw.end_array();                                     // GCOV_EXCL_BR_LINE
      }
      jw.end_object(); // GCOV_EXCL_BR_LINE
    }
    jw.end_array(); // GCOV_EXCL_BR_LINE
  }
  if (obj.tags.has_value() && !obj.tags->empty()) { // GCOV_EXCL_BR_LINE
    jw.key("tags");                                 // GCOV_EXCL_BR_LINE
    jw.start_array();                               // GCOV_EXCL_BR_LINE
    for (const auto &item : obj.tags.value())       // GCOV_EXCL_BR_LINE
      emit_Tag(jw, item);                           // GCOV_EXCL_BR_LINE
    jw.end_array();                                 // GCOV_EXCL_BR_LINE
  }
  if (obj.externalDocs.has_value()) { // GCOV_EXCL_BR_LINE
    jw.key("externalDocs");           // GCOV_EXCL_BR_LINE
    emit_ExternalDocumentation(jw,
                               obj.externalDocs.value()); // GCOV_EXCL_BR_LINE
  }
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

std::string emit(const OpenAPI &openapi) noexcept {
  utils::JsonWriter jw;      // GCOV_EXCL_BR_LINE
  emit_OpenAPI(jw, openapi); // GCOV_EXCL_BR_LINE
  return jw.str();
}
} // namespace cdd_cpp::openapi
