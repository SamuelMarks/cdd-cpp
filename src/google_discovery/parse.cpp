
/// @file parse.cpp
/// @brief Implementation of Google Discovery JSON to OpenAPI 3.2.0 converter.

#include "parse.hpp"
#include <simdjson.h>

namespace cdd_cpp::google_discovery {

/// @brief Convert Google Discovery JSON element via `convert_schema`.
openapi::Schema convert_schema(simdjson::dom::element el) noexcept {
  openapi::Schema out;
  simdjson::dom::object obj;
  if (el.get(obj) == simdjson::SUCCESS) {
    std::string_view sv;
    if (obj["type"].get(sv) == simdjson::SUCCESS) {
      out.type = std::string(sv);
    }
    if (obj["format"].get(sv) == simdjson::SUCCESS) {
      out.format = std::string(sv);
    }
    if (obj["description"].get(sv) == simdjson::SUCCESS) {
      out.description = std::string(sv);
    }
    if (obj["$ref"].get(sv) == simdjson::SUCCESS) {
      out.ref =
          openapi::Reference{.ref = "#/components/schemas/" + std::string(sv)};
    }
    simdjson::dom::element items_el;
    if (obj["items"].get(items_el) == simdjson::SUCCESS) {
      out.items = std::make_shared<openapi::Schema>(convert_schema(items_el));
    }
    simdjson::dom::object props_obj;
    if (obj["properties"].get(props_obj) == simdjson::SUCCESS) {
      out.properties =
          std::make_shared<std::map<std::string, openapi::Schema>>();
      for (auto prop : props_obj) {
        out.properties->insert(
            {std::string(prop.key), convert_schema(prop.value)});
      }
    }
  }
  return out;
}

/// @brief Convert Google Discovery JSON element via `process_methods`.
void process_methods(simdjson::dom::object methods,
                     std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto m : methods) {
    simdjson::dom::object method_obj;
    if (m.value.get(method_obj) != simdjson::SUCCESS)
      continue;

    std::string httpMethod = "GET";
    std::string_view sv;
    if (method_obj["httpMethod"].get(sv) == simdjson::SUCCESS) {
      httpMethod = std::string(sv);
    }

    std::string path = "/";
    if (method_obj["path"].get(sv) == simdjson::SUCCESS) {
      path = "/" + std::string(sv);
      size_t pos = 0;
      while ((pos = path.find("{+", pos)) != std::string::npos) {
        path.replace(pos, 2, "{");
      }
    }

    openapi::Operation op;
    if (method_obj["id"].get(sv) == simdjson::SUCCESS) {
      std::string id_str(sv);
      for (char &c : id_str) {
        if (c == '.')
          c = '_';
      }
      op.operationId = id_str;
    }
    if (method_obj["description"].get(sv) == simdjson::SUCCESS) {
      op.description = std::string(sv);
    }

    simdjson::dom::object params_obj;
    if (method_obj["parameters"].get(params_obj) == simdjson::SUCCESS) {
      op.parameters = std::vector<openapi::Parameter>{};
      for (auto p : params_obj) {
        openapi::Parameter param;
        param.name = std::string(p.key);
        simdjson::dom::object p_obj;
        if (p.value.get(p_obj) == simdjson::SUCCESS) {
          if (p_obj["location"].get(sv) == simdjson::SUCCESS) {
            param.in = std::string(sv);
          } else {
            param.in = "query";
          }
          bool req_b = false;
          if (p_obj["required"].get(req_b) == simdjson::SUCCESS) {
            param.required = req_b;
          }
          param.schema = convert_schema(p.value);
        }
        op.parameters->push_back(param);
      }
    }

    if (method_obj["request"]["$ref"].get(sv) == simdjson::SUCCESS) {
      openapi::RequestBody reqBody;
      reqBody.content = std::map<std::string, openapi::MediaType>{};
      openapi::MediaType mt;
      openapi::Schema s;
      s.ref =
          openapi::Reference{.ref = "#/components/schemas/" + std::string(sv)};
      mt.schema = s;
      reqBody.content.insert({"application/json", mt});
      op.requestBody = reqBody;
    }

    op.responses = std::map<std::string, openapi::Response>{};
    openapi::Response r200;
    r200.description = "Successful response";
    if (method_obj["response"]["$ref"].get(sv) == simdjson::SUCCESS) {
      r200.content = std::map<std::string, openapi::MediaType>{};
      openapi::MediaType mt;
      openapi::Schema s;
      s.ref =
          openapi::Reference{.ref = "#/components/schemas/" + std::string(sv)};
      mt.schema = s;
      r200.content->insert({"application/json", mt});
    }
    op.responses->insert({"200", r200});

    if (!paths.contains(path)) {
      paths[path] = openapi::PathItem{};
    }

    if (httpMethod == "GET")
      paths[path].get = op;
    else if (httpMethod == "POST")
      paths[path].post = op;
    else if (httpMethod == "PUT")
      paths[path].put = op;
    else if (httpMethod == "DELETE")
      paths[path].delete_op = op;
    else if (httpMethod == "PATCH")
      paths[path].patch = op;
  }
}

