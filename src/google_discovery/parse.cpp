#include <expected>
#include "parse.hpp"
#include <simdjson.h>

namespace cdd_cpp::google_discovery {

openapi::Schema convert_schema(simdjson::dom::element el) noexcept {
  openapi::Schema out;
  if (el.is_object()) {
    auto obj = el.get_object();
    simdjson::dom::element type_el;
    if (obj["type"].get(type_el) == simdjson::SUCCESS && type_el.is_string()) {
      out.type = std::string(type_el.get_string().value());
    }
    simdjson::dom::element format_el;
    if (obj["format"].get(format_el) == simdjson::SUCCESS &&
        format_el.is_string()) {
      out.format = std::string(format_el.get_string().value());
    }
    simdjson::dom::element desc_el;
    if (obj["description"].get(desc_el) == simdjson::SUCCESS &&
        desc_el.is_string()) {
      out.description = std::string(desc_el.get_string().value());
    }
    simdjson::dom::element ref_el;
    if (obj["$ref"].get(ref_el) == simdjson::SUCCESS && ref_el.is_string()) {
      out.ref =
          openapi::Reference{.ref = "#/components/schemas/" +
                                    std::string(ref_el.get_string().value())};
    }
    simdjson::dom::element items_el;
    if (obj["items"].get(items_el) == simdjson::SUCCESS) {
      out.items = std::make_shared<openapi::Schema>(convert_schema(items_el));
    }
    simdjson::dom::element props_el;
    if (obj["properties"].get(props_el) == simdjson::SUCCESS &&
        props_el.is_object()) {
      out.properties =
          std::make_shared<std::map<std::string, openapi::Schema>>();
      for (auto prop : props_el.get_object()) {
        out.properties->insert(
            {std::string(prop.key), convert_schema(prop.value)});
      }
    }
  }
  return out;
}

void process_methods(simdjson::dom::object methods,
                     std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto m : methods) {
    if (!m.value.is_object())
      continue;
    auto method_obj = m.value.get_object();

    std::string httpMethod = "GET";
    simdjson::dom::element hm_el;
    if (method_obj["httpMethod"].get(hm_el) == simdjson::SUCCESS &&
        hm_el.is_string()) {
      httpMethod = std::string(hm_el.get_string().value());
    }

    std::string path = "/";
    simdjson::dom::element path_el;
    if (method_obj["path"].get(path_el) == simdjson::SUCCESS &&
        path_el.is_string()) {
      path = "/" + std::string(path_el.get_string().value());
      size_t pos = 0;
      while ((pos = path.find("{+", pos)) != std::string::npos) {
        path.replace(pos, 2, "{");
      }
    }

    openapi::Operation op;
    simdjson::dom::element id_el;
    if (method_obj["id"].get(id_el) == simdjson::SUCCESS && id_el.is_string()) {
      op.operationId = std::string(id_el.get_string().value());
    }
    simdjson::dom::element desc_el;
    if (method_obj["description"].get(desc_el) == simdjson::SUCCESS &&
        desc_el.is_string()) {
      op.description = std::string(desc_el.get_string().value());
    }

    simdjson::dom::element params_el;
    if (method_obj["parameters"].get(params_el) == simdjson::SUCCESS &&
        params_el.is_object()) {
      op.parameters = std::vector<openapi::Parameter>{};
      for (auto p : params_el.get_object()) {
        openapi::Parameter param;
        param.name = std::string(p.key);
        if (p.value.is_object()) {
          auto p_obj = p.value.get_object();
          simdjson::dom::element loc_el;
          if (p_obj["location"].get(loc_el) == simdjson::SUCCESS &&
              loc_el.is_string()) {
            param.in = std::string(loc_el.get_string().value());
          } else {
            param.in = "query";
          }
          simdjson::dom::element req_el;
          if (p_obj["required"].get(req_el) == simdjson::SUCCESS &&
              req_el.is_bool()) {
            param.required = req_el.get_bool().value();
          }
          param.schema = convert_schema(p.value);
        }
        op.parameters->push_back(param);
      }
    }

    simdjson::dom::element req_el;
    if (method_obj["request"].get(req_el) == simdjson::SUCCESS &&
        req_el.is_object()) {
      simdjson::dom::element ref_el;
      if (req_el.get_object()["$ref"].get(ref_el) == simdjson::SUCCESS &&
          ref_el.is_string()) {
        openapi::RequestBody reqBody;
        reqBody.content = std::map<std::string, openapi::MediaType>{};
        openapi::MediaType mt;
        openapi::Schema s;
        s.ref =
            openapi::Reference{.ref = "#/components/schemas/" +
                                      std::string(ref_el.get_string().value())};
        mt.schema = s;
        reqBody.content.insert({"application/json", mt});
        op.requestBody = reqBody;
      }
    }

    simdjson::dom::element resp_el;
    op.responses = std::map<std::string, openapi::Response>{};
    openapi::Response r200;
    r200.description = "Successful response";
    if (method_obj["response"].get(resp_el) == simdjson::SUCCESS &&
        resp_el.is_object()) {
      simdjson::dom::element ref_el;
      if (resp_el.get_object()["$ref"].get(ref_el) == simdjson::SUCCESS &&
          ref_el.is_string()) {
        r200.content = std::map<std::string, openapi::MediaType>{};
        openapi::MediaType mt;
        openapi::Schema s;
        s.ref =
            openapi::Reference{.ref = "#/components/schemas/" +
                                      std::string(ref_el.get_string().value())};
        mt.schema = s;
        r200.content->insert({"application/json", mt});
      }
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

void process_resources(simdjson::dom::object resources,
                       std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto r : resources) {
    if (r.value.is_object()) {
      auto res_obj = r.value.get_object();
      simdjson::dom::element methods_el;
      if (res_obj["methods"].get(methods_el) == simdjson::SUCCESS &&
          methods_el.is_object()) {
        process_methods(methods_el.get_object(), paths);
      }
      simdjson::dom::element sub_res_el;
      if (res_obj["resources"].get(sub_res_el) == simdjson::SUCCESS &&
          sub_res_el.is_object()) {
        process_resources(sub_res_el.get_object(), paths);
      }
    }
  }
}

std::expected<std::vector<openapi::OpenAPI>, std::string> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  auto doc = parser.parse(input);
  auto root = doc.get_object();

  std::vector<openapi::OpenAPI> results;

  simdjson::dom::element kind_el;
  if (root["kind"].get(kind_el) == simdjson::SUCCESS && kind_el.is_string()) {
    std::string kind(kind_el.get_string().value());

    if (kind == "discovery#directoryList") {
      simdjson::dom::element items_el;
      if (root["items"].get(items_el) == simdjson::SUCCESS &&
          items_el.is_array()) {
        for (auto item : items_el.get_array()) {
          if (item.is_object()) {
            auto item_obj = item.get_object();
            openapi::OpenAPI spec;
            spec.openapi = "3.2.0";

            simdjson::dom::element title_el, version_el, desc_el;
            if (item_obj["title"].get(title_el) == simdjson::SUCCESS &&
                title_el.is_string())
              spec.info.title = std::string(title_el.get_string().value());
            if (item_obj["version"].get(version_el) == simdjson::SUCCESS &&
                version_el.is_string())
              spec.info.version = std::string(version_el.get_string().value());
            if (item_obj["description"].get(desc_el) == simdjson::SUCCESS &&
                desc_el.is_string())
              spec.info.description = std::string(desc_el.get_string().value());

            simdjson::dom::element url_el;
            if (item_obj["discoveryRestUrl"].get(url_el) == simdjson::SUCCESS &&
                url_el.is_string()) {
              openapi::Server srv;
              srv.url = std::string(url_el.get_string().value());
              spec.servers = std::vector<openapi::Server>{srv};
            }
            results.push_back(spec);
          }
        }
      }
    } else if (kind == "discovery#restDescription") {
      openapi::OpenAPI spec;
      spec.openapi = "3.2.0";

      simdjson::dom::element title_el, version_el, desc_el;
      if (root["title"].get(title_el) == simdjson::SUCCESS &&
          title_el.is_string())
        spec.info.title = std::string(title_el.get_string().value());
      if (root["version"].get(version_el) == simdjson::SUCCESS &&
          version_el.is_string())
        spec.info.version = std::string(version_el.get_string().value());
      if (root["description"].get(desc_el) == simdjson::SUCCESS &&
          desc_el.is_string())
        spec.info.description = std::string(desc_el.get_string().value());

      simdjson::dom::element root_url_el, service_path_el;
      std::string rootUrl = "";
      std::string servicePath = "";
      if (root["rootUrl"].get(root_url_el) == simdjson::SUCCESS &&
          root_url_el.is_string())
        rootUrl = std::string(root_url_el.get_string().value());
      if (root["servicePath"].get(service_path_el) == simdjson::SUCCESS &&
          service_path_el.is_string())
        servicePath = std::string(service_path_el.get_string().value());

      if (!rootUrl.empty()) {
        openapi::Server srv;
        srv.url = rootUrl + servicePath;
        spec.servers = std::vector<openapi::Server>{srv};
      }

      simdjson::dom::element schemas_el;
      if (root["schemas"].get(schemas_el) == simdjson::SUCCESS &&
          schemas_el.is_object()) {
        spec.components = openapi::Components{};
        spec.components->schemas = std::map<std::string, openapi::Schema>{};
        for (auto s : schemas_el.get_object()) {
          spec.components->schemas->insert(
              {std::string(s.key), convert_schema(s.value)});
        }
      }

      spec.paths = std::map<std::string, openapi::PathItem>{};

      simdjson::dom::element methods_el;
      if (root["methods"].get(methods_el) == simdjson::SUCCESS &&
          methods_el.is_object()) {
        process_methods(methods_el.get_object(), spec.paths.value());
      }

      simdjson::dom::element resources_el;
      if (root["resources"].get(resources_el) == simdjson::SUCCESS &&
          resources_el.is_object()) {
        process_resources(resources_el.get_object(), spec.paths.value());
      }

      results.push_back(spec);
    }
  }
  return results;
}
} // namespace cdd_cpp::google_discovery
