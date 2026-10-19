/// @file parse.cpp
/// @brief Implementation of C++ AST to OpenAPI Schema parser.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../openapi/models.hpp"
#include "../utils/cpp_ast.hpp"
#include "../utils/json_writer.hpp"
#include <string>

namespace cdd_cpp::utils {

/// @brief Parse C++ AST struct definitions via `parse_schemas`.
void parse_schemas(const CppAST &ast, openapi::OpenAPI &spec) noexcept {
  for (const auto &cls : ast.classes) {
    if (cls.name == "Client" || cls.name == "Router")
      continue; // skip main classes

    if (!spec.components.has_value())
      spec.components = openapi::Components{};
    if (!spec.components->schemas.has_value())
      spec.components->schemas = std::map<std::string, openapi::Schema>{};

    openapi::Schema schema;
    schema.type = "object";
    schema.properties =
        std::make_shared<std::map<std::string, openapi::Schema>>();
    std::vector<std::string> required;

    docstrings::parse_schema_docstrings(cls.docstring, schema);

    for (const auto &field : cls.fields) {
      openapi::Schema prop;
      std::string ftype = field.type;
      bool is_optional = false;
      if (ftype.starts_with("std::optional<")) {
        is_optional = true;
        ftype = ftype.substr(14, ftype.length() - 15);
      }

      std::string prop_name = unsanitize_cpp_identifier(field.name);

      if (!is_optional)
        required.push_back(prop_name);

      if (ftype == "int" || ftype == "int32_t" || ftype == "short") {
        prop.type = "integer";
        if (ftype != "int")
          prop.format = "int32";
      } else if (ftype == "long" || ftype == "int64_t") {
        prop.type = "integer";
        prop.format = "int64";
      } else if (ftype == "float") {
        prop.type = "number";
        prop.format = "float";
      } else if (ftype == "double") {
        prop.type = "number";
      } else if (ftype == "bool" || ftype == "boolean") {
        prop.type = "boolean";
      } else if (ftype == "std::string" || ftype == "string") {
        prop.type = "string";
      } else if (ftype.starts_with("std::vector<") ||
                 ftype.starts_with("vector<")) {
        prop.type = "array";
        size_t prefix_len = ftype.starts_with("std::vector<") ? 12 : 7;
        std::string item_type =
            ftype.substr(prefix_len, ftype.length() - prefix_len - 1);
        openapi::Schema item_schema;
        if (item_type == "int") {
          item_schema.type = "integer";
        } else if (item_type == "double") {
          item_schema.type = "number";
        } else if (item_type == "bool") {
          item_schema.type = "boolean";
        } else if (item_type == "std::string") {
          item_schema.type = "string";
        } else {
          item_schema.ref = openapi::Reference{
              "#/components/schemas/" + item_type, std::nullopt, std::nullopt};
        }
        prop.items = std::make_shared<openapi::Schema>(item_schema);
      } else {
        prop.ref = openapi::Reference{"#/components/schemas/" + ftype,
                                      std::nullopt, std::nullopt};
      }

      if (auto v = docstrings::extract_tag(field.docstring, "description")) {
        prop.description = *v;
      } else if (!field.docstring.empty()) {
        prop.description = field.docstring;
      }

      schema.properties->insert({prop_name, prop});
    }

    if (!required.empty()) {
      schema.required = required;
    }

    spec.components->schemas->insert({cls.name, schema});
  }
}

} // namespace cdd_cpp::utils
