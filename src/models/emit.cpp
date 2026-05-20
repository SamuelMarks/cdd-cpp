#include "emit.hpp"
#include <functional>
#include <set>
#include <sstream>

namespace cdd_cpp::models {

static std::string format_multi(const std::string &indent,
                                const std::string &text) {
  std::string res;
  for (char c : text) {
    if (c == '\r')
      continue;
    res += c;
    if (c == '\n')
      res += indent;
  }
  return res;
}

std::string map_type(const openapi::Schema &schema) noexcept {
  if (schema.ref.has_value()) {
    std::string ref = schema.ref.value().ref;
    size_t last_slash = ref.find_last_of('/');
    if (last_slash != std::string::npos) {
      return ref.substr(last_slash + 1);
    }
    return ref;
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
    if (schema.items)
      return "std::vector<" + map_type(*schema.items) + ">";
    return "std::vector<std::string>";
  }
  return "std::string";
}

void emit_docstrings(std::stringstream &ss, const openapi::Schema &schema,
                     const std::string &indent) noexcept {
  if (schema.description)
    ss << indent << "/// @description "
       << format_multi(indent + "/// ", *schema.description) << "\n";
  if (schema.example)
    ss << indent << "/// @example "
       << format_multi(indent + "/// ", *schema.example) << "\n";
  if (schema.externalDocs) {
    if (schema.externalDocs->description)
      ss << indent << "/// @externalDocs.description "
         << *schema.externalDocs->description << "\n";
    ss << indent << "/// @externalDocs.url " << schema.externalDocs->url
       << "\n";
  }
  if (schema.xml) {
    if (schema.xml->name)
      ss << indent << "/// @xml.name " << *schema.xml->name << "\n";
    if (schema.xml->namespace_url)
      ss << indent << "/// @xml.namespace_url " << *schema.xml->namespace_url
         << "\n";
    if (schema.xml->prefix)
      ss << indent << "/// @xml.prefix " << *schema.xml->prefix << "\n";
    if (schema.xml->attribute)
      ss << indent << "/// @xml.attribute "
         << (*schema.xml->attribute ? "true" : "false") << "\n";
    if (schema.xml->wrapped)
      ss << indent << "/// @xml.wrapped "
         << (*schema.xml->wrapped ? "true" : "false") << "\n";
  }
  if (schema.discriminator) {
    ss << indent << "/// @discriminator.propertyName "
       << schema.discriminator->propertyName << "\n";
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
  ss << "#define SIMDJSON_STATIC_REFLECTION 1\n";
  ss << "#include <simdjson.h>\n";
  ss << "#include <string>\n#include <vector>\n#include <optional>\n\n";

  ss << "namespace cdd_models {\n\n";

  if (spec.components && spec.components->schemas) {
    std::vector<std::string> sorted_names;
    std::set<std::string> visited;
    std::set<std::string> in_progress;

    std::function<void(const openapi::Schema &, std::vector<std::string> &)>
        extract_deps = [&](const openapi::Schema &schema,
                           std::vector<std::string> &deps) {
          if (schema.ref.has_value()) {
            std::string ref = schema.ref.value().ref;
            size_t last_slash = ref.find_last_of('/');
            if (last_slash != std::string::npos) {
              deps.push_back(ref.substr(last_slash + 1));
            } else {
              deps.push_back(ref);
            }
          }
          if (schema.items) {
            extract_deps(*schema.items, deps);
          }
          if (schema.properties) {
            for (const auto &[_, prop_schema] : *schema.properties) {
              extract_deps(prop_schema, deps);
            }
          }
        };

    auto get_deps = [&](const openapi::Schema &schema) -> std::vector<std::string> {
      std::vector<std::string> deps;
      extract_deps(schema, deps);
      return deps;
    };

    std::function<void(const std::string &)> dfs =
        [&](const std::string &name) {
          if (visited.count(name))
            return;
          if (in_progress.count(name))
            return;
          in_progress.insert(name);

          auto it = spec.components->schemas->find(name);
          if (it != spec.components->schemas->end()) {
            for (const auto &dep : get_deps(it->second)) {
              dfs(dep);
            }
          }

          in_progress.erase(name);
          visited.insert(name);
          sorted_names.push_back(name);
        };


    for (const auto &[name, _] : *spec.components->schemas) {
      dfs(name);
    }

    for (const auto &name : sorted_names) {
      auto it = spec.components->schemas->find(name);
      if (it == spec.components->schemas->end())
        continue;
      const auto &schema = it->second;

      emit_docstrings(ss, schema, "    ");
      ss << "    struct " << name << " {\n";
      if (schema.properties) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          emit_docstrings(ss, prop_schema, "        ");
          bool is_required = false;
          if (schema.required) {
            for (const auto &req : *schema.required) {
              if (req == prop_name)
                is_required = true;
            }
          }
          if (is_required) {
            ss << "        " << map_type(prop_schema) << " " << prop_name
               << ";\n";
          } else {
            ss << "        std::optional<" << map_type(prop_schema) << "> "
               << prop_name << ";\n";
          }
        }
      }
      ss << "    };\n\n";
    }
  }

  ss << "}\n\n";
  return ss.str();
}

} // namespace cdd_cpp::models
