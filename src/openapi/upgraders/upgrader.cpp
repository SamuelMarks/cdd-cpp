#include "upgrader.hpp"
#include "../../utils/json_writer.hpp"
#include "../emit.hpp"
#include "../parse.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace cdd_cpp::openapi::upgraders {

void process_schema_3_0(simdjson::dom::object schema, utils::JsonWriter &jw);
void process_element_3_0(simdjson::dom::element el, utils::JsonWriter &jw);

void process_schema_3_0(simdjson::dom::object schema, utils::JsonWriter &jw) {
  jw.start_object();
  double min_val = 0;
  bool has_min = false;
  double max_val = 0;
  bool has_max = false;
  bool exclusive_min = false;
  bool exclusive_max = false;

  simdjson::dom::element el;
  if (schema["minimum"].get(el) == simdjson::SUCCESS) {
    has_min = true;
    if (el.is_int64())
      min_val = (double)el.get_int64().value();
    else if (el.is_uint64())
      min_val = (double)el.get_uint64().value();
    else if (el.is_double())
      min_val = el.get_double().value();
  }
  if (schema["maximum"].get(el) == simdjson::SUCCESS) {
    has_max = true;
    if (el.is_int64())
      max_val = (double)el.get_int64().value();
    else if (el.is_uint64())
      max_val = (double)el.get_uint64().value();
    else if (el.is_double())
      max_val = el.get_double().value();
  }
  if (schema["exclusiveMinimum"].get(el) == simdjson::SUCCESS && el.is_bool()) {
    exclusive_min = el.get_bool().value();
  }
  if (schema["exclusiveMaximum"].get(el) == simdjson::SUCCESS && el.is_bool()) {
    exclusive_max = el.get_bool().value();
  }

  for (auto field : schema) {
    std::string key(field.key);
    if (key == "minimum" && exclusive_min)
      continue;
    if (key == "maximum" && exclusive_max)
      continue;
    if (key == "exclusiveMinimum") {
      if (exclusive_min) {
        jw.key("exclusiveMinimum");
        jw.raw_value(std::to_string(min_val));
      }
      continue;
    }
    if (key == "exclusiveMaximum") {
      if (exclusive_max) {
        jw.key("exclusiveMaximum");
        jw.raw_value(std::to_string(max_val));
      }
      continue;
    }

    jw.key(key);
    process_element_3_0(field.value, jw);
  }
  jw.end_object();
}

void process_element_3_0(simdjson::dom::element el, utils::JsonWriter &jw) {
  switch (el.type()) {
  case simdjson::dom::element_type::ARRAY: {
    jw.start_array();
    for (auto item : el.get_array()) {
      process_element_3_0(item, jw);
    }
    jw.end_array();
    break;
  }
  case simdjson::dom::element_type::OBJECT: {
    auto obj = el.get_object().value();
    simdjson::dom::element type_el;
    bool is_schema = false;
    if (obj["type"].get(type_el) == simdjson::SUCCESS && type_el.is_string()) {
      is_schema = true; // rough heuristic
    } else if (obj["properties"].get(type_el) == simdjson::SUCCESS ||
               obj["items"].get(type_el) == simdjson::SUCCESS) {
      is_schema = true;
    }

    if (is_schema) {
      process_schema_3_0(obj, jw);
    } else {
      jw.start_object();
      for (auto field : obj) {
        jw.key(std::string(field.key));
        // Deep check for schemas in known paths like "schema", "items",
        // "properties" could be better, but recursive heuristic mostly works.
        if (std::string(field.key) == "schema" ||
            std::string(field.key) == "items") {
          if (field.value.is_object())
            process_schema_3_0(field.value.get_object(), jw);
          else
            process_element_3_0(field.value, jw);
        } else {
          process_element_3_0(field.value, jw);
        }
      }
      jw.end_object();
    }
    break;
  }
  default:
    jw.raw_value(simdjson::minify(el));
    break;
  }
}

