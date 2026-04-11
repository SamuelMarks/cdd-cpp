#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace cdd_cpp::utils {

std::pair<std::string, std::string>
map_cpp_type_to_openapi(const std::string &type) {
  if (type == "int" || type == "int32_t" || type == "short")
    return {"integer", "int32"};
  if (type == "long" || type == "int64_t")
    return {"integer", "int64"};
  if (type == "float")
    return {"number", "float"};
  if (type == "double")
    return {"number", "double"};
  if (type == "bool" || type == "boolean")
    return {"boolean", ""};
  if (type == "std::string" || type == "string")
    return {"string", ""};

  if (type.starts_with("std::vector<") || type.starts_with("vector<")) {
    return {"array", ""};
  }

  return {"object", ""};
}



openapi::Schema generate_schema_from_class(const CppClass &cls) {
  openapi::Schema schema;
  schema.type = "object";
  schema.title = cls.name;
  if (!cls.docstring.empty()) {
    schema.description = cls.docstring;
  }
  schema.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
  for (const auto &field : cls.fields) {
    openapi::Schema field_schema;
    auto [type, format] = map_cpp_type_to_openapi(field.type);
    field_schema.type = type;
    if (!format.empty()) {
      field_schema.format = format;
    }
    if (!field.docstring.empty()) {
      field_schema.description = field.docstring;
    }
    (*schema.properties)[field.name] = field_schema;
  }
  return schema;
}

openapi::OpenAPI parse_cpp_project(const std::string &folder_path) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Generated API from C++";
  spec.info.version = "0.0.1";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};
  spec.paths = std::map<std::string, openapi::PathItem>{};

#if 1
  for (const auto &entry : fs::recursive_directory_iterator(folder_path)) {
    if (entry.is_regular_file()) {
      std::string path = entry.path().string();
      if (path.ends_with(".hpp") || path.ends_with(".cpp") ||
          path.ends_with(".h") || path.ends_with(".c")) {
        std::ifstream fs(path);
        std::string content((std::istreambuf_iterator<char>(fs)),
                            std::istreambuf_iterator<char>());
        CppAST ast = parse_cpp(content);

        for (const auto &cls : ast.classes) {
          openapi::Schema schema = generate_schema_from_class(cls);
          spec.components->schemas->insert({schema.title.value_or(cls.name), schema});
        }
      }
    }
  }
#else
  std::cerr << "Directory iteration is disabled in WebAssembly environments.\n";
#endif

  return spec;
}


} // namespace cdd_cpp::utils
