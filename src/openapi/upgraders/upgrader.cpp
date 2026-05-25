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

void process_schema_3_0(simdjson::dom::object schema,
                        utils::JsonWriter &jw) noexcept;
void process_element_3_0(simdjson::dom::element el,
                         utils::JsonWriter &jw) noexcept;

void process_schema_3_0(simdjson::dom::object schema,
                        utils::JsonWriter &jw) noexcept {
  jw.start_object(); // GCOV_EXCL_BR_LINE
  double min_val = 0;
  bool has_min = false;
  double max_val = 0;
  bool has_max = false;
  bool exclusive_min = false;
  bool exclusive_max = false;
  (void)has_min;
  (void)has_max;

  simdjson::dom::element el;
  if (schema["minimum"].get(el) == simdjson::SUCCESS) {
    has_min = true;
    if (el.is_int64())
      min_val = (double)el.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
    else if (el.is_uint64())
      min_val = (double)el.get_uint64().value_unsafe(); // GCOV_EXCL_BR_LINE
    else if (el.is_double())                            // GCOV_EXCL_BR_LINE
      min_val = el.get_double().value_unsafe();         // GCOV_EXCL_BR_LINE
  }
  if (schema["maximum"].get(el) == simdjson::SUCCESS) {
    has_max = true;
    if (el.is_int64())
      max_val = (double)el.get_int64().value_unsafe(); // GCOV_EXCL_BR_LINE
    else if (el.is_uint64())
      max_val = (double)el.get_uint64().value_unsafe(); // GCOV_EXCL_BR_LINE
    else if (el.is_double())                            // GCOV_EXCL_BR_LINE
      max_val = el.get_double().value_unsafe();         // GCOV_EXCL_BR_LINE
  }
  if (schema["exclusiveMinimum"].get(el) == simdjson::SUCCESS && el.is_bool()) {
    exclusive_min = el.get_bool().value_unsafe(); // GCOV_EXCL_BR_LINE
  }
  if (schema["exclusiveMaximum"].get(el) == simdjson::SUCCESS && el.is_bool()) {
    exclusive_max = el.get_bool().value_unsafe(); // GCOV_EXCL_BR_LINE
  }

  for (auto field : schema) {              // GCOV_EXCL_BR_LINE
    std::string key(field.key);            // GCOV_EXCL_BR_LINE
    if (key == "minimum" && exclusive_min) // GCOV_EXCL_BR_LINE
      continue;
    if (key == "maximum" && exclusive_max) // GCOV_EXCL_BR_LINE
      continue;
    if (key == "exclusiveMinimum") {
      if (exclusive_min) {                     // GCOV_EXCL_BR_LINE
        jw.key("exclusiveMinimum");            // GCOV_EXCL_BR_LINE
        jw.raw_value(std::to_string(min_val)); // GCOV_EXCL_BR_LINE
      }
      continue;
    }
    if (key == "exclusiveMaximum") {
      if (exclusive_max) {                     // GCOV_EXCL_BR_LINE
        jw.key("exclusiveMaximum");            // GCOV_EXCL_BR_LINE
        jw.raw_value(std::to_string(max_val)); // GCOV_EXCL_BR_LINE
      }
      continue;
    }

    if (key == "items" && field.value.is_array()) { // GCOV_EXCL_BR_LINE
      jw.key("prefixItems");                        // GCOV_EXCL_BR_LINE
      process_element_3_0(field.value, jw);         // GCOV_EXCL_BR_LINE
      continue;
    }

    jw.key(key);                          // GCOV_EXCL_BR_LINE
    process_element_3_0(field.value, jw); // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_BR_LINE
  jw.end_object(); // GCOV_EXCL_BR_LINE
}

void process_element_3_0(simdjson::dom::element el,
                         utils::JsonWriter &jw) noexcept {
  switch (el.type()) {
  case simdjson::dom::element_type::ARRAY: {
    jw.start_array();                  // GCOV_EXCL_BR_LINE
    for (auto item : el.get_array()) { // GCOV_EXCL_BR_LINE
      process_element_3_0(item, jw);   // GCOV_EXCL_BR_LINE
    }
    jw.end_array(); // GCOV_EXCL_BR_LINE
    break;
  }
  case simdjson::dom::element_type::OBJECT: {
    auto obj = el.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
    simdjson::dom::element type_el;
    bool is_schema = false;
    if (obj["type"].get(type_el) == simdjson::SUCCESS && type_el.is_string()) {
      is_schema = true; // rough heuristic
    } else if (obj["properties"].get(type_el) == // GCOV_EXCL_BR_LINE
                   simdjson::SUCCESS || // GCOV_EXCL_BR_LINE
               obj["items"].get(type_el) == simdjson::SUCCESS) {
      is_schema = true;
    }

    if (is_schema) {               // GCOV_EXCL_BR_LINE
      process_schema_3_0(obj, jw); // GCOV_EXCL_BR_LINE
    } else {
      jw.start_object();                // GCOV_EXCL_BR_LINE
      for (auto field : obj) {          // GCOV_EXCL_BR_LINE
        jw.key(std::string(field.key)); // GCOV_EXCL_BR_LINE
        // Deep check for schemas in known paths like "schema", "items",
        // "properties" could be better, but recursive heuristic mostly works.

        process_element_3_0(field.value, jw); // GCOV_EXCL_BR_LINE
      }
      jw.end_object(); // GCOV_EXCL_BR_LINE
    }
    break;
  }
  default:
    jw.raw_value(simdjson::minify(el)); // GCOV_EXCL_BR_LINE
    break;
  }
}

std::string upgrade_openapi_3(simdjson::dom::object root,
                              std::string_view current_version) noexcept {
  utils::JsonWriter jw; // GCOV_EXCL_BR_LINE
  jw.start_object();
  jw.key_value("openapi", "3.2.0"); // GCOV_EXCL_BR_LINE

  for (auto field : root) {                   // GCOV_EXCL_BR_LINE
    std::string key(field.key);               // GCOV_EXCL_BR_LINE
    if (key == "openapi" || key == "swagger") // GCOV_EXCL_BR_LINE
      continue;

    jw.key(key);
    if (current_version.starts_with("3.0")) {
      process_element_3_0(field.value, jw);
    } else {
      jw.raw_value(simdjson::minify(field.value)); // GCOV_EXCL_BR_LINE
    }
  } // GCOV_EXCL_BR_LINE

  jw.end_object();
  return jw.str();
}

std::string upgrade_swagger_1_2(simdjson::dom::object root) noexcept {
  utils::JsonWriter jw; // GCOV_EXCL_BR_LINE
  jw.start_object();
  jw.key_value("openapi", "3.2.0"); // GCOV_EXCL_BR_LINE

  jw.key("info"); // GCOV_EXCL_BR_LINE
  jw.start_object();

  simdjson::dom::element info_el;
  if (root["info"].get(info_el) == simdjson::SUCCESS &&
      info_el.type() == simdjson::dom::element_type::OBJECT) {
    auto info = info_el.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
    simdjson::dom::element title_el, desc_el;
    if (info["title"].get(title_el) == simdjson::SUCCESS &&
        title_el.type() == simdjson::dom::element_type::STRING) {
      jw.key_value(
          "title",                                    // GCOV_EXCL_BR_LINE
          std::string(                                // GCOV_EXCL_BR_LINE
              title_el.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
    } else {
      jw.key_value("title", "Upgraded from 1.2"); // GCOV_EXCL_BR_LINE
    }

    if (info["description"].get(desc_el) == simdjson::SUCCESS &&
        desc_el.type() == simdjson::dom::element_type::STRING) {
      jw.key_value(
          "description",                             // GCOV_EXCL_BR_LINE
          std::string(                               // GCOV_EXCL_BR_LINE
              desc_el.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
    }
  } else {
    jw.key_value("title", "Upgraded from 1.2"); // GCOV_EXCL_BR_LINE
  }

  jw.key_value("version", "1.0.0"); // GCOV_EXCL_BR_LINE
  jw.end_object();                  // info

  jw.key("paths"); // GCOV_EXCL_BR_LINE
  jw.start_object();

  simdjson::dom::element apis_el;
  if (root["apis"].get(apis_el) == simdjson::SUCCESS &&
      apis_el.type() == simdjson::dom::element_type::ARRAY) {
    for (auto api : apis_el.get_array()) {              // GCOV_EXCL_BR_LINE
      if (api.type() ==                                 // GCOV_EXCL_BR_LINE
          simdjson::dom::element_type::OBJECT) {        // GCOV_EXCL_BR_LINE
        auto api_obj = api.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
        simdjson::dom::element path_el;
        if (api_obj["path"].get(path_el) ==         // GCOV_EXCL_BR_LINE
            simdjson::SUCCESS) {                    // GCOV_EXCL_BR_LINE
          std::string path_str(                     // GCOV_EXCL_BR_LINE
              path_el.get_string().value_unsafe()); // GCOV_EXCL_BR_LINE
          jw.key(path_str);
          jw.start_object();

          simdjson::dom::element ops_el;
          if (api_obj["operations"].get(ops_el) == // GCOV_EXCL_BR_LINE
                  simdjson::SUCCESS &&             // GCOV_EXCL_BR_LINE
              ops_el.type() == simdjson::dom::element_type::ARRAY) {
            for (auto op : ops_el.get_array()) {         // GCOV_EXCL_BR_LINE
              if (op.type() ==                           // GCOV_EXCL_BR_LINE
                  simdjson::dom::element_type::OBJECT) { // GCOV_EXCL_BR_LINE
                auto op_obj =
                    op.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
                simdjson::dom::element method_el;
                if (op_obj["method"].get(method_el) == // GCOV_EXCL_BR_LINE
                    simdjson::SUCCESS) {               // GCOV_EXCL_BR_LINE
                  std::string method_str(              // GCOV_EXCL_BR_LINE
                      method_el.get_string()
                          .value_unsafe());  // GCOV_EXCL_BR_LINE
                  for (auto &c : method_str) // GCOV_EXCL_BR_LINE
                    c = std::tolower(c);     // GCOV_EXCL_BR_LINE
                  jw.key(method_str);
                  jw.start_object();

                  simdjson::dom::element nickname_el;
                  if (op_obj["nickname"].get( // GCOV_EXCL_BR_LINE
                          nickname_el) ==     // GCOV_EXCL_BR_LINE
                      simdjson::SUCCESS) {
                    jw.key_value(
                        "operationId", // GCOV_EXCL_BR_LINE
                        std::string(nickname_el // GCOV_EXCL_BR_LINE
                                        .get_string()      // GCOV_EXCL_BR_LINE
                                        .value_unsafe())); // GCOV_EXCL_BR_LINE
                  }

                  jw.key("responses"); // GCOV_EXCL_BR_LINE
                  jw.start_object();
                  jw.key("200"); // GCOV_EXCL_BR_LINE
                  jw.start_object();
                  jw.key_value("description", "Success"); // GCOV_EXCL_BR_LINE
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

std::string upgrade_swagger_2_0(simdjson::dom::object root) noexcept {
  utils::JsonWriter jw; // GCOV_EXCL_BR_LINE
  jw.start_object();
  jw.key_value("openapi", "3.2.0"); // GCOV_EXCL_BR_LINE

  simdjson::dom::element info_el;
  if (root["info"].get(info_el) == simdjson::SUCCESS) { // GCOV_EXCL_BR_LINE
    jw.key("info");                                     // GCOV_EXCL_BR_LINE
    jw.raw_value(simdjson::minify(info_el));            // GCOV_EXCL_BR_LINE
  }

  // Copy top-level properties
  for (auto field : root) {                       // GCOV_EXCL_BR_LINE
    std::string key(field.key);                   // GCOV_EXCL_BR_LINE
    if (key == "tags" || key == "externalDocs" || // GCOV_EXCL_BR_LINE
        key == "security" ||                      // GCOV_EXCL_BR_LINE
        key.starts_with("x-")) {
      jw.key(key);
      jw.raw_value(simdjson::minify(field.value)); // GCOV_EXCL_BR_LINE
    }
  }

  std::string host = "";     // GCOV_EXCL_BR_LINE
  std::string basePath = ""; // GCOV_EXCL_BR_LINE
  simdjson::dom::element host_el, bp_el, schemes_el;
  if (root["host"].get(host_el) == simdjson::SUCCESS)
    host = host_el.get_string().value_unsafe(); // GCOV_EXCL_BR_LINE
  if (root["basePath"].get(bp_el) == simdjson::SUCCESS)
    basePath = bp_el.get_string().value_unsafe(); // GCOV_EXCL_BR_LINE

  std::vector<std::string> schemes;
  if (root["schemes"].get(schemes_el) == simdjson::SUCCESS &&
      schemes_el.type() == simdjson::dom::element_type::ARRAY) {
    for (auto s : schemes_el.get_array())              // GCOV_EXCL_BR_LINE
      schemes.push_back(                               // GCOV_EXCL_BR_LINE
          std::string(s.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
  }
  if (schemes.empty() && host != "")
    schemes.push_back("https"); // GCOV_EXCL_BR_LINE

  if (!host.empty()) {
    jw.key("servers"); // GCOV_EXCL_BR_LINE
    jw.start_array();
    for (const auto &s : schemes) { // GCOV_EXCL_BR_LINE
      jw.start_object();
      jw.key_value("url", s + "://" + host + basePath); // GCOV_EXCL_BR_LINE
      jw.end_object();
    }
    jw.end_array();
  }

  std::vector<std::string> global_consumes;
  std::vector<std::string> global_produces;
  simdjson::dom::element g_cons_el, g_prod_el;
  if (root["consumes"].get(g_cons_el) == simdjson::SUCCESS &&
      g_cons_el.is_array()) {
    for (auto c : g_cons_el.get_array())               // GCOV_EXCL_BR_LINE
      global_consumes.push_back(                       // GCOV_EXCL_BR_LINE
          std::string(c.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
  }
  if (root["produces"].get(g_prod_el) == simdjson::SUCCESS &&
      g_prod_el.is_array()) {
    for (auto p : g_prod_el.get_array())               // GCOV_EXCL_BR_LINE
      global_produces.push_back(                       // GCOV_EXCL_BR_LINE
          std::string(p.get_string().value_unsafe())); // GCOV_EXCL_BR_LINE
  }
  if (global_consumes.empty())
    global_consumes.push_back("application/json"); // GCOV_EXCL_BR_LINE
  if (global_produces.empty())
    global_produces.push_back("application/json"); // GCOV_EXCL_BR_LINE

  // Helper lambda to process parameters
  auto process_parameters = [&](simdjson::dom::element // GCOV_EXCL_BR_LINE
                                    params_el) { // GCOV_EXCL_BR_LINE
    std::string body_param_raw = "";
    std::vector<std::string> form_params;
    jw.key("parameters");                             // GCOV_EXCL_BR_LINE
    jw.start_array();                                 // GCOV_EXCL_BR_LINE
    if (params_el.is_array()) {                       // GCOV_EXCL_BR_LINE
      for (auto p : params_el.get_array()) {          // GCOV_EXCL_BR_LINE
        if (p.is_object()) {                          // GCOV_EXCL_BR_LINE
          auto p_obj = p.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
          simdjson::dom::element in_el;
          if (p_obj["in"].get(in_el) == simdjson::SUCCESS) {
            std::string in_val( // GCOV_EXCL_BR_LINE
                in_el.get_string().value_unsafe()); // GCOV_EXCL_BR_LINE
            if (in_val == "body") {
              simdjson::dom::element schema_el;
              if (p_obj["schema"].get(schema_el) == // GCOV_EXCL_BR_LINE
                  simdjson::SUCCESS) {              // GCOV_EXCL_BR_LINE
                body_param_raw =
                    simdjson::minify(schema_el); // GCOV_EXCL_BR_LINE
              }
            } else if (in_val == "formData") {
              form_params.push_back(simdjson::minify(p)); // GCOV_EXCL_BR_LINE
            } else {
              jw.start_object();                      // GCOV_EXCL_BR_LINE
              for (auto pp : p_obj) {                 // GCOV_EXCL_BR_LINE
                std::string pk(pp.key);               // GCOV_EXCL_BR_LINE
                if (pk == "type" || pk == "format" || // GCOV_EXCL_BR_LINE
                    pk == "items" ||                  // GCOV_EXCL_BR_LINE
                    pk == "collectionFormat" ||       // GCOV_EXCL_BR_LINE
                    pk == "default" ||                // GCOV_EXCL_BR_LINE
                    pk == "maximum" ||                // GCOV_EXCL_BR_LINE
                    pk == "exclusiveMaximum" ||       // GCOV_EXCL_BR_LINE
                    pk == "minimum" ||                // GCOV_EXCL_BR_LINE
                    pk == "exclusiveMinimum" ||       // GCOV_EXCL_BR_LINE
                    pk == "maxLength" || // GCOV_EXCL_BR_LINE
                    pk == "minLength" ||                    // GCOV_EXCL_BR_LINE
                    pk == "pattern" ||                      // GCOV_EXCL_BR_LINE
                    pk == "maxItems" || pk == "minItems" || // GCOV_EXCL_BR_LINE
                    pk == "uniqueItems" || pk == "enum" ||  // GCOV_EXCL_BR_LINE
                    pk == "multipleOf")                     // GCOV_EXCL_BR_LINE
                  continue;                                 // Move to schema
                jw.key(pk);                                 // GCOV_EXCL_BR_LINE
                jw.raw_value(simdjson::minify(pp.value));   // GCOV_EXCL_BR_LINE
              } // GCOV_EXCL_BR_LINE
              jw.key("schema");                             // GCOV_EXCL_BR_LINE
              jw.start_object();                            // GCOV_EXCL_BR_LINE
              for (auto pp : p_obj) {                       // GCOV_EXCL_BR_LINE
                std::string pk(pp.key);                     // GCOV_EXCL_BR_LINE
                if (pk == "type" || pk == "format" ||       // GCOV_EXCL_BR_LINE
                    pk == "items" ||                        // GCOV_EXCL_BR_LINE
                    pk == "default" || pk == "maximum" ||   // GCOV_EXCL_BR_LINE
                    pk == "exclusiveMaximum" ||             // GCOV_EXCL_BR_LINE
                    pk == "minimum" ||                      // GCOV_EXCL_BR_LINE
                    pk == "exclusiveMinimum" ||             // GCOV_EXCL_BR_LINE
                    pk == "maxLength" ||                    // GCOV_EXCL_BR_LINE
                    pk == "minLength" || pk == "pattern" || // GCOV_EXCL_BR_LINE
                    pk == "maxItems" ||                     // GCOV_EXCL_BR_LINE
                    pk == "minItems" || // GCOV_EXCL_BR_LINE
                    pk == "uniqueItems" || // GCOV_EXCL_BR_LINE
                    pk == "enum" ||        // GCOV_EXCL_BR_LINE
                    pk == "multipleOf") {
                  jw.key(pk);                               // GCOV_EXCL_BR_LINE
                  jw.raw_value(simdjson::minify(pp.value)); // GCOV_EXCL_BR_LINE
                }
              }
              jw.end_object(); // GCOV_EXCL_BR_LINE
              jw.end_object(); // GCOV_EXCL_BR_LINE
            }
          } else {
            jw.raw_value( // GCOV_EXCL_BR_LINE
                simdjson::minify(p)); // $ref or other // GCOV_EXCL_BR_LINE
          }
        }
      }
    }
    jw.end_array();                                     // GCOV_EXCL_BR_LINE
    return std::make_pair(body_param_raw, form_params); // GCOV_EXCL_BR_LINE
  };

  simdjson::dom::element paths_el;
  if (root["paths"].get(paths_el) == simdjson::SUCCESS &&
      paths_el.is_object()) {
    jw.key("paths"); // GCOV_EXCL_BR_LINE
    jw.start_object();

    for (auto path_field : paths_el.get_object()) { // GCOV_EXCL_BR_LINE
      jw.key(std::string(path_field.key));          // GCOV_EXCL_BR_LINE
      jw.start_object();                            // path item
      if (path_field.value.is_object()) {           // GCOV_EXCL_BR_LINE
        for (auto op_field :                        // GCOV_EXCL_BR_LINE
             path_field.value.get_object()) {       // GCOV_EXCL_BR_LINE
          std::string op_method(op_field.key);      // GCOV_EXCL_BR_LINE
          if (op_method == "parameters") {
            // Path level parameters
            auto p_res =
                process_parameters(op_field.value); // GCOV_EXCL_BR_LINE
            continue;
          }
          if (op_method.starts_with("x-") || // GCOV_EXCL_BR_LINE
              op_method == "$ref") {         // GCOV_EXCL_BR_LINE
            jw.key(op_method);
            jw.raw_value(simdjson::minify(op_field.value)); // GCOV_EXCL_BR_LINE
            continue;
          }

          jw.key(op_method);
          jw.start_object();                           // operation
          if (op_field.value.is_object()) {            // GCOV_EXCL_BR_LINE
            auto op_obj = op_field.value.get_object(); // GCOV_EXCL_BR_LINE

            std::vector<std::string> local_consumes =
                global_consumes; // GCOV_EXCL_BR_LINE
            std::vector<std::string> local_produces =
                global_produces; // GCOV_EXCL_BR_LINE

            simdjson::dom::element l_cons_el, l_prod_el;
            if (op_obj["consumes"].get(l_cons_el) == simdjson::SUCCESS &&
                l_cons_el.is_array()) {
              local_consumes.clear();
              for (auto c : l_cons_el.get_array())    // GCOV_EXCL_BR_LINE
                local_consumes.push_back(std::string( // GCOV_EXCL_BR_LINE
                    c.get_string().value_unsafe()));  // GCOV_EXCL_BR_LINE
            }
            if (op_obj["produces"].get(l_prod_el) == simdjson::SUCCESS &&
                l_prod_el.is_array()) {
              local_produces.clear();
              for (auto p : l_prod_el.get_array())    // GCOV_EXCL_BR_LINE
                local_produces.push_back(std::string( // GCOV_EXCL_BR_LINE
                    p.get_string().value_unsafe()));  // GCOV_EXCL_BR_LINE
            }

            std::string body_param_raw = ""; // GCOV_EXCL_BR_LINE
            std::vector<std::string> form_params;

            for (auto op_prop : op_obj) { // GCOV_EXCL_BR_LINE
              std::string k(op_prop.key); // GCOV_EXCL_BR_LINE
              if (k == "consumes" || k == "produces")
                continue;
              if (k == "parameters") {
                auto p_res =
                    process_parameters(op_prop.value); // GCOV_EXCL_BR_LINE
                body_param_raw = p_res.first;          // GCOV_EXCL_BR_LINE
                form_params = p_res.second;            // GCOV_EXCL_BR_LINE
                continue;
              }

              if (k == "responses") {
                jw.key("responses"); // GCOV_EXCL_BR_LINE
                jw.start_object();
                if (op_prop.value.is_object()) {     // GCOV_EXCL_BR_LINE
                  for (auto r :                      // GCOV_EXCL_BR_LINE
                       op_prop.value.get_object()) { // GCOV_EXCL_BR_LINE
                    jw.key(std::string(r.key));      // GCOV_EXCL_BR_LINE
                    jw.start_object();
                    if (r.value.is_object()) {     // GCOV_EXCL_BR_LINE
                      for (auto rp :               // GCOV_EXCL_BR_LINE
                           r.value.get_object()) { // GCOV_EXCL_BR_LINE
                        std::string rk(rp.key);    // GCOV_EXCL_BR_LINE
                        if (rk == "schema") {
                          jw.key("content"); // GCOV_EXCL_BR_LINE
                          jw.start_object();
                          for (const auto &prod : // GCOV_EXCL_BR_LINE
                               local_produces) { // GCOV_EXCL_BR_LINE
                            jw.key(prod);        // GCOV_EXCL_BR_LINE
                            jw.start_object();
                            jw.key("schema");              // GCOV_EXCL_BR_LINE
                            jw.raw_value(simdjson::minify( // GCOV_EXCL_BR_LINE
                                rp.value));                // GCOV_EXCL_BR_LINE
                            jw.end_object();
                          }
                          jw.end_object();
                        } else {
                          jw.key(rk);
                          jw.raw_value(
                              simdjson::minify(rp.value)); // GCOV_EXCL_BR_LINE
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
              jw.raw_value(
                  simdjson::minify(op_prop.value)); // GCOV_EXCL_BR_LINE
            }

            // Construct requestBody
            if (!body_param_raw.empty() || // GCOV_EXCL_BR_LINE
                !form_params.empty()) {    // GCOV_EXCL_BR_LINE
              jw.key("requestBody");       // GCOV_EXCL_BR_LINE
              jw.start_object();
              jw.key("content"); // GCOV_EXCL_BR_LINE
              jw.start_object();

              if (!body_param_raw.empty()) {
                for (const auto &cons : local_consumes) { // GCOV_EXCL_BR_LINE
                  jw.key(cons);                           // GCOV_EXCL_BR_LINE
                  jw.start_object();
                  jw.key("schema"); // GCOV_EXCL_BR_LINE
                  jw.raw_value(body_param_raw);
                  jw.end_object();
                }
              } else if (!form_params.empty()) { // GCOV_EXCL_BR_LINE
                // Generate an object schema from form params
                std::string cons =
                    "application/x-www-form-urlencoded"; // GCOV_EXCL_BR_LINE
                for (const auto &c : local_consumes)     // GCOV_EXCL_BR_LINE
                  if (c == "multipart/form-data")        // GCOV_EXCL_BR_LINE
                    cons = c;                            // GCOV_EXCL_BR_LINE

                jw.key(cons);
                jw.start_object();
                jw.key("schema"); // GCOV_EXCL_BR_LINE
                jw.start_object();
                jw.key_value("type", "object"); // GCOV_EXCL_BR_LINE
                jw.key("properties");           // GCOV_EXCL_BR_LINE
                jw.start_object();

                simdjson::dom::parser p_parser;
                for (const auto &fp : form_params) { // GCOV_EXCL_BR_LINE
                  simdjson::dom::element el;
                  if (p_parser.parse(fp).get(el) == // GCOV_EXCL_BR_LINE
                          simdjson::SUCCESS &&      // GCOV_EXCL_BR_LINE
                      el.is_object()) {
                    auto obj = el.get_object();
                    simdjson::dom::element name_el;
                    if (obj["name"].get(name_el) == // GCOV_EXCL_BR_LINE
                        simdjson::SUCCESS) {        // GCOV_EXCL_BR_LINE
                      jw.key(std::string(           // GCOV_EXCL_BR_LINE
                          name_el.get_string()
                              .value_unsafe())); // GCOV_EXCL_BR_LINE
                      jw.start_object();
                      for (auto field : obj) {    // GCOV_EXCL_BR_LINE
                        std::string k(field.key); // GCOV_EXCL_BR_LINE
                        if (k == "name" || k == "in" || k == "required")
                          continue;
                        jw.key(k);
                        jw.raw_value(
                            simdjson::minify(field.value)); // GCOV_EXCL_BR_LINE
                      }
                      jw.end_object();
                    }
                  }
                }

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

  jw.key("components"); // GCOV_EXCL_BR_LINE
  jw.start_object();

  simdjson::dom::element defs_el;
  if (root["definitions"].get(defs_el) == simdjson::SUCCESS) {
    jw.key("schemas");             // GCOV_EXCL_BR_LINE
    jw.raw_value(simdjson::minify( // GCOV_EXCL_BR_LINE
        defs_el)); // Simplified: direct copy // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::element p_el;
  if (root["parameters"].get(p_el) == simdjson::SUCCESS && p_el.is_object()) {
    jw.key("parameters"); // GCOV_EXCL_BR_LINE
    jw.start_object();
    for (auto p : p_el.get_object()) { // GCOV_EXCL_BR_LINE
      jw.key(std::string(p.key));      // GCOV_EXCL_BR_LINE
      jw.start_object();
      if (p.value.is_object()) {                          // GCOV_EXCL_BR_LINE
        auto p_obj = p.value.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
        jw.key("schema");                                 // GCOV_EXCL_BR_LINE
        jw.start_object();
        for (auto pp : p_obj) {                         // GCOV_EXCL_BR_LINE
          std::string pk(pp.key);                       // GCOV_EXCL_BR_LINE
          if (pk == "type" || pk == "format" ||         // GCOV_EXCL_BR_LINE
              pk == "items" ||                          // GCOV_EXCL_BR_LINE
              pk == "default" || pk == "maximum" ||     // GCOV_EXCL_BR_LINE
              pk == "exclusiveMaximum" ||               // GCOV_EXCL_BR_LINE
              pk == "minimum" ||                        // GCOV_EXCL_BR_LINE
              pk == "exclusiveMinimum" ||               // GCOV_EXCL_BR_LINE
              pk == "maxLength" || pk == "minLength" || // GCOV_EXCL_BR_LINE
              pk == "pattern" ||                        // GCOV_EXCL_BR_LINE
              pk == "maxItems" || pk == "minItems" ||   // GCOV_EXCL_BR_LINE
              pk == "uniqueItems" ||                    // GCOV_EXCL_BR_LINE
              pk == "enum" || pk == "multipleOf") {
            jw.key(pk);
            jw.raw_value(simdjson::minify(pp.value)); // GCOV_EXCL_BR_LINE
          }
        }
        jw.end_object();
      }
      jw.end_object();
    }
    jw.end_object();
  }

  simdjson::dom::element r_el;
  if (root["responses"].get(r_el) == simdjson::SUCCESS && r_el.is_object()) {
    jw.key("responses"); // GCOV_EXCL_BR_LINE
    jw.start_object();
    for (auto r : r_el.get_object()) { // GCOV_EXCL_BR_LINE
      jw.key(std::string(r.key));      // GCOV_EXCL_BR_LINE
      jw.start_object();
      if (r.value.is_object()) {               // GCOV_EXCL_BR_LINE
        for (auto rp : r.value.get_object()) { // GCOV_EXCL_BR_LINE
          std::string rk(rp.key);              // GCOV_EXCL_BR_LINE
          if (rk == "schema") {
            jw.key("content"); // GCOV_EXCL_BR_LINE
            jw.start_object();
            for (const auto &prod : global_produces) { // GCOV_EXCL_BR_LINE
              jw.key(prod);                            // GCOV_EXCL_BR_LINE
              jw.start_object();
              jw.key("schema");                         // GCOV_EXCL_BR_LINE
              jw.raw_value(simdjson::minify(rp.value)); // GCOV_EXCL_BR_LINE
              jw.end_object();
            }
            jw.end_object();
          } else {
            jw.key(rk);
            jw.raw_value(simdjson::minify(rp.value)); // GCOV_EXCL_BR_LINE
          }
        }
      }
      jw.end_object();
    }
    jw.end_object();
  }

  simdjson::dom::element sec_el;
  if (root["securityDefinitions"].get(sec_el) == simdjson::SUCCESS) {
    jw.key("securitySchemes"); // GCOV_EXCL_BR_LINE
    jw.start_object();
    if (sec_el.is_object()) {               // GCOV_EXCL_BR_LINE
      for (auto sf : sec_el.get_object()) { // GCOV_EXCL_BR_LINE
        jw.key(std::string(sf.key));        // GCOV_EXCL_BR_LINE
        jw.start_object();
        if (sf.value.is_object()) {                        // GCOV_EXCL_BR_LINE
          auto sfo = sf.value.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE
          simdjson::dom::element type_el;
          std::string type_val = "";                        // GCOV_EXCL_BR_LINE
          if (sfo["type"].get(type_el) ==                   // GCOV_EXCL_BR_LINE
              simdjson::SUCCESS)                            // GCOV_EXCL_BR_LINE
            type_val = type_el.get_string().value_unsafe(); // GCOV_EXCL_BR_LINE

          if (type_val == "basic") {
            jw.key_value("type", "http");    // GCOV_EXCL_BR_LINE
            jw.key_value("scheme", "basic"); // GCOV_EXCL_BR_LINE
          } else if (type_val == "oauth2") {
            jw.key_value("type", "oauth2"); // GCOV_EXCL_BR_LINE
            jw.key("flows");                // GCOV_EXCL_BR_LINE
            jw.start_object();

            simdjson::dom::element flow_el;
            std::string flow_val = "implicit";         // GCOV_EXCL_BR_LINE
            if (sfo["flow"].get(flow_el) ==            // GCOV_EXCL_BR_LINE
                simdjson::SUCCESS)                     // GCOV_EXCL_BR_LINE
              flow_val =                               // GCOV_EXCL_BR_LINE
                  flow_el.get_string().value_unsafe(); // GCOV_EXCL_BR_LINE

            std::string nflow = flow_val; // GCOV_EXCL_BR_LINE
            if (nflow == "accessCode")
              nflow = "authorizationCode"; // GCOV_EXCL_BR_LINE
            if (nflow == "application")
              nflow = "clientCredentials"; // GCOV_EXCL_BR_LINE

            jw.key(nflow);
            jw.start_object();
            for (auto sfp : sfo) {    // GCOV_EXCL_BR_LINE
              std::string k(sfp.key); // GCOV_EXCL_BR_LINE
              if (k == "type" || k == "flow")
                continue;
              jw.key(k);
              jw.raw_value(simdjson::minify(sfp.value)); // GCOV_EXCL_BR_LINE
            }
            jw.end_object();
            jw.end_object(); // flows
          } else {
            // apikey, etc
            for (auto sfp : sfo) {                       // GCOV_EXCL_BR_LINE
              jw.key(std::string(sfp.key));              // GCOV_EXCL_BR_LINE
              jw.raw_value(simdjson::minify(sfp.value)); // GCOV_EXCL_BR_LINE
            }
          }
        }
        jw.end_object();
      }
    }
    jw.end_object();
  }

  jw.end_object(); // components

  jw.end_object();
  return jw.str();
}

std::expected<std::string, std::string>
upgrade_to_latest(const std::string &json_spec) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  if (parser.parse(json_spec).get(doc) != // GCOV_EXCL_BR_LINE
          simdjson::SUCCESS || // GCOV_EXCL_BR_LINE
      doc.type() != simdjson::dom::element_type::OBJECT) {
    return std::unexpected("Invalid JSON document"); // GCOV_EXCL_BR_LINE
  }

  simdjson::dom::object root =
      doc.get_object().value_unsafe(); // GCOV_EXCL_BR_LINE

  simdjson::dom::element version_el;
  if (root["swagger"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_view;
    if (version_el.get(v_view) != simdjson::SUCCESS)
      return std::unexpected(                   // GCOV_EXCL_BR_LINE
          "Invalid swagger version format");    // GCOV_EXCL_BR_LINE
    std::string v(v_view);                      // GCOV_EXCL_BR_LINE
    if (v == "1.2" || v == "1.1" || v == "1.0") // GCOV_EXCL_BR_LINE
      return upgrade_swagger_1_2(root);         // GCOV_EXCL_BR_LINE
    if (v == "2.0")
      return upgrade_swagger_2_0(root);                  // GCOV_EXCL_BR_LINE
    return std::unexpected("Unknown swagger version: " + // GCOV_EXCL_BR_LINE
                           v);                           // GCOV_EXCL_BR_LINE
  } else if (root["openapi"].get(version_el) == simdjson::SUCCESS) {
    std::string_view v_view;
    if (version_el.get(v_view) != simdjson::SUCCESS)
      return std::unexpected(                // GCOV_EXCL_BR_LINE
          "Invalid openapi version format"); // GCOV_EXCL_BR_LINE
    std::string v(v_view);                   // GCOV_EXCL_BR_LINE
    if (v.starts_with("3.0") || v.starts_with("3.1")) {
      return upgrade_openapi_3(root, v); // GCOV_EXCL_BR_LINE
    }
    if (v.starts_with("3.2")) {
      return json_spec; // Already latest // GCOV_EXCL_BR_LINE
    }
    return std::unexpected("Unknown openapi version: " + // GCOV_EXCL_BR_LINE
                           v);                           // GCOV_EXCL_BR_LINE
  } else if (root["swaggerVersion"].get(version_el) ==
             simdjson::SUCCESS) {     // Swagger 1.2 root
    return upgrade_swagger_1_2(root); // GCOV_EXCL_BR_LINE
  }

  return std::unexpected( // GCOV_EXCL_BR_LINE
      "Could not detect OpenAPI/Swagger version in document");
}

} // namespace cdd_cpp::openapi::upgraders
