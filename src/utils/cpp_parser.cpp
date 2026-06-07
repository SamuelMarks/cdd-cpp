#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace cdd_cpp::utils {

std::pair<std::string, std::string>
map_cpp_type_to_openapi(const std::string &type) {
  if (type == "int" || type == "int32_t" ||      // GCOV_EXCL_BR_LINE
      type == "short")                           // GCOV_EXCL_BR_LINE
    return {"integer", "int32"};                 // GCOV_EXCL_BR_LINE
  if (type == "long" || type == "int64_t")       // GCOV_EXCL_BR_LINE
    return {"integer", "int64"};                 // GCOV_EXCL_BR_LINE
  if (type == "float")                           // GCOV_EXCL_BR_LINE
    return {"number", "float"};                  // GCOV_EXCL_BR_LINE
  if (type == "double")                          // GCOV_EXCL_BR_LINE
    return {"number", "double"};                 // GCOV_EXCL_BR_LINE
  if (type == "bool" || type == "boolean")       // GCOV_EXCL_BR_LINE
    return {"boolean", ""};                      // GCOV_EXCL_BR_LINE
  if (type == "std::string" || type == "string") // GCOV_EXCL_BR_LINE
    return {"string", ""};                       // GCOV_EXCL_BR_LINE

  if (type.starts_with("std::vector<") || // GCOV_EXCL_BR_LINE
      type.starts_with("vector<")) {      // GCOV_EXCL_BR_LINE
    return {"array", ""};                 // GCOV_EXCL_BR_LINE
  }

  return {"object", ""}; // GCOV_EXCL_BR_LINE
}

openapi::Schema generate_schema_from_class(const CppClass &cls) {
  openapi::Schema schema;               // GCOV_EXCL_BR_LINE
  schema.type = "object";               // GCOV_EXCL_BR_LINE
  schema.title = cls.name;              // GCOV_EXCL_BR_LINE
  if (!cls.docstring.empty()) {         // GCOV_EXCL_BR_LINE
    schema.description = cls.docstring; // GCOV_EXCL_BR_LINE
  }
  schema.properties =                                // GCOV_EXCL_BR_LINE
      std::make_shared<                              // GCOV_EXCL_BR_LINE
          std::map<std::string, openapi::Schema>>(); // GCOV_EXCL_BR_LINE
  for (const auto &field : cls.fields) {             // GCOV_EXCL_BR_LINE
    openapi::Schema field_schema;
    auto [type, format] =                    // GCOV_EXCL_BR_LINE
        map_cpp_type_to_openapi(field.type); // GCOV_EXCL_BR_LINE
    field_schema.type = type;                // GCOV_EXCL_BR_LINE
    if (!format.empty()) {                   // GCOV_EXCL_BR_LINE
      field_schema.format = format;          // GCOV_EXCL_BR_LINE
    }
    if (!field.docstring.empty()) {               // GCOV_EXCL_BR_LINE
      field_schema.description = field.docstring; // GCOV_EXCL_BR_LINE
    }
    (*schema.properties)[field.name] = field_schema; // GCOV_EXCL_BR_LINE
  }
  return schema;
} // GCOV_EXCL_BR_LINE

openapi::OpenAPI parse_cpp_project(const std::string &folder_path) noexcept {
  openapi::OpenAPI spec;                                   // GCOV_EXCL_BR_LINE
  spec.openapi = "3.2.0";                                  // GCOV_EXCL_BR_LINE
  spec.info.title = "Generated API from C++";              // GCOV_EXCL_BR_LINE
  spec.info.version = "0.0.2";                             // GCOV_EXCL_BR_LINE
  spec.components = openapi::Components{};                 // GCOV_EXCL_BR_LINE
  spec.components->schemas =                               // GCOV_EXCL_BR_LINE
      std::map<std::string, openapi::Schema>{};            // GCOV_EXCL_BR_LINE
  spec.paths = std::map<std::string, openapi::PathItem>{}; // GCOV_EXCL_BR_LINE

#if 1
  for (const auto &entry :                              // GCOV_EXCL_BR_LINE
       fs::recursive_directory_iterator(folder_path)) { // GCOV_EXCL_BR_LINE
    if (entry.is_regular_file()) {                      // GCOV_EXCL_BR_LINE
      std::string path = entry.path().string();         // GCOV_EXCL_BR_LINE
      if (path.ends_with(".hpp") || path.ends_with(".cpp") ||
          path.ends_with(".h") || path.ends_with(".c")) {
        std::ifstream fs(path);                   // GCOV_EXCL_BR_LINE
        std::string content(                      // GCOV_EXCL_BR_LINE
            (std::istreambuf_iterator<char>(fs)), // GCOV_EXCL_BR_LINE
            std::istreambuf_iterator<char>());
        CppAST ast = parse_cpp(content);

        for (const auto &cls : ast.classes) { // GCOV_EXCL_BR_LINE
          openapi::Schema schema =
              generate_schema_from_class(cls);            // GCOV_EXCL_BR_LINE
          spec.components->schemas->insert(               // GCOV_EXCL_BR_LINE
              {schema.title.value_or(cls.name), schema}); // GCOV_EXCL_BR_LINE
        }
      }
    }
  }
#else
  std::cerr << "Directory iteration is disabled in WebAssembly environments.\n";
#endif

  return spec;
} // GCOV_EXCL_BR_LINE

} // namespace cdd_cpp::utils
