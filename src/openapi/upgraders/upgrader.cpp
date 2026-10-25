/// @file upgrader.cpp
/// @brief Implementation of Swagger 1.2, Swagger 2.0, and OpenAPI 3.0/3.1
/// upgraders to OpenAPI 3.2.0.

#include "upgrader.hpp"
#include "../../utils/json_writer.hpp"
#include "../emit.hpp"
#include "../parse.hpp"
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace cdd_cpp::openapi::upgraders {

/// @brief Swagger 2.0 parameter keys that belong inside the OpenAPI 3.x
/// parameter `schema` object.
static const std::set<std::string_view> kSwaggerParamSchemaKeys{
    "type",      "format",           "items",   "default",
    "maximum",   "exclusiveMaximum", "minimum", "exclusiveMinimum",
    "maxLength", "minLength",        "pattern", "maxItems",
    "minItems",  "uniqueItems",      "enum",    "multipleOf"};

/// @brief Top-level Swagger 2.0 keys copied verbatim to OpenAPI 3.2.0.
static const std::set<std::string_view> kTopLevelCopyKeys{
    "tags", "externalDocs", "security"};

/// @brief Swagger 1.x version strings upgraded via `upgrade_swagger_1_2`.
static const std::set<std::string_view> kSwagger1Versions{"1.2", "1.1", "1.0"};

/// @brief Unwrap a `simdjson::dom::object` from an element without throwing.
/// @param el The simdjson DOM element.
/// @return Unwrapped `simdjson::dom::object`.
static simdjson::dom::object unwrap_obj(simdjson::dom::element el) noexcept {
  simdjson::dom::object o;
  [[maybe_unused]] auto err = el.get(o);
  return o;
}

/// @brief Unwrap a `simdjson::dom::array` from an element without throwing.
/// @param el The simdjson DOM element.
/// @return Unwrapped `simdjson::dom::array`.
static simdjson::dom::array unwrap_arr(simdjson::dom::element el) noexcept {
  simdjson::dom::array a;
  [[maybe_unused]] auto err = el.get(a);
  return a;
}

/// @brief Unwrap a `std::string` from an element without throwing.
/// @param el The simdjson DOM element.
/// @return Unwrapped `std::string`.
static std::string unwrap_str(simdjson::dom::element el) noexcept {
  std::string_view sv;
  [[maybe_unused]] auto err = el.get(sv);
  return std::string(sv);
}

/// @brief Extract a numeric bound (`int64`, `uint64`, or `double`) from a
/// schema object.
/// @param schema The schema object.
/// @param key The field name ("minimum" or "maximum").
/// @return Extracted double value, or 0.0 if absent.
static double extract_numeric_bound(simdjson::dom::object schema,
                                    std::string_view key) noexcept {
  simdjson::dom::element el;
  if (schema[key].get(el) == simdjson::SUCCESS) {
    int64_t i_val = 0;
    if (el.get(i_val) == simdjson::SUCCESS)
      return static_cast<double>(i_val);
    uint64_t u_val = 0;
    if (el.get(u_val) == simdjson::SUCCESS)
      return static_cast<double>(u_val);
    double d_val = 0.0;
    [[maybe_unused]] auto err = el.get(d_val);
    return d_val;
  }
  return 0.0;
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `process_schema_3_0`.
void process_schema_3_0(simdjson::dom::object schema,
                        utils::JsonWriter &jw) noexcept;
/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `process_element_3_0`.
void process_element_3_0(simdjson::dom::element el,
                         utils::JsonWriter &jw) noexcept;

/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `process_schema_3_0`.
void process_schema_3_0(simdjson::dom::object schema,
                        utils::JsonWriter &jw) noexcept {
  jw.start_object();
  double min_val = extract_numeric_bound(schema, "minimum");
  double max_val = extract_numeric_bound(schema, "maximum");
  bool exclusive_min = false;
  bool exclusive_max = false;
  [[maybe_unused]] auto err_min = schema["exclusiveMinimum"].get(exclusive_min);
  [[maybe_unused]] auto err_max = schema["exclusiveMaximum"].get(exclusive_max);

  for (auto field : schema) {
    std::string key(field.key);
    if (key == "minimum" && exclusive_min)
      continue;
    if (key == "maximum" && exclusive_max)
      continue;
    if (key == "exclusiveMinimum") {
      jw.key("exclusiveMinimum");
      jw.raw_value(std::to_string(min_val));
      continue;
    }
    if (key == "exclusiveMaximum") {
      jw.key("exclusiveMaximum");
      jw.raw_value(std::to_string(max_val));
      continue;
    }

    if (key == "items" && field.value.is_array()) {
      jw.key("prefixItems");
      process_element_3_0(field.value, jw);
      continue;
    }

    jw.key(key);
    process_element_3_0(field.value, jw);
  }
  jw.end_object();
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `process_element_3_0`.
void process_element_3_0(simdjson::dom::element el,
                         utils::JsonWriter &jw) noexcept {
  if (el.is_array()) {
    jw.start_array();
    for (auto item : unwrap_arr(el)) {
      process_element_3_0(item, jw);
    }
    jw.end_array();
    return;
  }
  if (el.is_object()) {
    auto obj = unwrap_obj(el);
    std::string_view type_sv;
    simdjson::dom::element prop_el;
    bool is_schema = (obj["type"].get(type_sv) == simdjson::SUCCESS) ||
                     (obj["properties"].get(prop_el) == simdjson::SUCCESS);

    if (is_schema) {
      process_schema_3_0(obj, jw);
    } else {
      jw.start_object();
      for (auto field : obj) {
        jw.key(std::string(field.key));
        process_element_3_0(field.value, jw);
      }
      jw.end_object();
    }
    return;
  }
  jw.raw_value(simdjson::minify(el));
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via `upgrade_openapi_3`.
std::string upgrade_openapi_3(simdjson::dom::object root,
                              std::string_view current_version) noexcept {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  for (auto field : root) {
    std::string key(field.key);
    if (key == "openapi")
      continue;

    jw.key(key);
    if (current_version.starts_with("3.0")) {
      process_element_3_0(field.value, jw);
    } else {
      jw.raw_value(simdjson::minify(field.value));
    }
  }

  jw.end_object();
  return jw.str();
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `upgrade_swagger_1_2`.
std::string upgrade_swagger_1_2(simdjson::dom::object root) noexcept {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  jw.key("info");
  jw.start_object();

  simdjson::dom::object info;
  if (root["info"].get(info) == simdjson::SUCCESS) {
    std::string_view title_sv, desc_sv;
    if (info["title"].get(title_sv) == simdjson::SUCCESS) {
      jw.key_value("title", std::string(title_sv));
    } else {
      jw.key_value("title", "Upgraded from 1.2");
    }

    if (info["description"].get(desc_sv) == simdjson::SUCCESS) {
      jw.key_value("description", std::string(desc_sv));
    }
  } else {
    jw.key_value("title", "Upgraded from 1.2");
  }

  jw.key_value("version", "1.0.0");
  jw.end_object(); // info

  jw.key("paths");
  jw.start_object();

  simdjson::dom::array apis_arr;
  if (root["apis"].get(apis_arr) == simdjson::SUCCESS) {
    for (auto api : apis_arr) {
      auto api_obj = unwrap_obj(api);
      simdjson::dom::element path_el;
      [[maybe_unused]] auto err_path = api_obj["path"].get(path_el);
      jw.key(unwrap_str(path_el));
      jw.start_object();

      simdjson::dom::array ops_arr;
      if (api_obj["operations"].get(ops_arr) == simdjson::SUCCESS) {
        for (auto op : ops_arr) {
          auto op_obj = unwrap_obj(op);
          simdjson::dom::element m_el;
          [[maybe_unused]] auto err_m = op_obj["method"].get(m_el);
          std::string method_str = unwrap_str(m_el);
          for (auto &c : method_str)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
          jw.key(method_str);
          jw.start_object();

          std::string_view nickname_sv;
          if (op_obj["nickname"].get(nickname_sv) == simdjson::SUCCESS) {
            jw.key_value("operationId", std::string(nickname_sv));
          }

          jw.key("responses");
          jw.start_object();
          jw.key("200");
          jw.start_object();
          jw.key_value("description", "Success");
          jw.end_object();
          jw.end_object();

          jw.end_object(); // op
        }
      }

      jw.end_object(); // path
    }
  }

  jw.end_object(); // paths

  jw.end_object();
  return jw.str();
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via
/// `upgrade_swagger_2_0`.
std::string upgrade_swagger_2_0(simdjson::dom::object root) noexcept {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  simdjson::dom::element info_el;
  if (root["info"].get(info_el) == simdjson::SUCCESS) {
    jw.key("info");
    jw.raw_value(simdjson::minify(info_el));
  }

  // Copy top-level properties
  for (auto field : root) {
    std::string key(field.key);
    if (kTopLevelCopyKeys.contains(key) || key.starts_with("x-")) {
      jw.key(key);
      jw.raw_value(simdjson::minify(field.value));
    }
  }

  std::string host = "";
  std::string basePath = "";
  simdjson::dom::element host_el, bp_el;
  if (root["host"].get(host_el) == simdjson::SUCCESS)
    host = unwrap_str(host_el);
  if (root["basePath"].get(bp_el) == simdjson::SUCCESS)
    basePath = unwrap_str(bp_el);

  std::vector<std::string> schemes;
  simdjson::dom::array schemes_arr;
  if (root["schemes"].get(schemes_arr) == simdjson::SUCCESS) {
    for (auto s : schemes_arr) {
      schemes.push_back(unwrap_str(s));
    }
  }
  if (schemes.empty() && host != "")
    schemes.push_back("https");

  if (!host.empty()) {
    jw.key("servers");
    jw.start_array();
    for (const auto &s : schemes) {
      jw.start_object();
      jw.key_value("url", s + "://" + host + basePath);
      jw.end_object();
    }
    jw.end_array();
  }

  std::vector<std::string> global_consumes;
  std::vector<std::string> global_produces;
  simdjson::dom::array g_cons_arr, g_prod_arr;
  if (root["consumes"].get(g_cons_arr) == simdjson::SUCCESS) {
    for (auto c : g_cons_arr)
      global_consumes.push_back(unwrap_str(c));
  }
  if (root["produces"].get(g_prod_arr) == simdjson::SUCCESS) {
    for (auto p : g_prod_arr)
      global_produces.push_back(unwrap_str(p));
  }
  if (global_consumes.empty())
    global_consumes.push_back("application/json");
  if (global_produces.empty())
    global_produces.push_back("application/json");

  // Helper lambda to process parameters
  auto process_parameters = [&](simdjson::dom::array p_arr) {
    std::string body_param_raw = "";
    std::vector<simdjson::dom::object> form_params;
    jw.key("parameters");
    jw.start_array();
    for (auto p : p_arr) {
      auto p_obj = unwrap_obj(p);
      simdjson::dom::element in_el;
      if (p_obj["in"].get(in_el) == simdjson::SUCCESS) {
        std::string in_val = unwrap_str(in_el);
        if (in_val == "body") {
          simdjson::dom::element sch_el;
          [[maybe_unused]] auto err_sch = p_obj["schema"].get(sch_el);
          body_param_raw = simdjson::minify(sch_el);
        } else if (in_val == "formData") {
          form_params.push_back(p_obj);
        } else {
          jw.start_object();
          for (auto pp : p_obj) {
            std::string pk(pp.key);
            if (kSwaggerParamSchemaKeys.contains(pk) ||
                pk == "collectionFormat")
              continue; // Move to schema
            jw.key(pk);
            jw.raw_value(simdjson::minify(pp.value));
          }
          jw.key("schema");
          jw.start_object();
          for (auto pp : p_obj) {
            std::string pk(pp.key);
            if (kSwaggerParamSchemaKeys.contains(pk)) {
              jw.key(pk);
              jw.raw_value(simdjson::minify(pp.value));
            }
          }
          jw.end_object();
          jw.end_object();
        }
      } else {
        jw.raw_value(simdjson::minify(p)); // $ref or other
      }
    }
    jw.end_array();
    return std::make_pair(body_param_raw, form_params);
  };

  simdjson::dom::object paths_obj;
  if (root["paths"].get(paths_obj) == simdjson::SUCCESS) {
    jw.key("paths");
    jw.start_object();

    for (auto path_field : paths_obj) {
      jw.key(std::string(path_field.key));
      jw.start_object(); // path item
      auto pi_obj = unwrap_obj(path_field.value);
      for (auto op_field : pi_obj) {
        std::string op_method(op_field.key);
        if (op_method == "parameters") {
          (void)process_parameters(unwrap_arr(op_field.value));
          continue;
        }
        if (op_method.starts_with("x-") || op_method == "$ref") {
          jw.key(op_method);
          jw.raw_value(simdjson::minify(op_field.value));
          continue;
        }

        jw.key(op_method);
        jw.start_object(); // operation
        auto op_obj = unwrap_obj(op_field.value);

        std::vector<std::string> local_consumes = global_consumes;
        std::vector<std::string> local_produces = global_produces;

        simdjson::dom::array l_cons_arr, l_prod_arr;
        if (op_obj["consumes"].get(l_cons_arr) == simdjson::SUCCESS) {
          local_consumes.clear();
          for (auto c : l_cons_arr)
            local_consumes.push_back(unwrap_str(c));
        }
        if (op_obj["produces"].get(l_prod_arr) == simdjson::SUCCESS) {
          local_produces.clear();
          for (auto p : l_prod_arr)
            local_produces.push_back(unwrap_str(p));
        }

        std::string body_param_raw = "";
        std::vector<simdjson::dom::object> form_params;

        for (auto op_prop : op_obj) {
          std::string k(op_prop.key);
          if (k == "consumes" || k == "produces")
            continue;
          if (k == "parameters") {
            auto p_res = process_parameters(unwrap_arr(op_prop.value));
            body_param_raw = p_res.first;
            form_params = p_res.second;
            continue;
          }

          if (k == "responses") {
            jw.key("responses");
            jw.start_object();
            for (auto r : unwrap_obj(op_prop.value)) {
              jw.key(std::string(r.key));
              jw.start_object();
              auto r_obj = unwrap_obj(r.value);
              for (auto rp : r_obj) {
                std::string rk(rp.key);
                if (rk == "schema") {
                  jw.key("content");
                  jw.start_object();
                  for (const auto &prod : local_produces) {
                    jw.key(prod);
                    jw.start_object();
                    jw.key("schema");
                    jw.raw_value(simdjson::minify(rp.value));
                    jw.end_object();
                  }
                  jw.end_object();
                } else {
                  jw.key(rk);
                  jw.raw_value(simdjson::minify(rp.value));
                }
              }
              jw.end_object();
            }
            jw.end_object();
            continue;
          }

          jw.key(k);
          jw.raw_value(simdjson::minify(op_prop.value));
        }

        // Construct requestBody
        if (!body_param_raw.empty() || !form_params.empty()) {
          jw.key("requestBody");
          jw.start_object();
          jw.key("content");
          jw.start_object();

          if (!body_param_raw.empty()) {
            for (const auto &cons : local_consumes) {
              jw.key(cons);
              jw.start_object();
              jw.key("schema");
              jw.raw_value(body_param_raw);
              jw.end_object();
            }
          } else {
            // Generate an object schema from form params
            std::string cons = "application/x-www-form-urlencoded";
            for (const auto &c : local_consumes)
              if (c == "multipart/form-data")
                cons = c;

            jw.key(cons);
            jw.start_object();
            jw.key("schema");
            jw.start_object();
            jw.key_value("type", "object");
            jw.key("properties");
            jw.start_object();

            for (const auto &obj : form_params) {
              simdjson::dom::element n_el;
              [[maybe_unused]] auto err_n = obj["name"].get(n_el);
              jw.key(unwrap_str(n_el));
              jw.start_object();
              for (auto field : obj) {
                std::string k(field.key);
                if (k == "name" || k == "in" || k == "required")
                  continue;
                jw.key(k);
                jw.raw_value(simdjson::minify(field.value));
              }
              jw.end_object();
            }

            jw.end_object(); // properties
            jw.end_object(); // schema
            jw.end_object(); // cons
          }

          jw.end_object(); // content
          jw.end_object(); // requestBody
        }

        jw.end_object(); // operation
      }
      jw.end_object(); // path item
    }
    jw.end_object(); // paths
  }

  jw.key("components");
  jw.start_object();

  simdjson::dom::element defs_el;
  if (root["definitions"].get(defs_el) == simdjson::SUCCESS) {
    jw.key("schemas");
    jw.raw_value(simdjson::minify(defs_el)); // Simplified: direct copy
  }

  simdjson::dom::object p_map;
  if (root["parameters"].get(p_map) == simdjson::SUCCESS) {
    jw.key("parameters");
    jw.start_object();
    for (auto p : p_map) {
      jw.key(std::string(p.key));
      jw.start_object();
      auto p_obj = unwrap_obj(p.value);
      jw.key("schema");
      jw.start_object();
      for (auto pp : p_obj) {
        std::string pk(pp.key);
        if (kSwaggerParamSchemaKeys.contains(pk)) {
          jw.key(pk);
          jw.raw_value(simdjson::minify(pp.value));
        }
      }
      jw.end_object();
      jw.end_object();
    }
    jw.end_object();
  }

  simdjson::dom::object r_map;
  if (root["responses"].get(r_map) == simdjson::SUCCESS) {
    jw.key("responses");
    jw.start_object();
    for (auto r : r_map) {
      jw.key(std::string(r.key));
      jw.start_object();
      auto r_obj = unwrap_obj(r.value);
      for (auto rp : r_obj) {
        std::string rk(rp.key);
        if (rk == "schema") {
          jw.key("content");
          jw.start_object();
          for (const auto &prod : global_produces) {
            jw.key(prod);
            jw.start_object();
            jw.key("schema");
            jw.raw_value(simdjson::minify(rp.value));
            jw.end_object();
          }
          jw.end_object();
        } else {
          jw.key(rk);
          jw.raw_value(simdjson::minify(rp.value));
        }
      }
      jw.end_object();
    }
    jw.end_object();
  }

  simdjson::dom::object sec_map;
  if (root["securityDefinitions"].get(sec_map) == simdjson::SUCCESS) {
    jw.key("securitySchemes");
    jw.start_object();
    for (auto sf : sec_map) {
      jw.key(std::string(sf.key));
      jw.start_object();
      auto sfo = unwrap_obj(sf.value);
      simdjson::dom::element t_el;
      [[maybe_unused]] auto err_t = sfo["type"].get(t_el);
      std::string type_val = unwrap_str(t_el);

      if (type_val == "basic") {
        jw.key_value("type", "http");
        jw.key_value("scheme", "basic");
      } else if (type_val == "oauth2") {
        jw.key_value("type", "oauth2");
        jw.key("flows");
        jw.start_object();

        simdjson::dom::element f_el;
        [[maybe_unused]] auto err_f = sfo["flow"].get(f_el);
        std::string flow_val = unwrap_str(f_el);
        std::string nflow = flow_val;
        if (nflow == "accessCode")
          nflow = "authorizationCode";
        if (nflow == "application")
          nflow = "clientCredentials";

        jw.key(nflow);
        jw.start_object();
        for (auto sfp : sfo) {
          std::string k(sfp.key);
          if (k == "type" || k == "flow")
            continue;
          jw.key(k);
          jw.raw_value(simdjson::minify(sfp.value));
        }
        jw.end_object();
        jw.end_object(); // flows
      } else {
        // apikey, etc
        for (auto sfp : sfo) {
          jw.key(std::string(sfp.key));
          jw.raw_value(simdjson::minify(sfp.value));
        }
      }
      jw.end_object();
    }
    jw.end_object();
  }

  jw.end_object(); // components

  jw.end_object();
  std::string out = jw.str();
  auto replace_all = [](std::string &str, const std::string &from,
                        const std::string &to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
      str.replace(pos, from.length(), to);
      pos += to.length();
    }
  };
  replace_all(out, "\"#/definitions/", "\"#/components/schemas/");
  replace_all(out, "\"#/parameters/", "\"#/components/parameters/");
  replace_all(out, "\"#/responses/", "\"#/components/responses/");
  return out;
}

/// @brief Upgrade legacy OpenAPI/Swagger specification via `upgrade_to_latest`.
Result<std::string> upgrade_to_latest(const std::string &json_spec) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  if (parser.parse(json_spec).get(doc) != simdjson::SUCCESS ||
      !doc.is_object()) {
    return utils::make_error(utils::ErrorCode::JsonParseError,
                             "Invalid JSON document");
  }
  simdjson::dom::object root = unwrap_obj(doc);

  simdjson::dom::element version_el;
  if (root["swagger"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_sv;
    if (version_el.get(v_sv) != simdjson::SUCCESS)
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Invalid swagger version format");
    std::string v(v_sv);
    if (kSwagger1Versions.contains(v))
      return upgrade_swagger_1_2(root);
    if (v == "2.0")
      return upgrade_swagger_2_0(root);
    return utils::make_error(utils::ErrorCode::UnsupportedSpecVersion,
                             "Unknown swagger version: " + v);
  } else if (root["openapi"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_sv;
    if (version_el.get(v_sv) != simdjson::SUCCESS)
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Invalid openapi version format");
    std::string v(v_sv);
    if (v.starts_with("3.0") || v.starts_with("3.1")) {
      return upgrade_openapi_3(root, v);
    }
    if (v.starts_with("3.2")) {
      return json_spec; // Already latest
    }
    return utils::make_error(utils::ErrorCode::UnsupportedSpecVersion,
                             "Unknown openapi version: " + v);
  } else if (root["swaggerVersion"].get(version_el) ==
             simdjson::SUCCESS) { // Swagger 1.2 root
    return upgrade_swagger_1_2(root);
  }

  return utils::make_error(
      utils::ErrorCode::InvalidOpenApiDocument,
      "Could not detect OpenAPI/Swagger version in document");
}

} // namespace cdd_cpp::openapi::upgraders
