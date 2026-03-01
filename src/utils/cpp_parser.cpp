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

openapi::OpenAPI parse_cpp_project(const std::string &folder_path) {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Generated API from C++";
  spec.info.version = "1.0.0";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};
  spec.paths = std::map<std::string, openapi::PathItem>{};

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
          openapi::Schema s;
          s.type = "object";
          if (!cls.docstring.empty()) {
            std::string d = cls.docstring;
            size_t p = d.find("@description ");
            if (p != std::string::npos)
              d = d.substr(p + 13);
            while (!d.empty() && (d.back() == '\n' || d.back() == '*'))
              d.pop_back();
            while (!d.empty() && std::isspace(d.front()))
              d.erase(d.begin());
            while (!d.empty() && std::isspace(d.back()))
              d.pop_back();
            s.description = d;
          }

          s.properties =
              std::make_shared<std::map<std::string, openapi::Schema>>();
          for (const auto &fld : cls.fields) {
            openapi::Schema ps;
            auto [t, f] = map_cpp_type_to_openapi(fld.type);
            if (t == "object") {
              ps.ref = openapi::Reference{"#/components/schemas/" + fld.type};
            } else if (t == "array") {
              ps.type = "array";
              openapi::Schema items;
              size_t start = fld.type.find('<');
              size_t end = fld.type.rfind('>');
              if (start != std::string::npos && end != std::string::npos) {
                std::string inner = fld.type.substr(start + 1, end - start - 1);
                auto [it, f_it] = map_cpp_type_to_openapi(inner);
                if (it == "object") {
                  items.ref =
                      openapi::Reference{"#/components/schemas/" + inner};
                } else {
                  items.type = it;
                  if (!f_it.empty())
                    items.format = f_it;
                }
              }
              ps.items = std::make_shared<openapi::Schema>(items);
            } else {
              ps.type = t;
              if (!f.empty())
                ps.format = f;
            }
            if (!fld.docstring.empty()) {
              std::string d = fld.docstring;
              size_t p = d.find("@description ");
              if (p != std::string::npos)
                d = d.substr(p + 13);
              ps.description = d;
            }
            s.properties->insert({fld.name, ps});
          }
          spec.components->schemas->insert({cls.name, s});
        }

        for (const auto &fn : ast.functions) {
          std::string route_path;
          std::string method = "get";

          if (fn.docstring.find("@route") != std::string::npos) {
            size_t pos = fn.docstring.find("@route");
            size_t space1 = fn.docstring.find(' ', pos);
            size_t space2 = fn.docstring.find(' ', space1 + 1);
            size_t newline = fn.docstring.find('\n', space2);
            if (newline == std::string::npos)
              newline = fn.docstring.length();

            if (space1 != std::string::npos && space2 != std::string::npos) {
              std::string m =
                  fn.docstring.substr(space1 + 1, space2 - space1 - 1);
              for (auto &c : m)
                c = std::tolower(c);
              method = m;
              route_path =
                  fn.docstring.substr(space2 + 1, newline - space2 - 1);
            }
          } else {
            if (fn.name.starts_with("get_")) {
              method = "get";
              route_path = "/" + fn.name.substr(4);
            } else if (fn.name.starts_with("post_")) {
              method = "post";
              route_path = "/" + fn.name.substr(5);
            } else if (fn.name.starts_with("create_")) {
              method = "post";
              route_path = "/" + fn.name.substr(7);
            } else {
              continue;
            }
          }

          while (!route_path.empty() && std::isspace(route_path.back()))
            route_path.pop_back();

          openapi::Operation op;
          op.operationId = fn.name;

          if (!fn.docstring.empty()) {
            size_t sum_pos = fn.docstring.find("@summary");
            if (sum_pos != std::string::npos) {
              size_t sum_end = fn.docstring.find('\n', sum_pos);
              op.summary =
                  fn.docstring.substr(sum_pos + 9, sum_end - sum_pos - 9);
            }
          }

          if (!fn.params.empty()) {
            op.parameters = std::vector<openapi::Parameter>{};
            for (const auto &p : fn.params) {
              if (p.type == "Client&" || p.type == "cdd_cli::Client&" || p.type == "Client") {
                  continue;
              }
              openapi::Parameter param;
              param.name = p.name;
              if (route_path.find("{" + p.name + "}") != std::string::npos) {
                  param.in = "path";
              } else {
                  param.in = "query";
              }
              param.required = true;
              openapi::Schema ps;
              auto [t, f] = map_cpp_type_to_openapi(p.type);
              ps.type = t;
              if (!f.empty())
                ps.format = f;
              param.schema = ps;
              op.parameters->push_back(param);
            }
          }

          openapi::Response resp;
          resp.description = "Success";
          if (fn.return_type != "void") {
            resp.content = std::map<std::string, openapi::MediaType>{};
            openapi::MediaType mt;
            auto [t, f] = map_cpp_type_to_openapi(fn.return_type);

            if (t == "object") {
              openapi::Schema s;
              s.ref =
                  openapi::Reference{"#/components/schemas/" + fn.return_type};
              mt.schema = s;
            } else if (t == "array") {
              openapi::Schema s;
              s.type = "array";
              openapi::Schema items;
              size_t start = fn.return_type.find('<');
              size_t end = fn.return_type.rfind('>');
              if (start != std::string::npos && end != std::string::npos) {
                std::string inner =
                    fn.return_type.substr(start + 1, end - start - 1);
                auto [it, f_it] = map_cpp_type_to_openapi(inner);
                if (it == "object") {
                  items.ref =
                      openapi::Reference{"#/components/schemas/" + inner};
                } else {
                  items.type = it;
                  if (!f_it.empty())
                    items.format = f_it;
                }
              }
              s.items = std::make_shared<openapi::Schema>(items);
              mt.schema = s;
            } else {
              openapi::Schema s;
              s.type = t;
              if (!f.empty())
                s.format = f;
              mt.schema = s;
            }
            resp.content->insert({"application/json", mt});
          }

          op.responses =
              std::map<std::string, openapi::Response>{{"200", resp}};

          if (!spec.paths->contains(route_path)) {
            spec.paths->insert({route_path, openapi::PathItem{}});
          }

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
        }
      }
    }
  }

  return spec;
}
} // namespace cdd_cpp::utils
