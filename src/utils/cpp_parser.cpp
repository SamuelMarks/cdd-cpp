/// @file cpp_parser.cpp
/// @brief Implementation of recursive C++ project directory scanner and OpenAPI
/// extractor.

#include "cpp_parser.hpp"
#include "../client_sdk/parse.hpp"
#include "../client_sdk_cli/parse.hpp"
#include "../server/parse.hpp"
#include "cpp_ast.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace cdd_cpp::utils {

/// @brief Map a C++ field type to an OpenAPI (type, format) pair.
/// @param type The C++ type string.
/// @return Pair of OpenAPI type and optional format string.
std::pair<std::string, std::string>
map_cpp_type_to_openapi(const std::string &type) noexcept {
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

/// @brief Generate an OpenAPI Schema from a parsed CppClass AST node.
/// @param cls The CppClass AST node.
/// @return Populated OpenAPI Schema.
openapi::Schema generate_schema_from_class(const CppClass &cls) noexcept {
  openapi::Schema schema;
  schema.type = "object";
  schema.title = cls.name;
  if (!cls.docstring.empty()) {
    schema.description = cls.docstring;
  }
  schema.properties =
      std::make_shared<std::map<std::string, openapi::Schema>>();
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

/// @brief Merge paths from a source OpenAPI spec into a destination PathItem
/// map without clobbering existing operations.
/// @param dest Destination PathItem map.
/// @param src Source PathItem map.
static void
merge_paths(std::map<std::string, openapi::PathItem> &dest,
            const std::map<std::string, openapi::PathItem> &src) noexcept {
  for (const auto &[path, pi] : src) {
    auto &target = dest[path];
    for (auto hm : {openapi::HttpMethod::Get, openapi::HttpMethod::Put,
                    openapi::HttpMethod::Post, openapi::HttpMethod::Delete,
                    openapi::HttpMethod::Options, openapi::HttpMethod::Head,
                    openapi::HttpMethod::Patch, openapi::HttpMethod::Trace,
                    openapi::HttpMethod::Query}) {
      if (pi.operation_for(hm).has_value() &&
          !target.operation_for(hm).has_value()) {
        target.operation_for(hm) = pi.operation_for(hm);
      }
    }
  }
}

/// @brief Extract all quoted arguments from calls like `<prefix>("...")` in
/// `body`.
/// @param body Source string to scan.
/// @param call_prefix Prefix before `(` (e.g., `req.path_params.at`).
/// @return Vector of extracted string literals.
static std::vector<std::string>
extract_call_string_args(std::string_view body,
                         std::string_view call_prefix) noexcept {
  std::vector<std::string> res;
  std::string needle = std::string(call_prefix) + "(\"";
  size_t pos = 0;
  for (;;) {
    size_t idx = body.find(needle, pos);
    if (idx == std::string_view::npos)
      break;
    size_t q1 = idx + needle.size();
    size_t q2 = body.find('"', q1);
    res.emplace_back(body.substr(q1, q2 - q1));
    pos = q2 + 1;
  }
  return res;
}

/// @brief Scan `svr.<Method>("/path", ...)` route registrations in `content`.
/// @param content C++ source file content.
/// @param spec OpenAPI specification to populate.
static void scan_httplib_routes(std::string_view content,
                                openapi::OpenAPI &spec) noexcept {
  size_t pos = 0;
  for (;;) {
    size_t idx = content.find("svr.", pos);
    if (idx == std::string_view::npos)
      break;
    size_t open_p = content.find('(', idx + 4);
    std::string matched_method(content.substr(idx + 4, open_p - (idx + 4)));
    size_t q1 = content.find('"', open_p + 1);
    size_t q2 = content.find('"', q1 + 1);
    std::string route_path(content.substr(q1 + 1, q2 - q1 - 1));

    size_t open_b = content.find('{', q2 + 1);
    size_t close_b = content.find('}', open_b + 1);
    std::string_view body_sv = content.substr(open_b + 1, close_b - open_b - 1);
    pos = close_b + 1;

    openapi::Operation op;
    std::string lower_method = matched_method;
    for (auto &c : lower_method)
      c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    std::string op_id = lower_method + "_" + route_path;
    for (auto &c : op_id) {
      if (c == '/' || c == '{' || c == '}')
        c = '_';
    }
    op.operationId = op_id;
    op.parameters = std::vector<openapi::Parameter>{};

    for (const auto &pname :
         extract_call_string_args(body_sv, "req.path_params.at")) {
      openapi::Parameter p;
      p.name = pname;
      p.in = "path";
      p.required = true;
      p.schema = openapi::Schema{};
      p.schema->type = "string";
      op.parameters->push_back(p);
    }
    for (const auto &qname :
         extract_call_string_args(body_sv, "req.get_param_value")) {
      openapi::Parameter p;
      p.name = qname;
      p.in = "query";
      p.required = false;
      p.schema = openapi::Schema{};
      p.schema->type = "string";
      op.parameters->push_back(p);
    }
    for (const auto &hname :
         extract_call_string_args(body_sv, "req.get_header_value")) {
      openapi::Parameter p;
      p.name = hname;
      p.in = "header";
      p.required = false;
      p.schema = openapi::Schema{};
      p.schema->type = "string";
      op.parameters->push_back(p);
    }

    size_t bg = body_sv.find("body.get<");
    if (bg != std::string_view::npos) {
      size_t gt = body_sv.find(">()", bg + 9);
      std::string model_type(body_sv.substr(bg + 9, gt - bg - 9));
      openapi::RequestBody req_body;
      req_body.required = true;
      openapi::MediaType mt;
      mt.schema = openapi::Schema{};
      mt.schema->ref = openapi::Reference{"#/components/schemas/" + model_type,
                                          std::nullopt, std::nullopt};
      req_body.content["application/json"] = mt;
      op.requestBody = req_body;
    }

    if (op.parameters->empty())
      op.parameters.reset();

    (*spec.paths)[route_path].set_operation(matched_method, std::move(op));
  }
}

/// @brief Scan `handle_<op_id>(client, ...)` calls in `content`.
/// @param content C++ source file content.
/// @param spec OpenAPI specification to populate.
static void scan_cli_handler_calls(std::string_view content,
                                   openapi::OpenAPI &spec) noexcept {
  size_t pos = 0;
  for (;;) {
    size_t idx = content.find("handle_", pos);
    if (idx == std::string_view::npos)
      break;
    size_t name_start = idx + 7;
    size_t name_end = name_start;
    while (std::isalnum(static_cast<unsigned char>(content[name_end])) ||
           content[name_end] == '_') {
      ++name_end;
    }
    if (!content.substr(name_end).starts_with("(client")) {
      pos = name_end + 1;
      continue;
    }
    size_t close_p = content.find(')', name_end + 7);

    std::string op_id(content.substr(name_start, name_end - name_start));
    std::string_view args_sv =
        content.substr(name_end + 7, close_p - (name_end + 7));

    bool already_parsed = false;
    for (const auto &[_, pi] : *spec.paths) {
      for (auto hm : {openapi::HttpMethod::Get, openapi::HttpMethod::Put,
                      openapi::HttpMethod::Post, openapi::HttpMethod::Delete,
                      openapi::HttpMethod::Options, openapi::HttpMethod::Head,
                      openapi::HttpMethod::Patch, openapi::HttpMethod::Trace,
                      openapi::HttpMethod::Query}) {
        if (pi.operation_for(hm).has_value() &&
            pi.operation_for(hm)->operationId == op_id) {
          already_parsed = true;
        }
      }
    }

    if (!already_parsed) {
      std::string dummy_path = "/cli/" + op_id;
      openapi::Operation op;
      op.operationId = op_id;
      op.parameters = std::vector<openapi::Parameter>{};

      size_t apos = 0;
      while (apos < args_sv.size()) {
        size_t comma = args_sv.find(',', apos);
        std::string_view tok = (comma == std::string_view::npos)
                                   ? args_sv.substr(apos)
                                   : args_sv.substr(apos, comma - apos);
        apos = (comma == std::string_view::npos) ? args_sv.size() : comma + 1;
        size_t ts = tok.find_first_not_of(" \t\r\n");
        if (ts == std::string_view::npos)
          continue;
        tok.remove_prefix(ts);
        size_t te = tok.find_last_not_of(" \t\r\n");
        tok = tok.substr(0, te + 1);

        if (tok == "body") {
          openapi::RequestBody req_body;
          req_body.required = true;
          openapi::MediaType mt;
          mt.schema = openapi::Schema{};
          mt.schema->type = "string";
          req_body.content["application/json"] = mt;
          op.requestBody = req_body;
        } else {
          bool is_path = tok.starts_with("path_params");
          std::string pname = "param";
          size_t q1 = tok.find("[\"");
          if (q1 != std::string_view::npos) {
            size_t q2 = tok.find("\"]", q1 + 2);
            pname = std::string(tok.substr(q1 + 2, q2 - q1 - 2));
          }
          openapi::Parameter p;
          p.name = pname;
          p.in = is_path ? "path" : "query";
          p.required = is_path;
          p.schema = openapi::Schema{};
          p.schema->type = "string";
          op.parameters->push_back(p);
        }
      }

      if (op.parameters->empty())
        op.parameters.reset();
      (*spec.paths)[dummy_path].post = op;
    }

    pos = close_p + 1;
  }
}

Result<openapi::OpenAPI>
try_parse_cpp_project(const std::string &folder_path) noexcept {
  std::error_code ec;
  fs::recursive_directory_iterator it(folder_path, ec);
  if (ec) {
    return make_error(ErrorCode::DirectoryIterationError, ec.message(),
                      folder_path);
  }

  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Generated API from C++";
  spec.info.version = "0.0.3";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};
  spec.paths = std::map<std::string, openapi::PathItem>{};

  for (fs::recursive_directory_iterator end; it != end; it.increment(ec)) {
    if (it->is_regular_file(ec)) {
      std::string path = it->path().string();
      if (path.ends_with(".hpp") || path.ends_with(".cpp") ||
          path.ends_with(".h") || path.ends_with(".c")) {
        std::ifstream fs(path);
        std::string content((std::istreambuf_iterator<char>(fs)),
                            std::istreambuf_iterator<char>());
        CppAST ast = parse_cpp(content);

        for (const auto &cls : ast.classes) {
          openapi::Schema schema = generate_schema_from_class(cls);
          spec.components->schemas->insert(
              {schema.title.value_or(cls.name), schema});
        }

        merge_paths(*spec.paths, *client_sdk::parse(content)->paths);
        merge_paths(*spec.paths, *server::parse(content)->paths);
        merge_paths(*spec.paths, *client_sdk_cli::parse(content)->paths);

        scan_httplib_routes(content, spec);
        scan_cli_handler_calls(content, spec);
      }
    }
  }

  return spec;
}

/// @brief Internal helper or implementation for `parse_cpp_project`.
openapi::OpenAPI parse_cpp_project(const std::string &folder_path) noexcept {
  auto res = try_parse_cpp_project(folder_path);
  if (res.has_value()) {
    return *res;
  }
  openapi::OpenAPI fallback;
  fallback.openapi = "3.2.0";
  fallback.info.title = "Generated API from C++";
  fallback.info.version = "0.0.3";
  fallback.components = openapi::Components{};
  fallback.components->schemas = std::map<std::string, openapi::Schema>{};
  fallback.paths = std::map<std::string, openapi::PathItem>{};
  return fallback;
}

} // namespace cdd_cpp::utils
