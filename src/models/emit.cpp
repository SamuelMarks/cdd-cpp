#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::models {

std::string map_type(const openapi::Schema &schema) noexcept {
  if (schema.ref.has_value()) {
    std::string ref = schema.ref.value().ref;
    size_t last_slash = ref.find_last_of('/');
    if (last_slash != std::string::npos) {
      return ref.substr(last_slash + 1);
    }
  }
  if (!schema.type.has_value())
    return "std::string";

  std::string t = schema.type.value();
  if (t == "integer")
    return "int";
  if (t == "number")
    return "double";
  if (t == "boolean")
    return "bool";
  if (t == "string")
    return "std::string";
  if (t == "array") {
    if (schema.items) {
      return "std::vector<" + map_type(*schema.items) + ">";
    }
    return "std::vector<std::string>";
  }
  return "std::string";
}

bool is_required(const openapi::Schema &parent_schema, const std::string &prop_name) noexcept {
  if (parent_schema.required.has_value()) {
    for (const auto &r : parent_schema.required.value()) {
      if (r == prop_name) return true;
    }
  }
  return false;
}

void emit_docstrings(std::stringstream &ss, const openapi::Schema &schema, const std::string &indent) noexcept {
  if (schema.description) ss << indent << "/// @description " << *schema.description << "\n";
  if (schema.example) ss << indent << "/// @example " << *schema.example << "\n";
  if (schema.externalDocs) {
    if (schema.externalDocs->description) ss << indent << "/// @externalDocs.description " << *schema.externalDocs->description << "\n";
    ss << indent << "/// @externalDocs.url " << schema.externalDocs->url << "\n";
  }
  if (schema.xml) {
    if (schema.xml->name) ss << indent << "/// @xml.name " << *schema.xml->name << "\n";
    if (schema.xml->namespace_url) ss << indent << "/// @xml.namespace_url " << *schema.xml->namespace_url << "\n";
    if (schema.xml->prefix) ss << indent << "/// @xml.prefix " << *schema.xml->prefix << "\n";
    if (schema.xml->attribute) ss << indent << "/// @xml.attribute " << (*schema.xml->attribute ? "true" : "false") << "\n";
    if (schema.xml->wrapped) ss << indent << "/// @xml.wrapped " << (*schema.xml->wrapped ? "true" : "false") << "\n";
  }
  if (schema.discriminator) {
    ss << indent << "/// @discriminator.propertyName " << schema.discriminator->propertyName << "\n";
    if (schema.discriminator->mapping) {
      for (const auto &[k, v] : *schema.discriminator->mapping) {
        ss << indent << "/// @discriminator.mapping " << k << ":" << v << "\n";
      }
    }
  }
}

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <string>\n";
  ss << "#include <vector>\n";
  ss << "#include <optional>\n\n";
  ss << "namespace cdd_models {\n\n";

  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.value()) {
      emit_docstrings(ss, schema, "    ");
      ss << "    struct " << name << " {\n";
      if (schema.properties != nullptr) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          emit_docstrings(ss, prop_schema, "        ");
          std::string cpp_type = map_type(prop_schema);
          if (!is_required(schema, prop_name)) {
            cpp_type = "std::optional<" + cpp_type + ">";
          }
          ss << "        " << cpp_type << " " << prop_name << ";\n";
        }
      }
      ss << "    };\n\n";
    }
  }

  ss << "}\n";
  return ss.str();
}

} // namespace cdd_cpp::models
