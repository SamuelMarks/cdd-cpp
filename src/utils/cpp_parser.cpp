#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

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

openapi::OpenAPI parse_cpp_project(const std::string &folder_path) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Generated API from C++";
  spec.info.version = "0.0.3";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};
  spec.paths = std::map<std::string, openapi::PathItem>{};

#if 1
  std::regex route_regex(
      "svr\\.(Get|Post|Put|Delete|Patch|Options|Head)\\(\\s*\"([^\"]+)\"(?:\\s*"
      ",\\s*\\[[^\\]]*\\]\\s*\\([^\\)]*\\)\\s*\\{([^}]*)\\})?");
  std::regex path_param_regex(
      "req\\.path_params\\.at\\(\\s*\"([^\"]+)\"\\s*\\)");
  std::regex query_regex("req\\.get_param_value\\(\\s*\"([^\"]+)\"\\s*\\)");
  std::regex header_regex("req\\.get_header_value\\(\\s*\"([^\"]+)\"\\s*\\)");
  std::regex body_regex("body\\.get<([^>]+)>\\(\\)");

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
          spec.components->schemas->insert(
              {schema.title.value_or(cls.name), schema});
        }

        std::sregex_iterator route_it(content.begin(), content.end(),
                                      route_regex);
        std::sregex_iterator end;
        while (route_it != end) {
          std::string method = (*route_it)[1].str();
          std::string route_path = (*route_it)[2].str();
          std::string body = (*route_it)[3].str();

          for (auto &c : method)
            c = std::tolower(c);

          if (!spec.paths->count(route_path)) {
            spec.paths->insert({route_path, openapi::PathItem{}});
          }

          openapi::Operation op;
          op.operationId = method + "_" + route_path;
          for (auto &c : op.operationId.value()) {
            if (c == '/' || c == '{' || c == '}')
              c = '_';
          }

          op.parameters = std::vector<openapi::Parameter>{};

          std::sregex_iterator p_it(body.begin(), body.end(), path_param_regex);
          while (p_it != end) {
            openapi::Parameter p;
            p.name = (*p_it)[1].str();
            p.in = "path";
            p.required = true;
            p.schema = openapi::Schema{};
            p.schema->type = "string";
            op.parameters->push_back(p);
            p_it++;
          }

          std::sregex_iterator q_it(body.begin(), body.end(), query_regex);
          while (q_it != end) {
            openapi::Parameter p;
            p.name = (*q_it)[1].str();
            p.in = "query";
            p.required = false;
            p.schema = openapi::Schema{};
            p.schema->type = "string";
            op.parameters->push_back(p);
            q_it++;
          }

          std::sregex_iterator h_it(body.begin(), body.end(), header_regex);
          while (h_it != end) {
            openapi::Parameter p;
            p.name = (*h_it)[1].str();
            p.in = "header";
            p.required = false;
            p.schema = openapi::Schema{};
            p.schema->type = "string";
            op.parameters->push_back(p);
            h_it++;
          }

          std::sregex_iterator b_it(body.begin(), body.end(), body_regex);
          if (b_it != end) {
            std::string model_type = (*b_it)[1].str();
            openapi::RequestBody req_body;
            req_body.required = true;
            openapi::MediaType mt;
            mt.schema = openapi::Schema{};
            mt.schema->ref =
                openapi::Reference{"#/components/schemas/" + model_type,
                                   std::nullopt, std::nullopt};
            req_body.content["application/json"] = mt;
            op.requestBody = req_body;
          }

          if (op.parameters->empty())
            op.parameters.reset();

          if (method == "get")
            spec.paths->at(route_path).get = op;
          else if (method == "post")
            spec.paths->at(route_path).post = op;
          else if (method == "put")
            spec.paths->at(route_path).put = op;
          else if (method == "delete")
            spec.paths->at(route_path).delete_op = op;
          else if (method == "patch")
            spec.paths->at(route_path).patch = op;
          else if (method == "options")
            spec.paths->at(route_path).options = op;
          else if (method == "head")
            spec.paths->at(route_path).head = op;

          route_it++;
        }

        std::regex client_regex(
            "std::expected<std::string,\\s*std::string>\\s+Client::(\\w+)\\(([^"
            ")]*)\\)\\s*noexcept\\s*\\{([\\s\\S]*?)\\n\\s*\\}");
        std::sregex_iterator client_it(content.begin(), content.end(),
                                       client_regex);
        while (client_it != end) {
          std::string func_name = (*client_it)[1].str();
          std::string params_str = (*client_it)[2].str();
          std::string body = (*client_it)[3].str();

          std::regex url_regex(
              "full_url\\s*=\\s*base_url\\s*\\+\\s*\"([^\"]+)\"");
          std::smatch match_url;
          std::string route_path = "";
          if (std::regex_search(body, match_url, url_regex)) {
            route_path = match_url[1].str();
          }

          std::regex method_regex("CURLOPT_CUSTOMREQUEST,\\s*\"([^\"]+)\"");
          std::smatch match_method;
          std::string method = "get";
          if (std::regex_search(body, match_method, method_regex)) {
            method = match_method[1].str();
            for (auto &c : method)
              c = std::tolower(c);
          }

          if (!route_path.empty()) {
            // Replace <var> or {var} to OpenAPI {} format
            std::regex var_regex("<([^>]+)>");
            route_path = std::regex_replace(route_path, var_regex, "{$1}");

            if (!spec.paths->count(route_path)) {
              spec.paths->insert({route_path, openapi::PathItem{}});
            }
            openapi::Operation op;
            op.operationId = func_name;

            op.parameters = std::vector<openapi::Parameter>{};

            std::regex param_extract("(?:const\\s+)?([\\w:]+(?:<\\w+>)?)(?:\\s*"
                                     "&|\\s*\\*|\\s+)\\s*(\\w+)");
            std::sregex_iterator p_it(params_str.begin(), params_str.end(),
                                      param_extract);
            while (p_it != end) {
              std::string p_type = (*p_it)[1].str();
              std::string p_name = (*p_it)[2].str();

              if (p_name == "body") {
                openapi::RequestBody req_body;
                req_body.required = true;
                openapi::MediaType mt;
                mt.schema = openapi::Schema{};
                mt.schema->type = "string";
                req_body.content["application/json"] = mt;
                op.requestBody = req_body;
              } else {
                openapi::Parameter p;
                p.name = p_name;
                p.in = route_path.find("{" + p_name + "}") != std::string::npos
                           ? "path"
                           : "query";
                p.required = p.in == "path";
                p.schema = openapi::Schema{};
                p.schema->type =
                    p_type == "int" || p_type == "double" ? "number" : "string";
                op.parameters->push_back(p);
              }
              p_it++;
            }

            if (op.parameters->empty())
              op.parameters.reset();

            if (method == "get")
              spec.paths->at(route_path).get = op;
            else if (method == "post")
              spec.paths->at(route_path).post = op;
            else if (method == "put")
              spec.paths->at(route_path).put = op;
            else if (method == "delete")
              spec.paths->at(route_path).delete_op = op;
            else if (method == "patch")
              spec.paths->at(route_path).patch = op;
            else if (method == "options")
              spec.paths->at(route_path).options = op;
            else if (method == "head")
              spec.paths->at(route_path).head = op;
          }

          client_it++;
        }

        std::regex cli_exec_regex(
            R"(handle_([a-zA-Z0-9_]+)\(client(?:,\s*([^)]+))?\))");
        std::sregex_iterator cli_it(content.begin(), content.end(),
                                    cli_exec_regex);
        while (cli_it != end) {
          std::string op_id = (*cli_it)[1].str();
          std::string args = (*cli_it)[2].str();

          std::string dummy_path = "/cli/" + op_id;
          if (!spec.paths->count(dummy_path)) {
            spec.paths->insert({dummy_path, openapi::PathItem{}});
          }

          openapi::Operation op;
          op.operationId = op_id;
          op.parameters = std::vector<openapi::Parameter>{};

          std::regex arg_extract(
              "(path_params|query_params|body)(?:\\[\"([^\"]+)\"\\])?");
          std::sregex_iterator arg_it(args.begin(), args.end(), arg_extract);
          while (arg_it != end) {
            std::string type = (*arg_it)[1].str();
            std::string name = (*arg_it)[2].str();

            if (type == "body") {
              openapi::RequestBody req_body;
              req_body.required = true;
              openapi::MediaType mt;
              mt.schema = openapi::Schema{};
              mt.schema->type = "string";
              req_body.content["application/json"] = mt;
              op.requestBody = req_body;
            } else {
              openapi::Parameter p;
              p.name = name.empty() ? "param" : name;
              p.in = type == "path_params" ? "path" : "query";
              p.required = p.in == "path";
              p.schema = openapi::Schema{};
              p.schema->type = "string";
              op.parameters->push_back(p);
            }
            arg_it++;
          }

          if (op.parameters->empty())
            op.parameters.reset();
          spec.paths->at(dummy_path).post = op;

          cli_it++;
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