std::string upgrade_openapi_3(simdjson::dom::object root,
                              std::string_view current_version) {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  for (auto field : root) {
    std::string key(field.key);
    if (key == "openapi" || key == "swagger")
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

std::string upgrade_swagger_1_2(simdjson::dom::object root) {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  jw.key("info");
  jw.start_object();

  simdjson::dom::element info_el;
  if (root["info"].get(info_el) == simdjson::SUCCESS &&
      info_el.type() == simdjson::dom::element_type::OBJECT) {
    auto info = info_el.get_object().value();
    simdjson::dom::element title_el, desc_el;
    if (info["title"].get(title_el) == simdjson::SUCCESS &&
        title_el.type() == simdjson::dom::element_type::STRING) {
      jw.key_value("title", std::string(title_el.get_string().value()));
    } else {
      jw.key_value("title", "Upgraded from 1.2");
    }

    if (info["description"].get(desc_el) == simdjson::SUCCESS &&
        desc_el.type() == simdjson::dom::element_type::STRING) {
      jw.key_value("description", std::string(desc_el.get_string().value()));
    }
  } else {
    jw.key_value("title", "Upgraded from 1.2");
  }

  jw.key_value("version", "1.0.0");
  jw.end_object(); // info

  jw.key("paths");
  jw.start_object();

  simdjson::dom::element apis_el;
  if (root["apis"].get(apis_el) == simdjson::SUCCESS &&
      apis_el.type() == simdjson::dom::element_type::ARRAY) {
    for (auto api : apis_el.get_array()) {
      if (api.type() == simdjson::dom::element_type::OBJECT) {
        auto api_obj = api.get_object().value();
        simdjson::dom::element path_el;
        if (api_obj["path"].get(path_el) == simdjson::SUCCESS) {
          std::string path_str(path_el.get_string().value());
          jw.key(path_str);
          jw.start_object();

          simdjson::dom::element ops_el;
          if (api_obj["operations"].get(ops_el) == simdjson::SUCCESS &&
              ops_el.type() == simdjson::dom::element_type::ARRAY) {
            for (auto op : ops_el.get_array()) {
              if (op.type() == simdjson::dom::element_type::OBJECT) {
                auto op_obj = op.get_object().value();
                simdjson::dom::element method_el;
                if (op_obj["method"].get(method_el) == simdjson::SUCCESS) {
                  std::string method_str(method_el.get_string().value());
                  for (auto &c : method_str)
                    c = std::tolower(c);
                  jw.key(method_str);
                  jw.start_object();

                  simdjson::dom::element nickname_el;
                  if (op_obj["nickname"].get(nickname_el) ==
                      simdjson::SUCCESS) {
                    jw.key_value("operationId",
                                 std::string(nickname_el.get_string().value()));
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
            }
          }

          jw.end_object(); // path
        }
      }
    }
  }

  jw.end_object(); // paths

  jw.end_object();
  return jw.str();
}

std::string upgrade_swagger_2_0(simdjson::dom::object root) {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("openapi", "3.2.0");

  simdjson::dom::element info_el;
  if (root["info"].get(info_el) == simdjson::SUCCESS) {
    jw.key("info");
    jw.raw_value(simdjson::minify(info_el));
  }

  std::string host = "";
  std::string basePath = "";
  simdjson::dom::element host_el, bp_el, schemes_el;
  if (root["host"].get(host_el) == simdjson::SUCCESS)
    host = host_el.get_string().value();
  if (root["basePath"].get(bp_el) == simdjson::SUCCESS)
    basePath = bp_el.get_string().value();

  std::vector<std::string> schemes;
  if (root["schemes"].get(schemes_el) == simdjson::SUCCESS &&
      schemes_el.type() == simdjson::dom::element_type::ARRAY) {
    for (auto s : schemes_el.get_array())
      schemes.push_back(std::string(s.get_string().value()));
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
  simdjson::dom::element g_cons_el, g_prod_el;
  if (root["consumes"].get(g_cons_el) == simdjson::SUCCESS &&
      g_cons_el.is_array()) {
    for (auto c : g_cons_el.get_array())
      global_consumes.push_back(std::string(c.get_string().value()));
  }
  if (root["produces"].get(g_prod_el) == simdjson::SUCCESS &&
      g_prod_el.is_array()) {
    for (auto p : g_prod_el.get_array())
      global_produces.push_back(std::string(p.get_string().value()));
  }
  if (global_consumes.empty())
    global_consumes.push_back("application/json");
  if (global_produces.empty())
    global_produces.push_back("application/json");

  simdjson::dom::element paths_el;
  if (root["paths"].get(paths_el) == simdjson::SUCCESS &&
      paths_el.is_object()) {
    jw.key("paths");
    jw.start_object();

    for (auto path_field : paths_el.get_object()) {
      jw.key(std::string(path_field.key));
      jw.start_object(); // path item
      if (path_field.value.is_object()) {
        for (auto op_field : path_field.value.get_object()) {
          std::string op_method(op_field.key);
          if (op_method == "parameters") {
            jw.key("parameters");
            jw.raw_value(simdjson::minify(op_field.value));
            continue;
          }

          jw.key(op_method);
          jw.start_object(); // operation
          if (op_field.value.is_object()) {
            auto op_obj = op_field.value.get_object();

            std::vector<std::string> local_consumes = global_consumes;
            std::vector<std::string> local_produces = global_produces;

            simdjson::dom::element l_cons_el, l_prod_el;
            if (op_obj["consumes"].get(l_cons_el) == simdjson::SUCCESS &&
                l_cons_el.is_array()) {
              local_consumes.clear();
              for (auto c : l_cons_el.get_array())
                local_consumes.push_back(std::string(c.get_string().value()));
            }
            if (op_obj["produces"].get(l_prod_el) == simdjson::SUCCESS &&
                l_prod_el.is_array()) {
              local_produces.clear();
              for (auto p : l_prod_el.get_array())
                local_produces.push_back(std::string(p.get_string().value()));
            }

            std::vector<std::string> form_params;
            std::string body_param_raw = "";

            for (auto op_prop : op_obj) {
              std::string k(op_prop.key);
              if (k == "consumes" || k == "produces")
                continue;
              if (k == "parameters") {
                jw.key("parameters");
                jw.start_array();
                for (auto p : op_prop.value.get_array()) {
                  if (p.is_object()) {
                    auto p_obj = p.get_object().value();
                    simdjson::dom::element in_el;
                    if (p_obj["in"].get(in_el) == simdjson::SUCCESS) {
                      std::string in_val(in_el.get_string().value());
                      if (in_val == "body") {
                        // extract body schema
                        simdjson::dom::element schema_el;
                        if (p_obj["schema"].get(schema_el) ==
                            simdjson::SUCCESS) {
                          body_param_raw = simdjson::minify(schema_el);
                        }
                      } else if (in_val == "formData") {
                        form_params.push_back(simdjson::minify(p));
                      } else {
                        // Normal parameter, need to copy type to schema
                        jw.start_object();
                        for (auto pp : p_obj) {
                          std::string pk(pp.key);
                          if (pk == "type" || pk == "format" || pk == "items")
                            continue; // Move to schema
                          jw.key(pk);
                          jw.raw_value(simdjson::minify(pp.value));
                        }
                        jw.key("schema");
                        jw.start_object();
                        simdjson::dom::element tel, fel, iel;
                        if (p_obj["type"].get(tel) == simdjson::SUCCESS) {
                          jw.key("type");
                          jw.raw_value(simdjson::minify(tel));
                        }
                        if (p_obj["format"].get(fel) == simdjson::SUCCESS) {
                          jw.key("format");
                          jw.raw_value(simdjson::minify(fel));
                        }
                        if (p_obj["items"].get(iel) == simdjson::SUCCESS) {
                          jw.key("items");
                          jw.raw_value(simdjson::minify(iel));
                        }
                        jw.end_object();
                        jw.end_object();
                      }
                    } else {
                      jw.raw_value(simdjson::minify(p));
                    }
                  }
                }
                jw.end_array();
                continue;
              }

              if (k == "responses") {
                jw.key("responses");
                jw.start_object();
                if (op_prop.value.is_object()) {
                  for (auto r : op_prop.value.get_object()) {
                    jw.key(std::string(r.key));
                    jw.start_object();
                    if (r.value.is_object()) {
                      for (auto rp : r.value.get_object()) {
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
                    }
                    jw.end_object();
                  }
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
              } else if (!form_params.empty()) {
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

                // Let's just output dummy for simplicity right now unless we
                // really re-parse

                jw.end_object(); // properties
                jw.end_object(); // schema
                jw.end_object(); // cons
              }

              jw.end_object(); // content
              jw.end_object(); // requestBody
            }
          }
          jw.end_object(); // operation
        }
      }
      jw.end_object(); // path item
    }
    jw.end_object(); // paths
  }

  simdjson::dom::element defs_el;
  if (root["definitions"].get(defs_el) == simdjson::SUCCESS) {
    jw.key("components");
    jw.start_object();
    jw.key("schemas");
    jw.raw_value(simdjson::minify(defs_el)); // Simplified: direct copy

    simdjson::dom::element sec_el;
    if (root["securityDefinitions"].get(sec_el) == simdjson::SUCCESS) {
      jw.key("securitySchemes");
      // basic -> http/basic
      // oauth2 -> flows
      jw.start_object();
      if (sec_el.is_object()) {
        for (auto sf : sec_el.get_object()) {
          jw.key(std::string(sf.key));
          jw.start_object();
          if (sf.value.is_object()) {
            auto sfo = sf.value.get_object().value();
            simdjson::dom::element type_el;
            std::string type_val = "";
            if (sfo["type"].get(type_el) == simdjson::SUCCESS)
              type_val = type_el.get_string().value();

            if (type_val == "basic") {
              jw.key_value("type", "http");
              jw.key_value("scheme", "basic");
            } else if (type_val == "oauth2") {
              jw.key_value("type", "oauth2");
              jw.key("flows");
              jw.start_object();

              simdjson::dom::element flow_el;
              std::string flow_val = "implicit";
              if (sfo["flow"].get(flow_el) == simdjson::SUCCESS)
                flow_val = flow_el.get_string().value();

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
          }
          jw.end_object();
        }
      }
      jw.end_object();
    }

    jw.end_object(); // components
  }

  jw.end_object();
  return jw.str();
}

std::string upgrade_to_latest(const std::string &json_spec) {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  if (parser.parse(json_spec).get(doc) != simdjson::SUCCESS ||
      doc.type() != simdjson::dom::element_type::OBJECT) {
    throw std::runtime_error("Invalid JSON document");
  }

  simdjson::dom::object root;
  if (doc.get(root) != simdjson::SUCCESS) {
    throw std::runtime_error("Invalid root object");
  }

  simdjson::dom::element version_el;
  if (root["swagger"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_view;
    if (version_el.get(v_view) != simdjson::SUCCESS)
      throw std::runtime_error("Invalid swagger version format");
    std::string v(v_view);
    if (v == "1.2" || v == "1.1" || v == "1.0")
      return upgrade_swagger_1_2(root);
    if (v == "2.0")
      return upgrade_swagger_2_0(root);
    throw std::runtime_error("Unknown swagger version: " + v);
  } else if (root["openapi"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_view;
    if (version_el.get(v_view) != simdjson::SUCCESS)
      throw std::runtime_error("Invalid openapi version format");
    std::string v(v_view);
    if (v.starts_with("3.0") || v.starts_with("3.1")) {
      return upgrade_openapi_3(root, v);
    }
    if (v.starts_with("3.2")) {
      return json_spec; // Already latest
    }
    throw std::runtime_error("Unknown openapi version: " + v);
  } else if (root["swaggerVersion"].get(version_el) ==
             simdjson::SUCCESS) { // Swagger 1.2 root
    return upgrade_swagger_1_2(root);
  }

  throw std::runtime_error(
      "Could not detect OpenAPI/Swagger version in document");
}

} // namespace cdd_cpp::openapi::upgraders
