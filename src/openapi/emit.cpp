#include "emit.hpp"
#include "../utils/json_writer.hpp"

namespace cdd_cpp::openapi {

void emit_Reference(utils::JsonWriter &jw, const Reference &obj);
void emit_Contact(utils::JsonWriter &jw, const Contact &obj);
void emit_License(utils::JsonWriter &jw, const License &obj);
void emit_Info(utils::JsonWriter &jw, const Info &obj);
void emit_ServerVariable(utils::JsonWriter &jw, const ServerVariable &obj);
void emit_Server(utils::JsonWriter &jw, const Server &obj);
void emit_ExternalDocumentation(utils::JsonWriter &jw,
                                const ExternalDocumentation &obj);
void emit_Tag(utils::JsonWriter &jw, const Tag &obj);
void emit_Discriminator(utils::JsonWriter &jw, const Discriminator &obj);
void emit_XML(utils::JsonWriter &jw, const XML &obj);
void emit_Schema(utils::JsonWriter &jw, const Schema &obj);
void emit_Example(utils::JsonWriter &jw, const Example &obj);
void emit_Encoding(utils::JsonWriter &jw, const Encoding &obj);
void emit_MediaType(utils::JsonWriter &jw, const MediaType &obj);
void emit_Header(utils::JsonWriter &jw, const Header &obj);
void emit_RequestBody(utils::JsonWriter &jw, const RequestBody &obj);
void emit_Link(utils::JsonWriter &jw, const Link &obj);
void emit_Response(utils::JsonWriter &jw, const Response &obj);
void emit_Parameter(utils::JsonWriter &jw, const Parameter &obj);
void emit_OAuthFlow(utils::JsonWriter &jw, const OAuthFlow &obj);
void emit_OAuthFlows(utils::JsonWriter &jw, const OAuthFlows &obj);
void emit_SecurityScheme(utils::JsonWriter &jw, const SecurityScheme &obj);
void emit_Operation(utils::JsonWriter &jw, const Operation &obj);
void emit_PathItem(utils::JsonWriter &jw, const PathItem &obj);
void emit_Components(utils::JsonWriter &jw, const Components &obj);
void emit_OpenAPI(utils::JsonWriter &jw, const OpenAPI &obj);

void emit_Reference(utils::JsonWriter &jw, const Reference &obj) {
  jw.start_object();
  jw.key_value("$ref", obj.ref);
  jw.key_optional("summary", obj.summary);
  jw.key_optional("description", obj.description);
  jw.end_object();
}

void emit_Contact(utils::JsonWriter &jw, const Contact &obj) {
  jw.start_object();
  jw.key_optional("name", obj.name);
  jw.key_optional("url", obj.url);
  jw.key_optional("email", obj.email);
  jw.end_object();
}

void emit_License(utils::JsonWriter &jw, const License &obj) {
  jw.start_object();
  jw.key_value("name", obj.name);
  jw.key_optional("identifier", obj.identifier);
  jw.key_optional("url", obj.url);
  jw.end_object();
}

void emit_Info(utils::JsonWriter &jw, const Info &obj) {
  jw.start_object();
  jw.key_value("title", obj.title);
  jw.key_optional("summary", obj.summary);
  jw.key_optional("description", obj.description);
  jw.key_optional("termsOfService", obj.termsOfService);
  if (obj.contact.has_value()) {
    jw.key("contact");
    emit_Contact(jw, obj.contact.value());
  }
  if (obj.license.has_value()) {
    jw.key("license");
    emit_License(jw, obj.license.value());
  }
  jw.key_value("version", obj.version);
  jw.end_object();
}

void emit_ServerVariable(utils::JsonWriter &jw, const ServerVariable &obj) {
  jw.start_object();
  if (obj.enum_values.has_value() && !obj.enum_values->empty()) {
    jw.key("enum");
    jw.start_array();
    for (const auto &item : obj.enum_values.value())
      jw.value(item);
    jw.end_array();
  }
  jw.key_value("default", obj.default_value);
  jw.key_optional("description", obj.description);
  jw.end_object();
}

void emit_Server(utils::JsonWriter &jw, const Server &obj) {
  jw.start_object();
  jw.key_value("url", obj.url);
  jw.key_optional("description", obj.description);
  jw.key_optional("name", obj.name);
  if (obj.variables.has_value() && !obj.variables->empty()) {
    jw.key("variables");
    jw.start_object();
    for (const auto &[k, v] : obj.variables.value()) {
      jw.key(k);
      emit_ServerVariable(jw, v);
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_ExternalDocumentation(utils::JsonWriter &jw,
                                const ExternalDocumentation &obj) {
  jw.start_object();
  jw.key_optional("description", obj.description);
  jw.key_value("url", obj.url);
  jw.end_object();
}

void emit_Tag(utils::JsonWriter &jw, const Tag &obj) {
  jw.start_object();
  jw.key_value("name", obj.name);
  jw.key_optional("description", obj.description);
  jw.key_optional("parent", obj.parent);
  jw.key_optional("kind", obj.kind);
  if (obj.externalDocs.has_value()) {
    jw.key("externalDocs");
    emit_ExternalDocumentation(jw, obj.externalDocs.value());
  }
  jw.end_object();
}
void emit_Discriminator(utils::JsonWriter &jw, const Discriminator &obj) {
  jw.start_object();
  jw.key_value("propertyName", obj.propertyName);
  if (obj.mapping.has_value() && !obj.mapping->empty()) {
    jw.key("mapping");
    jw.start_object();
    for (const auto &[k, v] : obj.mapping.value()) {
      jw.key_value(k, v);
    }
    jw.end_object();
  }
  jw.key_optional("defaultMapping", obj.defaultMapping);
  jw.end_object();
}

void emit_XML(utils::JsonWriter &jw, const XML &obj) {
  jw.start_object();
  jw.key_optional("name", obj.name);
  jw.key_optional("namespace", obj.namespace_url);
  jw.key_optional("prefix", obj.prefix);
  if (obj.attribute.has_value())
    jw.key_value("attribute", obj.attribute.value());
  if (obj.wrapped.has_value())
    jw.key_value("wrapped", obj.wrapped.value());
  jw.end_object();
}

void emit_Schema(utils::JsonWriter &jw, const Schema &obj) {
  jw.start_object();
  jw.key_optional("type", obj.type);
  jw.key_optional("description", obj.description);
  jw.key_optional("format", obj.format);
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  if (obj.discriminator.has_value()) {
    jw.key("discriminator");
    emit_Discriminator(jw, obj.discriminator.value());
  }
  if (obj.xml.has_value()) {
    jw.key("xml");
    emit_XML(jw, obj.xml.value());
  }
  if (obj.properties != nullptr && !obj.properties->empty()) {
    jw.key("properties");
    jw.start_object();
    for (const auto &[k, v] : *obj.properties) {
      jw.key(k);
      emit_Schema(jw, v);
    }
    jw.end_object();
  }
  if (obj.items != nullptr) {
    jw.key("items");
    emit_Schema(jw, *obj.items);
  }
  jw.end_object();
}

void emit_Example(utils::JsonWriter &jw, const Example &obj) {
  jw.start_object();
  jw.key_optional("summary", obj.summary);
  jw.key_optional("description", obj.description);
  jw.key_optional("value", obj.value);
  jw.key_optional("externalValue", obj.externalValue);
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_Encoding(utils::JsonWriter &jw, const Encoding &obj) {
  jw.start_object();
  jw.key_optional("contentType", obj.contentType);
  jw.key_optional("style", obj.style);
  if (obj.explode.has_value())
    jw.key_value("explode", obj.explode.value());
  if (obj.allowReserved.has_value())
    jw.key_value("allowReserved", obj.allowReserved.value());
  if (obj.headers != nullptr && !obj.headers->empty()) {
    jw.key("headers");
    jw.start_object();
    for (const auto &[k, v] : *obj.headers) {
      jw.key(k);
      emit_Header(jw, v);
    }
    jw.end_object();
  }
  if (obj.encoding != nullptr && !obj.encoding->empty()) {
    jw.key("encoding");
    jw.start_object();
    for (const auto &[k, v] : *obj.encoding) {
      jw.key(k);
      emit_Encoding(jw, v);
    }
    jw.end_object();
  }
  if (obj.itemSchema != nullptr) {
    jw.key("itemSchema");
    emit_Schema(jw, *obj.itemSchema);
  }
  if (obj.prefixEncoding != nullptr && !obj.prefixEncoding->empty()) {
    jw.key("prefixEncoding");
    jw.start_array();
    for (const auto &e : *obj.prefixEncoding) {
      emit_Encoding(jw, e);
    }
    jw.end_array();
  }
  if (obj.itemEncoding != nullptr) {
    jw.key("itemEncoding");
    emit_Encoding(jw, *obj.itemEncoding);
  }
  if (obj.prefixEncoding != nullptr && !obj.prefixEncoding->empty()) {
    jw.key("prefixEncoding");
    jw.start_array();
    for (const auto &e : *obj.prefixEncoding) {
      emit_Encoding(jw, e);
    }
    jw.end_array();
  }
  if (obj.itemEncoding != nullptr) {
    jw.key("itemEncoding");
    emit_Encoding(jw, *obj.itemEncoding);
  }
  jw.end_object();
}

void emit_MediaType(utils::JsonWriter &jw, const MediaType &obj) {
  jw.start_object();
  if (obj.schema.has_value()) {
    jw.key("schema");
    emit_Schema(jw, obj.schema.value());
  }
  jw.key_optional("example", obj.example);
  if (obj.examples.has_value() && !obj.examples->empty()) {
    jw.key("examples");
    jw.start_object();
    for (const auto &[k, v] : obj.examples.value()) {
      jw.key(k);
      emit_Example(jw, v);
    }
    jw.end_object();
  }
  if (obj.encoding != nullptr && !obj.encoding->empty()) {
    jw.key("encoding");
    jw.start_object();
    for (const auto &[k, v] : *obj.encoding) {
      jw.key(k);
      emit_Encoding(jw, v);
    }
    jw.end_object();
  }
  if (obj.itemSchema != nullptr) {
    jw.key("itemSchema");
    emit_Schema(jw, *obj.itemSchema);
  }
  if (obj.prefixEncoding != nullptr && !obj.prefixEncoding->empty()) {
    jw.key("prefixEncoding");
    jw.start_array();
    for (const auto &e : *obj.prefixEncoding) {
      emit_Encoding(jw, e);
    }
    jw.end_array();
  }
  if (obj.itemEncoding != nullptr) {
    jw.key("itemEncoding");
    emit_Encoding(jw, *obj.itemEncoding);
  }
  jw.end_object();
}

void emit_Header(utils::JsonWriter &jw, const Header &obj) {
  jw.start_object();
  jw.key_optional("description", obj.description);
  if (obj.required)
    jw.key_value("required", true);
  if (obj.deprecated)
    jw.key_value("deprecated", true);
  if (obj.allowEmptyValue)
    jw.key_value("allowEmptyValue", true);
  jw.key_optional("style", obj.style);
  if (obj.explode.has_value())
    jw.key_value("explode", obj.explode.value());
  if (obj.allowReserved.has_value())
    jw.key_value("allowReserved", obj.allowReserved.value());
  if (obj.schema.has_value()) {
    jw.key("schema");
    emit_Schema(jw, obj.schema.value());
  }
  jw.key_optional("example", obj.example);
  if (obj.examples.has_value() && !obj.examples->empty()) {
    jw.key("examples");
    jw.start_object();
    for (const auto &[k, v] : obj.examples.value()) {
      jw.key(k);
      emit_Example(jw, v);
    }
    jw.end_object();
  }
  if (obj.content.has_value() && !obj.content->empty()) {
    jw.key("content");
    jw.start_object();
    for (const auto &[k, v] : obj.content.value()) {
      jw.key(k);
      emit_MediaType(jw, v);
    }
    jw.end_object();
  }
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_RequestBody(utils::JsonWriter &jw, const RequestBody &obj) {
  jw.start_object();
  jw.key_optional("description", obj.description);
  if (!obj.content.empty()) {
    jw.key("content");
    jw.start_object();
    for (const auto &[k, v] : obj.content) {
      jw.key(k);
      emit_MediaType(jw, v);
    }
    jw.end_object();
  }
  if (obj.required)
    jw.key_value("required", true);
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_Link(utils::JsonWriter &jw, const Link &obj) {
  jw.start_object();
  jw.key_optional("operationRef", obj.operationRef);
  jw.key_optional("operationId", obj.operationId);
  if (obj.parameters.has_value() && !obj.parameters->empty()) {
    jw.key("parameters");
    jw.start_object();
    for (const auto &[k, v] : obj.parameters.value())
      jw.key_value(k, v);
    jw.end_object();
  }
  jw.key_optional("requestBody", obj.requestBody);
  jw.key_optional("description", obj.description);
  if (obj.server.has_value()) {
    jw.key("server");
    emit_Server(jw, obj.server.value());
  }
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_Response(utils::JsonWriter &jw, const Response &obj) {
  jw.start_object();
  jw.key_value("description", obj.description);
  if (obj.headers.has_value() && !obj.headers->empty()) {
    jw.key("headers");
    jw.start_object();
    for (const auto &[k, v] : obj.headers.value()) {
      jw.key(k);
      emit_Header(jw, v);
    }
    jw.end_object();
  }
  if (obj.content.has_value() && !obj.content->empty()) {
    jw.key("content");
    jw.start_object();
    for (const auto &[k, v] : obj.content.value()) {
      jw.key(k);
      emit_MediaType(jw, v);
    }
    jw.end_object();
  }
  if (obj.links.has_value() && !obj.links->empty()) {
    jw.key("links");
    jw.start_object();
    for (const auto &[k, v] : obj.links.value()) {
      jw.key(k);
      emit_Link(jw, v);
    }
    jw.end_object();
  }
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_Parameter(utils::JsonWriter &jw, const Parameter &obj) {
  jw.start_object();
  jw.key_value("name", obj.name);
  jw.key_value("in", obj.in);
  jw.key_optional("description", obj.description);
  if (obj.required)
    jw.key_value("required", true);
  if (obj.deprecated)
    jw.key_value("deprecated", true);
  if (obj.allowEmptyValue)
    jw.key_value("allowEmptyValue", true);
  jw.key_optional("style", obj.style);
  if (obj.explode.has_value())
    jw.key_value("explode", obj.explode.value());
  if (obj.allowReserved.has_value())
    jw.key_value("allowReserved", obj.allowReserved.value());
  if (obj.schema.has_value()) {
    jw.key("schema");
    emit_Schema(jw, obj.schema.value());
  }
  jw.key_optional("example", obj.example);
  if (obj.examples.has_value() && !obj.examples->empty()) {
    jw.key("examples");
    jw.start_object();
    for (const auto &[k, v] : obj.examples.value()) {
      jw.key(k);
      emit_Example(jw, v);
    }
    jw.end_object();
  }
  if (obj.content.has_value() && !obj.content->empty()) {
    jw.key("content");
    jw.start_object();
    for (const auto &[k, v] : obj.content.value()) {
      jw.key(k);
      emit_MediaType(jw, v);
    }
    jw.end_object();
  }
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_OAuthFlow(utils::JsonWriter &jw, const OAuthFlow &obj) {
  jw.start_object();
  jw.key_optional("authorizationUrl", obj.authorizationUrl);
  jw.key_optional("tokenUrl", obj.tokenUrl);
  jw.key_optional("refreshUrl", obj.refreshUrl);
  if (obj.scopes.has_value() && !obj.scopes->empty()) {
    jw.key("scopes");
    jw.start_object();
    for (const auto &[k, v] : obj.scopes.value())
      jw.key_value(k, v);
    jw.end_object();
  }
  jw.end_object();
}

void emit_OAuthFlows(utils::JsonWriter &jw, const OAuthFlows &obj) {
  jw.start_object();
  if (obj.implicit.has_value()) {
    jw.key("implicit");
    emit_OAuthFlow(jw, obj.implicit.value());
  }
  if (obj.password.has_value()) {
    jw.key("password");
    emit_OAuthFlow(jw, obj.password.value());
  }
  if (obj.clientCredentials.has_value()) {
    jw.key("clientCredentials");
    emit_OAuthFlow(jw, obj.clientCredentials.value());
  }
  if (obj.authorizationCode.has_value()) {
    jw.key("authorizationCode");
    emit_OAuthFlow(jw, obj.authorizationCode.value());
  }
  jw.end_object();
}

void emit_SecurityScheme(utils::JsonWriter &jw, const SecurityScheme &obj) {
  jw.start_object();
  jw.key_value("type", obj.type);
  jw.key_optional("description", obj.description);
  jw.key_optional("name", obj.name);
  jw.key_optional("in", obj.in);
  jw.key_optional("scheme", obj.scheme);
  jw.key_optional("bearerFormat", obj.bearerFormat);
  if (obj.flows.has_value()) {
    jw.key("flows");
    emit_OAuthFlows(jw, obj.flows.value());
  }
  jw.key_optional("openIdConnectUrl", obj.openIdConnectUrl);
  if (obj.ref.has_value()) {
    jw.key_value("$ref", obj.ref.value().ref);
    jw.key_optional("summary", obj.ref.value().summary);
    jw.key_optional("description", obj.ref.value().description);
  }
  jw.end_object();
}

void emit_Operation(utils::JsonWriter &jw, const Operation &obj) {
  jw.start_object();
  if (obj.tags.has_value() && !obj.tags->empty()) {
    jw.key("tags");
    jw.start_array();
    for (const auto &item : obj.tags.value())
      jw.value(item);
    jw.end_array();
  }
  jw.key_optional("summary", obj.summary);
  jw.key_optional("description", obj.description);
  if (obj.externalDocs.has_value()) {
    jw.key("externalDocs");
    emit_ExternalDocumentation(jw, obj.externalDocs.value());
  }
  jw.key_optional("operationId", obj.operationId);
  if (obj.parameters.has_value() && !obj.parameters->empty()) {
    jw.key("parameters");
    jw.start_array();
    for (const auto &item : obj.parameters.value())
      emit_Parameter(jw, item);
    jw.end_array();
  }
  if (obj.requestBody.has_value()) {
    jw.key("requestBody");
    emit_RequestBody(jw, obj.requestBody.value());
  }
  if (obj.responses.has_value() && !obj.responses->empty()) {
    jw.key("responses");
    jw.start_object();
    for (const auto &[k, v] : obj.responses.value()) {
      jw.key(k);
      emit_Response(jw, v);
    }
    jw.end_object();
  }
  if (obj.callbacks != nullptr && !obj.callbacks->empty()) {
    jw.key("callbacks");
    jw.start_object();
    for (const auto &[k, v] : *obj.callbacks) {
      jw.key(k);
      jw.start_object();
      for (const auto &[ik, iv] : v) {
        jw.key(ik);
        emit_PathItem(jw, iv);
      }
      jw.end_object();
    }
    jw.end_object();
  }
  if (obj.deprecated)
    jw.key_value("deprecated", true);
  if (obj.security.has_value() && !obj.security->empty()) {
    jw.key("security");
    jw.start_array();
    for (const auto &req : obj.security.value()) {
      jw.start_object();
      for (const auto &[k, scopes] : req) {
        jw.key(k);
        jw.start_array();
        for (const auto &s : scopes)
          jw.value(s);
        jw.end_array();
      }
      jw.end_object();
    }
    jw.end_array();
  }
  if (obj.servers.has_value() && !obj.servers->empty()) {
    jw.key("servers");
    jw.start_array();
    for (const auto &item : obj.servers.value())
      emit_Server(jw, item);
    jw.end_array();
  }
  jw.end_object();
}

void emit_PathItem(utils::JsonWriter &jw, const PathItem &obj) {
  jw.start_object();
  jw.key_optional("$ref", obj.ref);
  jw.key_optional("summary", obj.summary);
  jw.key_optional("description", obj.description);
  if (obj.get.has_value()) {
    jw.key("get");
    emit_Operation(jw, obj.get.value());
  }
  if (obj.put.has_value()) {
    jw.key("put");
    emit_Operation(jw, obj.put.value());
  }
  if (obj.post.has_value()) {
    jw.key("post");
    emit_Operation(jw, obj.post.value());
  }
  if (obj.delete_op.has_value()) {
    jw.key("delete");
    emit_Operation(jw, obj.delete_op.value());
  }
  if (obj.options.has_value()) {
    jw.key("options");
    emit_Operation(jw, obj.options.value());
  }
  if (obj.head.has_value()) {
    jw.key("head");
    emit_Operation(jw, obj.head.value());
  }
  if (obj.patch.has_value()) {
    jw.key("patch");
    emit_Operation(jw, obj.patch.value());
  }
  if (obj.trace.has_value()) {
    jw.key("trace");
    emit_Operation(jw, obj.trace.value());
  }
  if (obj.query.has_value()) {
    jw.key("query");
    emit_Operation(jw, obj.query.value());
  }
  if (obj.servers.has_value() && !obj.servers->empty()) {
    jw.key("servers");
    jw.start_array();
    for (const auto &item : obj.servers.value())
      emit_Server(jw, item);
    jw.end_array();
  }
  if (obj.parameters.has_value() && !obj.parameters->empty()) {
    jw.key("parameters");
    jw.start_array();
    for (const auto &item : obj.parameters.value())
      emit_Parameter(jw, item);
    jw.end_array();
  }
  if (obj.additionalOperations) {
    jw.key("additionalOperations");
    jw.start_object();
    for (const auto &[k, v] : *obj.additionalOperations) {
      jw.key(k);
      emit_Operation(jw, v);
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_Components(utils::JsonWriter &jw, const Components &obj) {
  jw.start_object();
  if (obj.schemas.has_value() && !obj.schemas->empty()) {
    jw.key("schemas");
    jw.start_object();
    for (const auto &[k, v] : obj.schemas.value()) {
      jw.key(k);
      emit_Schema(jw, v);
    }
    jw.end_object();
  }
  if (obj.responses.has_value() && !obj.responses->empty()) {
    jw.key("responses");
    jw.start_object();
    for (const auto &[k, v] : obj.responses.value()) {
      jw.key(k);
      emit_Response(jw, v);
    }
    jw.end_object();
  }
  if (obj.parameters.has_value() && !obj.parameters->empty()) {
    jw.key("parameters");
    jw.start_object();
    for (const auto &[k, v] : obj.parameters.value()) {
      jw.key(k);
      emit_Parameter(jw, v);
    }
    jw.end_object();
  }
  if (obj.examples.has_value() && !obj.examples->empty()) {
    jw.key("examples");
    jw.start_object();
    for (const auto &[k, v] : obj.examples.value()) {
      jw.key(k);
      emit_Example(jw, v);
    }
    jw.end_object();
  }
  if (obj.requestBodies.has_value() && !obj.requestBodies->empty()) {
    jw.key("requestBodies");
    jw.start_object();
    for (const auto &[k, v] : obj.requestBodies.value()) {
      jw.key(k);
      emit_RequestBody(jw, v);
    }
    jw.end_object();
  }
  if (obj.headers.has_value() && !obj.headers->empty()) {
    jw.key("headers");
    jw.start_object();
    for (const auto &[k, v] : obj.headers.value()) {
      jw.key(k);
      emit_Header(jw, v);
    }
    jw.end_object();
  }
  if (obj.securitySchemes.has_value() && !obj.securitySchemes->empty()) {
    jw.key("securitySchemes");
    jw.start_object();
    for (const auto &[k, v] : obj.securitySchemes.value()) {
      jw.key(k);
      emit_SecurityScheme(jw, v);
    }
    jw.end_object();
  }
  if (obj.links.has_value() && !obj.links->empty()) {
    jw.key("links");
    jw.start_object();
    for (const auto &[k, v] : obj.links.value()) {
      jw.key(k);
      emit_Link(jw, v);
    }
    jw.end_object();
  }
  if (obj.callbacks != nullptr && !obj.callbacks->empty()) {
    jw.key("callbacks");
    jw.start_object();
    for (const auto &[k, v] : *obj.callbacks) {
      jw.key(k);
      jw.start_object();
      for (const auto &[ik, iv] : v) {
        jw.key(ik);
        emit_PathItem(jw, iv);
      }
      jw.end_object();
    }
    jw.end_object();
  }
  if (obj.pathItems.has_value() && !obj.pathItems->empty()) {
    jw.key("pathItems");
    jw.start_object();
    for (const auto &[k, v] : obj.pathItems.value()) {
      jw.key(k);
      emit_PathItem(jw, v);
    }
    jw.end_object();
  }
  if (obj.mediaTypes != nullptr && !obj.mediaTypes->empty()) {
    jw.key("mediaTypes");
    jw.start_object();
    for (const auto &[k, v] : *obj.mediaTypes) {
      jw.key(k);
      emit_MediaType(jw, v);
    }
    jw.end_object();
  }
  jw.end_object();
}

void emit_OpenAPI(utils::JsonWriter &jw, const OpenAPI &obj) {
  jw.start_object();
  jw.key_value("openapi", obj.openapi);
  jw.key_optional("$self", obj.self_link);
  jw.key("info");
  emit_Info(jw, obj.info);
  jw.key_optional("jsonSchemaDialect", obj.jsonSchemaDialect);
  if (obj.servers.has_value() && !obj.servers->empty()) {
    jw.key("servers");
    jw.start_array();
    for (const auto &item : obj.servers.value())
      emit_Server(jw, item);
    jw.end_array();
  }
  if (obj.paths.has_value() && !obj.paths->empty()) {
    jw.key("paths");
    jw.start_object();
    for (const auto &[k, v] : obj.paths.value()) {
      jw.key(k);
      emit_PathItem(jw, v);
    }
    jw.end_object();
  }
  if (obj.webhooks.has_value() && !obj.webhooks->empty()) {
    jw.key("webhooks");
    jw.start_object();
    for (const auto &[k, v] : obj.webhooks.value()) {
      jw.key(k);
      emit_PathItem(jw, v);
    }
    jw.end_object();
  }
  if (obj.webhooks.has_value() && !obj.webhooks->empty()) {
    jw.key("webhooks");
    jw.start_object();
    for (const auto &[k, v] : obj.webhooks.value()) {
      jw.key(k);
      emit_PathItem(jw, v);
    }
    jw.end_object();
  }
  if (obj.components.has_value()) {
    jw.key("components");
    emit_Components(jw, obj.components.value());
  }
  if (obj.security.has_value() && !obj.security->empty()) {
    jw.key("security");
    jw.start_array();
    for (const auto &req : obj.security.value()) {
      jw.start_object();
      for (const auto &[k, scopes] : req) {
        jw.key(k);
        jw.start_array();
        for (const auto &s : scopes)
          jw.value(s);
        jw.end_array();
      }
      jw.end_object();
    }
    jw.end_array();
  }
  if (obj.tags.has_value() && !obj.tags->empty()) {
    jw.key("tags");
    jw.start_array();
    for (const auto &item : obj.tags.value())
      emit_Tag(jw, item);
    jw.end_array();
  }
  if (obj.externalDocs.has_value()) {
    jw.key("externalDocs");
    emit_ExternalDocumentation(jw, obj.externalDocs.value());
  }
  jw.end_object();
}

std::string emit(const OpenAPI &openapi) noexcept {
  utils::JsonWriter jw;
  emit_OpenAPI(jw, openapi);
  return jw.str();
}
} // namespace cdd_cpp::openapi