/// @brief Convert Google Discovery JSON element via `process_resources`.
void process_resources(
    simdjson::dom::object resources,
    std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto r : resources) {
    simdjson::dom::object res_obj;
    if (r.value.get(res_obj) == simdjson::SUCCESS) {
      simdjson::dom::object methods_obj;
      if (res_obj["methods"].get(methods_obj) == simdjson::SUCCESS) {
        process_methods(methods_obj, paths);
      }
      simdjson::dom::object sub_res_obj;
      if (res_obj["resources"].get(sub_res_obj) == simdjson::SUCCESS) {
        process_resources(sub_res_obj, paths);
      }
    }
  }
}

/// @brief Convert Google Discovery JSON element via `parse`.
Result<std::vector<openapi::OpenAPI>> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(input).get(doc);
  if (error) {
    return utils::make_error(utils::ErrorCode::JsonParseError,
                             simdjson::error_message(error));
  }
  simdjson::dom::object root;
  error = doc.get_object().get(root);
  if (error) {
    return utils::make_error(utils::ErrorCode::InvalidFieldType,
                             simdjson::error_message(error));
  }

  std::vector<openapi::OpenAPI> results;

  std::string_view kind_sv;
  if (root["kind"].get(kind_sv) == simdjson::SUCCESS) {
    std::string kind(kind_sv);

    if (kind == "discovery#directoryList") {
      simdjson::dom::array items_arr;
      if (root["items"].get(items_arr) == simdjson::SUCCESS) {
        for (auto item : items_arr) {
          simdjson::dom::object item_obj;
          if (item.get(item_obj) == simdjson::SUCCESS) {
            openapi::OpenAPI spec;
            spec.openapi = "3.2.0";

            std::string_view sv;
            if (item_obj["title"].get(sv) == simdjson::SUCCESS)
              spec.info.title = std::string(sv);
            if (item_obj["version"].get(sv) == simdjson::SUCCESS)
              spec.info.version = std::string(sv);
            if (item_obj["description"].get(sv) == simdjson::SUCCESS)
              spec.info.description = std::string(sv);
            if (item_obj["discoveryRestUrl"].get(sv) == simdjson::SUCCESS) {
              openapi::Server srv;
              srv.url = std::string(sv);
              spec.servers = std::vector<openapi::Server>{srv};
            }
            results.push_back(spec);
          }
        }
      }
    } else if (kind == "discovery#restDescription") {
      openapi::OpenAPI spec;
      spec.openapi = "3.2.0";

      std::string_view sv;
      if (root["title"].get(sv) == simdjson::SUCCESS)
        spec.info.title = std::string(sv);
      if (root["version"].get(sv) == simdjson::SUCCESS)
        spec.info.version = std::string(sv);
      if (root["description"].get(sv) == simdjson::SUCCESS)
        spec.info.description = std::string(sv);

      std::string rootUrl = "";
      std::string servicePath = "";
      if (root["rootUrl"].get(sv) == simdjson::SUCCESS)
        rootUrl = std::string(sv);
      if (root["servicePath"].get(sv) == simdjson::SUCCESS)
        servicePath = std::string(sv);

      if (!rootUrl.empty()) {
        openapi::Server srv;
        srv.url = rootUrl + servicePath;
        spec.servers = std::vector<openapi::Server>{srv};
      }

      simdjson::dom::object schemas_obj;
      if (root["schemas"].get(schemas_obj) == simdjson::SUCCESS) {
        spec.components = openapi::Components{};
        spec.components->schemas = std::map<std::string, openapi::Schema>{};
        for (auto s : schemas_obj) {
          spec.components->schemas->insert(
              {std::string(s.key), convert_schema(s.value)});
        }
      }

      spec.paths = std::map<std::string, openapi::PathItem>{};

      simdjson::dom::object methods_obj;
      if (root["methods"].get(methods_obj) == simdjson::SUCCESS) {
        process_methods(methods_obj, spec.paths.operator*());
      }

      simdjson::dom::object resources_obj;
      if (root["resources"].get(resources_obj) == simdjson::SUCCESS) {
        process_resources(resources_obj, spec.paths.operator*());
      }

      results.push_back(spec);
    }
  }
  return results;
}
} // namespace cdd_cpp::google_discovery
