/// @file parse.cpp
/// @brief Implementation of Google Discovery JSON to OpenAPI 3.2.0 converter.

#include "parse.hpp"
#include <cstdlib>
#include <simdjson.h>

namespace cdd_cpp::google_discovery {

/// @brief Normalize RFC 6570 URI template operators (`{+var}` and `{/var}`)
/// into standard OpenAPI `{var}` path templates.
/// @param raw_path The raw Discovery path or flatPath string.
/// @return Normalized path string starting with `/` and using `{var}`
/// placeholders.
static std::string
normalize_discovery_path(std::string_view raw_path) noexcept {
  std::string out;
  out.reserve(raw_path.size() + 1);
  if (raw_path.empty() || raw_path.front() != '/') {
    out.push_back('/');
  }
  for (size_t i = 0; i < raw_path.size(); ++i) {
    if (raw_path[i] == '{' && i + 1 < raw_path.size() &&
        (raw_path[i + 1] == '+' || raw_path[i + 1] == '/')) {
      if (raw_path[i + 1] == '/' && out.back() != '/') {
        out.push_back('/');
      }
      out.push_back('{');
      ++i;
    } else {
      out.push_back(raw_path[i]);
    }
  }
  return out;
}

/// @brief Populate an OpenAPI Parameter's schema and metadata from a Discovery
/// parameter JSON object.
/// @param p_val The Discovery parameter JSON object.
/// @param p The target OpenAPI Parameter to populate.
static void populate_parameter_details(simdjson::dom::object p_val,
                                       openapi::Parameter &p) noexcept {
  std::string_view desc;
  if (p_val["description"].get(desc) == simdjson::SUCCESS) {
    p.description = std::string(desc);
  }
  bool req = false;
  if (p_val["required"].get(req) == simdjson::SUCCESS) {
    p.required = req;
  } else if (p.in == "path") {
    p.required = true;
  }
  bool dep = false;
  if (p_val["deprecated"].get(dep) == simdjson::SUCCESS) {
    p.deprecated = dep;
  }

  openapi::Schema item_s;
  std::string_view type;
  if (p_val["type"].get(type) == simdjson::SUCCESS && type != "any") {
    item_s.type = std::string(type);
  } else {
    item_s.type = "string";
  }
  std::string_view format;
  if (p_val["format"].get(format) == simdjson::SUCCESS) {
    item_s.format = std::string(format);
  }
  std::string_view pattern;
  if (p_val["pattern"].get(pattern) == simdjson::SUCCESS) {
    item_s.pattern = std::string(pattern);
  }
  std::string_view def_val;
  if (p_val["default"].get(def_val) == simdjson::SUCCESS) {
    item_s.default_value = std::string(def_val);
  }
  std::string_view min_str;
  if (p_val["minimum"].get(min_str) == simdjson::SUCCESS) {
    char *endptr = nullptr;
    std::string min_s(min_str);
    double v = std::strtod(min_s.c_str(), &endptr);
    if (endptr != min_s.c_str()) {
      item_s.minimum = v;
    }
  }
  std::string_view max_str;
  if (p_val["maximum"].get(max_str) == simdjson::SUCCESS) {
    char *endptr = nullptr;
    std::string max_s(max_str);
    double v = std::strtod(max_s.c_str(), &endptr);
    if (endptr != max_s.c_str()) {
      item_s.maximum = v;
    }
  }
  simdjson::dom::array enum_arr;
  if (p_val["enum"].get(enum_arr) == simdjson::SUCCESS) {
    std::vector<std::string> enums;
    for (auto e_el : enum_arr) {
      std::string_view ev;
      if (e_el.get(ev) == simdjson::SUCCESS) {
        enums.push_back(std::string(ev));
      }
    }
    if (!enums.empty()) {
      item_s.enum_values = std::move(enums);
    }
  }

  bool repeated = false;
  if (p_val["repeated"].get(repeated) == simdjson::SUCCESS && repeated) {
    openapi::Schema arr_s;
    arr_s.type = "array";
    arr_s.items = std::make_shared<openapi::Schema>(std::move(item_s));
    p.schema = std::move(arr_s);
    p.explode = true;
  } else {
    p.schema = std::move(item_s);
  }
}

/// @brief Convert Google Discovery JSON Schema object via `parse_schema`.
/// @param obj The simdjson object representing a Discovery schema.
/// @return Converted OpenAPI 3.2.0 Schema object.
static openapi::Schema parse_schema(simdjson::dom::object obj) noexcept {
  openapi::Schema s;
  std::string_view str_val;
  if (obj["type"].get(str_val) == simdjson::SUCCESS) {
    if (str_val != "any") {
      s.type = std::string(str_val);
    }
  }
  if (obj["description"].get(str_val) == simdjson::SUCCESS) {
    s.description = std::string(str_val);
  }
  if (obj["format"].get(str_val) == simdjson::SUCCESS) {
    s.format = std::string(str_val);
  }
  if (obj["pattern"].get(str_val) == simdjson::SUCCESS) {
    s.pattern = std::string(str_val);
  }
  if (obj["default"].get(str_val) == simdjson::SUCCESS) {
    s.default_value = std::string(str_val);
  }
  bool read_only = false;
  if (obj["readOnly"].get(read_only) == simdjson::SUCCESS) {
    s.readOnly = read_only;
  }
  bool dep = false;
  if (obj["deprecated"].get(dep) == simdjson::SUCCESS) {
    s.deprecated = dep;
  }
  if (obj["$ref"].get(str_val) == simdjson::SUCCESS) {
    openapi::Reference ref;
    ref.ref = "#/components/schemas/" + std::string(str_val);
    s.ref = ref;
  }

  simdjson::dom::array enum_arr;
  if (obj["enum"].get(enum_arr) == simdjson::SUCCESS) {
    std::vector<std::string> enums;
    for (auto e_el : enum_arr) {
      std::string_view ev;
      if (e_el.get(ev) == simdjson::SUCCESS) {
        enums.push_back(std::string(ev));
      }
    }
    if (!enums.empty()) {
      s.enum_values = std::move(enums);
    }
  }

  simdjson::dom::object props;
  if (obj["properties"].get(props) == simdjson::SUCCESS) {
    s.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
    std::vector<std::string> req_fields;
    for (auto field : props) {
      simdjson::dom::object prop_obj;
      if (field.value.get(prop_obj) == simdjson::SUCCESS) {
        s.properties->operator[](std::string(field.key)) =
            parse_schema(prop_obj);
        bool prop_req = false;
        if (prop_obj["required"].get(prop_req) == simdjson::SUCCESS &&
            prop_req) {
          req_fields.push_back(std::string(field.key));
        }
      }
    }
    if (!req_fields.empty()) {
      s.required = std::move(req_fields);
    }
  }

  simdjson::dom::object add_props;
  if (obj["additionalProperties"].get(add_props) == simdjson::SUCCESS) {
    s.additionalProperties =
        std::make_shared<openapi::Schema>(parse_schema(add_props));
  }

  simdjson::dom::object items;
  if (obj["items"].get(items) == simdjson::SUCCESS) {
    s.items = std::make_shared<openapi::Schema>(parse_schema(items));
  }

  simdjson::dom::object variant_obj;
  if (obj["variant"].get(variant_obj) == simdjson::SUCCESS) {
    openapi::Discriminator disc;
    std::string_view disc_prop;
    if (variant_obj["discriminant"].get(disc_prop) == simdjson::SUCCESS) {
      disc.propertyName = std::string(disc_prop);
    }
    simdjson::dom::array map_arr;
    if (variant_obj["map"].get(map_arr) == simdjson::SUCCESS) {
      std::map<std::string, std::string> mapping;
      s.oneOf = std::make_shared<std::vector<openapi::Schema>>();
      for (auto m_el : map_arr) {
        simdjson::dom::object m_obj;
        if (m_el.get(m_obj) == simdjson::SUCCESS) {
          std::string_view ref_v;
          if (m_obj["$ref"].get(ref_v) == simdjson::SUCCESS) {
            std::string full_ref = "#/components/schemas/" + std::string(ref_v);
            openapi::Schema ref_s;
            ref_s.ref = openapi::Reference{full_ref};
            s.oneOf->push_back(std::move(ref_s));
            std::string_view type_val;
            if (m_obj["type_value"].get(type_val) == simdjson::SUCCESS) {
              mapping[std::string(type_val)] = full_ref;
            }
          }
        }
      }
      if (!mapping.empty()) {
        disc.mapping = std::move(mapping);
      }
    }
    if (!disc.propertyName.empty()) {
      s.discriminator = std::move(disc);
    }
  }
  return s;
}

/// @brief Convert Google Discovery JSON to OpenAPI 3.2.0 via `process_methods`.
/// @param methods_obj The simdjson object containing Discovery methods.
/// @param paths The target OpenAPI Paths map to populate.
static void
process_methods(simdjson::dom::object methods_obj,
                std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto field : methods_obj) {
    simdjson::dom::object method_obj;
    if (field.value.get(method_obj) != simdjson::SUCCESS)
      continue;

    std::string_view path_str;
    if (method_obj["flatPath"].get(path_str) != simdjson::SUCCESS &&
        method_obj["path"].get(path_str) != simdjson::SUCCESS)
      continue;

    std::string full_path = normalize_discovery_path(path_str);

    std::string_view http_method;
    if (method_obj["httpMethod"].get(http_method) != simdjson::SUCCESS)
      continue;

    openapi::Operation op;
    std::string_view id_str;
    if (method_obj["id"].get(id_str) == simdjson::SUCCESS) {
      op.operationId = std::string(id_str);
    } else {
      op.operationId = std::string(field.key);
    }

    std::string_view desc_str;
    if (method_obj["description"].get(desc_str) == simdjson::SUCCESS) {
      op.description = std::string(desc_str);
    }

    bool dep = false;
    if (method_obj["deprecated"].get(dep) == simdjson::SUCCESS) {
      op.deprecated = dep;
    }

    bool supports_sub = false;
    if (method_obj["supportsSubscription"].get(supports_sub) ==
        simdjson::SUCCESS) {
      op.extensions["x-supports-subscription"] =
          openapi::JsonValue(supports_sub);
    }

    simdjson::dom::object params_obj;
    if (method_obj["parameters"].get(params_obj) == simdjson::SUCCESS) {
      op.parameters = std::vector<openapi::Parameter>{};
      for (auto p_field : params_obj) {
        simdjson::dom::object p_val;
        if (p_field.value.get(p_val) != simdjson::SUCCESS)
          continue;

        openapi::Parameter p;
        p.name = std::string(p_field.key);

        std::string_view loc;
        if (p_val["location"].get(loc) == simdjson::SUCCESS) {
          p.in = std::string(loc); // "query", "path", or "header"
        } else {
          p.in = "query";
        }

        populate_parameter_details(p_val, p);
        op.parameters->push_back(p);
      }
    }

    simdjson::dom::object req_obj;
    bool has_req_ref = false;
    std::string req_schema_ref;
    if (method_obj["request"].get(req_obj) == simdjson::SUCCESS) {
      std::string_view ref_str;
      if (req_obj["$ref"].get(ref_str) == simdjson::SUCCESS) {
        has_req_ref = true;
        req_schema_ref = "#/components/schemas/" + std::string(ref_str);
        openapi::RequestBody rb;
        openapi::MediaType mt;
        openapi::Schema s;
        openapi::Reference ref;
        ref.ref = req_schema_ref;
        s.ref = ref;
        mt.schema = s;
        rb.content["application/json"] = mt;
        op.requestBody = rb;
      }
    }

    bool supports_media_upload = false;
    (void)method_obj["supportsMediaUpload"].get(supports_media_upload);
    simdjson::dom::object media_upload_obj;
    bool has_media_upload =
        (method_obj["mediaUpload"].get(media_upload_obj) == simdjson::SUCCESS);
    if (has_media_upload || supports_media_upload) {
      if (!op.requestBody.has_value()) {
        op.requestBody = openapi::RequestBody{};
      }
      openapi::MediaType bin_mt;
      openapi::Schema bin_s;
      bin_s.type = "string";
      bin_s.format = "binary";
      bin_mt.schema = bin_s;
      op.requestBody->content["application/octet-stream"] = bin_mt;

      openapi::MediaType multi_mt;
      openapi::Schema multi_s;
      multi_s.type = "object";
      multi_s.properties =
          std::make_shared<std::map<std::string, openapi::Schema>>();
      if (has_req_ref) {
        openapi::Schema meta_s;
        meta_s.ref = openapi::Reference{req_schema_ref};
        (*multi_s.properties)["metadata"] = meta_s;
      }
      (*multi_s.properties)["media"] = bin_s;
      multi_mt.schema = multi_s;
      op.requestBody->content["multipart/related"] = multi_mt;

      simdjson::dom::array accept_arr;
      if (has_media_upload &&
          media_upload_obj["accept"].get(accept_arr) == simdjson::SUCCESS) {
        for (auto acc_el : accept_arr) {
          std::string_view mime_sv;
          if (acc_el.get(mime_sv) == simdjson::SUCCESS) {
            op.requestBody->content[std::string(mime_sv)] = bin_mt;
          }
        }
      }
    }

    op.responses = std::map<std::string, openapi::Response>{};
    openapi::Response resp200;
    resp200.description = "Successful operation";
    simdjson::dom::object resp_obj;
    if (method_obj["response"].get(resp_obj) == simdjson::SUCCESS) {
      std::string_view ref_str;
      if (resp_obj["$ref"].get(ref_str) == simdjson::SUCCESS) {
        resp200.content = std::map<std::string, openapi::MediaType>{};
        openapi::MediaType mt;
        openapi::Schema s;
        openapi::Reference ref;
        ref.ref = "#/components/schemas/" + std::string(ref_str);
        s.ref = ref;
        mt.schema = s;
        resp200.content->operator[]("application/json") = mt;
      }
    }
    bool supports_media_download = false;
    if (method_obj["supportsMediaDownload"].get(supports_media_download) ==
            simdjson::SUCCESS &&
        supports_media_download) {
      if (!resp200.content.has_value()) {
        resp200.content = std::map<std::string, openapi::MediaType>{};
      }
      openapi::MediaType dl_mt;
      openapi::Schema dl_s;
      dl_s.type = "string";
      dl_s.format = "binary";
      dl_mt.schema = dl_s;
      (*resp200.content)["application/octet-stream"] = dl_mt;
    }
    op.responses->operator[]("200") = resp200;

    simdjson::dom::array scopes_arr;
    if (method_obj["scopes"].get(scopes_arr) == simdjson::SUCCESS) {
      std::vector<std::string> scopes;
      for (auto s_el : scopes_arr) {
        std::string_view sv;
        if (s_el.get(sv) == simdjson::SUCCESS) {
          scopes.push_back(std::string(sv));
        }
      }
      if (!scopes.empty()) {
        openapi::SecurityRequirement sr;
        sr["Oauth2"] = scopes;
        op.security = std::vector<openapi::SecurityRequirement>{sr};
      }
    }

    if (http_method == "GET")
      paths[full_path].get = op;
    else if (http_method == "POST")
      paths[full_path].post = op;
    else if (http_method == "PUT")
      paths[full_path].put = op;
    else if (http_method == "DELETE")
      paths[full_path].delete_op = op;
    else if (http_method == "PATCH")
      paths[full_path].patch = op;
    else if (http_method == "HEAD")
      paths[full_path].head = op;
    else if (http_method == "OPTIONS")
      paths[full_path].options = op;
  }
}

