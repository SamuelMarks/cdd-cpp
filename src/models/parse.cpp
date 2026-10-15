/// @file parse.cpp
/// @brief Implementation of C++ AST to OpenAPI Schema parser.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../openapi/models.hpp"
#include "../utils/cpp_ast.hpp"
#include "../utils/json_writer.hpp"
#include <string>

namespace cdd_cpp::utils {

/// @brief Internal helper or implementation for `parse_schemas`.
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

    if (auto v = docstrings::extract_tag(cls.docstring, "description"))
      schema.description = *v;
    if (auto v = docstrings::extract_tag(cls.docstring, "example"))
      schema.example = *v;

    if (auto vd = docstrings::extract_tag(cls.docstring,
                                          "externalDocs.description")) {
      schema.externalDocs = openapi::ExternalDocumentation{};
      schema.externalDocs->description = *vd;
    }
    if (auto vu = docstrings::extract_tag(cls.docstring, "externalDocs.url")) {
      if (!schema.externalDocs.has_value())
        schema.externalDocs = openapi::ExternalDocumentation{};
      schema.externalDocs->url = *vu;
    }

    if (auto vn = docstrings::extract_tag(cls.docstring, "xml.name")) {
      schema.xml = openapi::XML{};
      schema.xml->name = *vn;
    }
    if (auto vns =
            docstrings::extract_tag(cls.docstring, "xml.namespace_url")) {
      if (!schema.xml.has_value())
        schema.xml = openapi::XML{};
      schema.xml->namespace_url = *vns;
    }
    if (auto vp = docstrings::extract_tag(cls.docstring, "xml.prefix")) {
      if (!schema.xml.has_value())
        schema.xml = openapi::XML{};
      schema.xml->prefix = *vp;
    }
    if (auto va = docstrings::extract_tag(cls.docstring, "xml.attribute")) {
      if (!schema.xml.has_value())
        schema.xml = openapi::XML{};
      schema.xml->attribute = (*va == "true");
    }
    if (auto vw = docstrings::extract_tag(cls.docstring, "xml.wrapped")) {
      if (!schema.xml.has_value())
        schema.xml = openapi::XML{};
      schema.xml->wrapped = (*vw == "true");
    }

    if (auto vdp = docstrings::extract_tag(cls.docstring,
                                           "discriminator.propertyName")) {
      schema.discriminator = openapi::Discriminator{};
      schema.discriminator->propertyName = *vdp;
    }

    size_t dm_pos = 0;
    for (;;) {
      size_t idx = cls.docstring.find("@discriminator.mapping", dm_pos);
      if (idx == std::string::npos)
        break;
      size_t after = idx + 22;
      size_t s = cls.docstring.find_first_not_of(" \t", after);
      size_t e = cls.docstring.find_first_of(" \t\r\n", s);
      std::string token = cls.docstring.substr(s, e - s);
      size_t colon = token.find(':');
      if (!schema.discriminator.has_value())
        schema.discriminator = openapi::Discriminator{};
      if (!schema.discriminator->mapping.has_value())
        schema.discriminator->mapping = std::map<std::string, std::string>{};
      schema.discriminator->mapping->insert(
          {token.substr(0, colon), token.substr(colon + 1)});
      dm_pos = e;
    }

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

      if (ftype == "int") {
        prop.type = "integer";
      } else if (ftype == "double") {
        prop.type = "number";
      } else if (ftype == "bool") {
        prop.type = "boolean";
      } else if (ftype == "std::string") {
        prop.type = "string";
      } else if (ftype.starts_with("std::vector<")) {
        prop.type = "array";
        std::string item_type = ftype.substr(12, ftype.length() - 13);
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

      if (auto v = docstrings::extract_tag(field.docstring, "description"))
        prop.description = *v;

      schema.properties->insert({prop_name, prop});
    }

    if (!required.empty()) {
      schema.required = required;
    }

    spec.components->schemas->insert({cls.name, schema});
  }
}

} // namespace cdd_cpp::utils
