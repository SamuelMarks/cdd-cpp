#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::classes {

std::string map_type(const openapi::Schema &schema) {
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

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <string>\n";
  ss << "#include <vector>\n";
  ss << "#include <optional>\n\n";
  ss << "namespace cdd_models {\n\n";

  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.value()) {
      if (schema.description.has_value()) {
        ss << "    /// @description " << schema.description.value() << "\n";
      }
      ss << "    struct " << name << " {\n";
      if (schema.properties != nullptr) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          std::string cpp_type = map_type(prop_schema);
          ss << "        " << cpp_type << " " << prop_name << ";\n";
        }
      }
      ss << "    };\n\n";
    }
  }

  ss << "}\n";
  return ss.str();
}

} // namespace cdd_cpp::classes