/// @brief Convert Google Discovery JSON to OpenAPI 3.2.0 via
/// `process_resources`.
/// @param resources_obj The simdjson object containing nested Discovery
/// resources.
/// @param paths The target OpenAPI Paths map to populate.
static void
process_resources(simdjson::dom::object resources_obj,
                  std::map<std::string, openapi::PathItem> &paths) noexcept {
  for (auto field : resources_obj) {
    simdjson::dom::object res_obj;
    if (field.value.get(res_obj) != simdjson::SUCCESS)
      continue;

    simdjson::dom::object methods_obj;
    if (res_obj["methods"].get(methods_obj) == simdjson::SUCCESS) {
      process_methods(methods_obj, paths);
    }

    simdjson::dom::object sub_resources_obj;
    if (res_obj["resources"].get(sub_resources_obj) == simdjson::SUCCESS) {
      process_resources(sub_resources_obj, paths);
    }
  }
}

/// @brief Parse a single Google Discovery `rest` description object into an
/// OpenAPI 3.2.0 document.
/// @param root The simdjson object representing a Discovery REST description.
/// @return Populated OpenAPI 3.2.0 model.
static openapi::OpenAPI
parse_rest_document(simdjson::dom::object root) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";

  std::string_view str_val;
  if (root["title"].get(str_val) == simdjson::SUCCESS) {
    spec.info.title = std::string(str_val);
  } else if (root["name"].get(str_val) == simdjson::SUCCESS) {
    spec.info.title = std::string(str_val);
  } else {
    spec.info.title = "Google Discovery API";
  }

  if (root["version"].get(str_val) == simdjson::SUCCESS) {
    spec.info.version = std::string(str_val);
  } else {
    spec.info.version = "v1";
  }

  if (root["description"].get(str_val) == simdjson::SUCCESS) {
    spec.info.description = std::string(str_val);
  }

  if (root["documentationLink"].get(str_val) == simdjson::SUCCESS) {
    openapi::ExternalDocumentation ext;
    ext.url = std::string(str_val);
    spec.externalDocs = ext;
  }

  std::string_view root_url_sv;
  std::string_view service_path_sv;
  if (root["rootUrl"].get(root_url_sv) == simdjson::SUCCESS) {
    std::string server_url(root_url_sv);
    if (root["servicePath"].get(service_path_sv) == simdjson::SUCCESS) {
      if (server_url.ends_with('/') && service_path_sv.starts_with('/')) {
        server_url += std::string(service_path_sv.substr(1));
      } else {
        server_url += std::string(service_path_sv);
      }
    }
    openapi::Server s;
    s.url = std::move(server_url);
    spec.servers = std::vector<openapi::Server>{s};
  } else if (root["baseUrl"].get(str_val) == simdjson::SUCCESS) {
    openapi::Server s;
    s.url = std::string(str_val);
    spec.servers = std::vector<openapi::Server>{s};
  }

  // Global parameters -> components.parameters
  simdjson::dom::object global_params;
  if (root["parameters"].get(global_params) == simdjson::SUCCESS) {
    spec.components = openapi::Components{};
    spec.components->parameters = std::map<std::string, openapi::Parameter>{};
    for (auto p_field : global_params) {
      simdjson::dom::object p_val;
      if (p_field.value.get(p_val) != simdjson::SUCCESS)
        continue;
      openapi::Parameter p;
      p.name = std::string(p_field.key);
      std::string_view loc;
      if (p_val["location"].get(loc) == simdjson::SUCCESS) {
        p.in = std::string(loc);
      } else {
        p.in = "query";
      }
      populate_parameter_details(p_val, p);
      spec.components->parameters->operator[](p.name) = p;
    }
  }

  // Auth -> components.securitySchemes
  simdjson::dom::object auth_obj;
  if (root["auth"].get(auth_obj) == simdjson::SUCCESS) {
    simdjson::dom::object oauth2_obj;
    if (auth_obj["oauth2"].get(oauth2_obj) == simdjson::SUCCESS) {
      simdjson::dom::object scopes_obj;
      if (oauth2_obj["scopes"].get(scopes_obj) == simdjson::SUCCESS) {
        if (!spec.components.has_value()) {
          spec.components = openapi::Components{};
        }
        spec.components->securitySchemes =
            std::map<std::string, openapi::SecurityScheme>{};
        openapi::SecurityScheme sec;
        sec.type = "oauth2";
        openapi::OAuthFlows flows;
        openapi::OAuthFlow implicit_flow;
        implicit_flow.authorizationUrl =
            "https://accounts.google.com/o/oauth2/auth";
        openapi::OAuthFlow auth_code_flow;
        auth_code_flow.authorizationUrl =
            "https://accounts.google.com/o/oauth2/auth";
        auth_code_flow.tokenUrl = "https://oauth2.googleapis.com/token";
        std::map<std::string, std::string> scope_map;
        for (auto sc_field : scopes_obj) {
          std::string sc_desc;
          simdjson::dom::object sc_val;
          std::string_view desc_sv;
          if (sc_field.value.get(sc_val) == simdjson::SUCCESS &&
              sc_val["description"].get(desc_sv) == simdjson::SUCCESS) {
            sc_desc = std::string(desc_sv);
          }
          scope_map[std::string(sc_field.key)] = sc_desc;
        }
        implicit_flow.scopes = scope_map;
        auth_code_flow.scopes = scope_map;
        flows.implicit = implicit_flow;
        flows.authorizationCode = auth_code_flow;
        sec.flows = flows;
        (*spec.components->securitySchemes)["Oauth2"] = sec;
      }
    }
  }

  simdjson::dom::object schemas_obj;
  if (root["schemas"].get(schemas_obj) == simdjson::SUCCESS) {
    if (!spec.components.has_value()) {
      spec.components = openapi::Components{};
    }
    spec.components->schemas = std::map<std::string, openapi::Schema>{};
    for (auto field : schemas_obj) {
      simdjson::dom::object s_obj;
      if (field.value.get(s_obj) == simdjson::SUCCESS) {
        spec.components->schemas->operator[](std::string(field.key)) =
            parse_schema(s_obj);
      }
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

  return spec;
}

/// @brief Convert Google Discovery JSON to OpenAPI 3.2.0 via `parse`.
Result<std::vector<openapi::OpenAPI>> parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(input).get(doc);
  if (error) {
    return std::unexpected(simdjson::error_message(error));
  }

  std::vector<openapi::OpenAPI> results;
  simdjson::dom::object root;
  if (doc.get(root) != simdjson::SUCCESS) {
    return std::unexpected("Root is not an object");
  }

  std::string_view kind;
  if (root["kind"].get(kind) == simdjson::SUCCESS) {
    if (kind == "discovery#directoryList") {
      simdjson::dom::array items;
      if (root["items"].get(items) == simdjson::SUCCESS) {
        for (auto item_el : items) {
          simdjson::dom::object item;
          if (item_el.get(item) != simdjson::SUCCESS)
            continue;

          openapi::OpenAPI spec;
          spec.openapi = "3.2.0";

          std::string_view str_val;
          if (item["title"].get(str_val) == simdjson::SUCCESS) {
            spec.info.title = std::string(str_val);
          } else if (item["name"].get(str_val) == simdjson::SUCCESS) {
            spec.info.title = std::string(str_val);
          } else {
            spec.info.title = "Google Discovery API";
          }

          if (item["version"].get(str_val) == simdjson::SUCCESS) {
            spec.info.version = std::string(str_val);
          } else {
            spec.info.version = "v1";
          }

          if (item["description"].get(str_val) == simdjson::SUCCESS) {
            spec.info.description = std::string(str_val);
          }

          if (item["discoveryRestUrl"].get(str_val) == simdjson::SUCCESS) {
            openapi::Server s;
            s.url = std::string(str_val);
            spec.servers = std::vector<openapi::Server>{s};
          }

          if (item["documentationLink"].get(str_val) == simdjson::SUCCESS) {
            openapi::ExternalDocumentation ext;
            ext.url = std::string(str_val);
            spec.externalDocs = ext;
          }

          results.push_back(spec);
        }
      }
    } else if (kind == "discovery#restDescription") {
      results.push_back(parse_rest_document(root));
    }
  } else {
    simdjson::dom::element dummy;
    if (root["schemas"].get(dummy) == simdjson::SUCCESS ||
        root["resources"].get(dummy) == simdjson::SUCCESS ||
        root["methods"].get(dummy) == simdjson::SUCCESS) {
      results.push_back(parse_rest_document(root));
    }
  }
  return results;
}
} // namespace cdd_cpp::google_discovery
