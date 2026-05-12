
#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include "../models/emit.hpp"
#include <sstream>

namespace cdd_cpp::client_sdk {

std::string map_type(const openapi::Schema &schema) noexcept {
  if (schema.ref.has_value()) {
    std::string ref = schema.ref.value().ref;
    size_t last_slash = ref.find_last_of('/');
    if (last_slash != std::string::npos) {
      return ref.substr(last_slash + 1);
    }
    return ref;
  }
  if (!schema.type.has_value())
    return "std::string";

  std::string t = schema.type.value();
  if (t == "integer")
    return "int";
  if (t == "number")
    return "double";
  if (t == "boolean")
    return "bool";
  if (t == "string")
    return "std::string";
  if (t == "array") {
    if (schema.items)
      return "std::vector<" + map_type(*schema.items) + ">";
    return "std::vector<std::string>";
  }
  return "std::string";
}

std::map<std::string, std::string> emit_client(const openapi::OpenAPI &spec) noexcept {
  std::map<std::string, std::string> result;

  // 1. models.hpp
  std::stringstream m_hpp;
  m_hpp << "#pragma once\n";
  m_hpp << "#include <string>\n";
  m_hpp << "#include <vector>\n";
  m_hpp << "#include <optional>\n";
  m_hpp << "#include <simdjson.h>\n\n";
  m_hpp << "namespace cdd_models {\n\n";

  if (spec.components && spec.components->schemas) {
    for (const auto &[name, schema] : *spec.components->schemas) {
      m_hpp << "    struct " << name << " {\n";
      if (schema.properties) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          bool is_required = false;
          if (schema.required) {
            for (const auto &req : *schema.required) {
              if (req == prop_name) is_required = true;
            }
          }
          if (is_required) {
            m_hpp << "        " << map_type(prop_schema) << " " << prop_name << ";\n";
          } else {
            m_hpp << "        std::optional<" << map_type(prop_schema) << "> " << prop_name << ";\n";
          }
        }
      }
      m_hpp << "    };\n\n";
      m_hpp << "    " << name << " from_json_" << name << "(simdjson::ondemand::value val);\n";
      m_hpp << "    std::string to_json(const " << name << "& obj);\n\n";
    }
  }
  m_hpp << "}\n";
  result["models.hpp"] = m_hpp.str();

  // 2. models.cpp
  std::stringstream m_cpp;
  m_cpp << "#include \"models.hpp\"\n";
  m_cpp << "#include <sstream>\n";
  m_cpp << "#include <iomanip>\n\n";
  m_cpp << "namespace cdd_models {\n\n";

  m_cpp << "    static void escape_string(std::stringstream& ss, const std::string &str) noexcept {\n";
  m_cpp << "        ss << '\"';\n";
  m_cpp << "        for (unsigned char c : str) {\n";
  m_cpp << "            if (c == '\"') ss << \"\\\\\\\"\";\n";
  m_cpp << "            else if (c == '\\') ss << \"\\\\\\\\\";\n";
  m_cpp << "            else if (c == '\b') ss << \"\\\\b\";\n";
  m_cpp << "            else if (c == '\f') ss << \"\\\\f\";\n";
  m_cpp << "            else if (c == '\n') ss << \"\\\\n\";\n";
  m_cpp << "            else if (c == '\r') ss << \"\\\\r\";\n";
  m_cpp << "            else if (c == '\t') ss << \"\\\\t\";\n";
  m_cpp << "            else if (c <= 0x1f) {\n";
  m_cpp << "                ss << \"\\\\u\" << std::hex << std::setw(4) << std::setfill('0') << (int)c << std::dec;\n";
  m_cpp << "            } else ss << c;\n";
  m_cpp << "        }\n";
  m_cpp << "        ss << '\"';\n";
  m_cpp << "    }\n\n";

  if (spec.components && spec.components->schemas) {
    for (const auto &[name, schema] : *spec.components->schemas) {
      m_cpp << "    " << name << " from_json_" << name << "(simdjson::ondemand::value val) {\n";
      m_cpp << "        " << name << " obj;\n";
      
      if (schema.properties) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          std::string type = map_type(prop_schema);
          
          m_cpp << "        simdjson::ondemand::value val_" << prop_name << ";\n";
          m_cpp << "        if (val[\"" << prop_name << "\"].get(val_" << prop_name << ") == simdjson::SUCCESS) {\n";
          
          if (type == "int") {
            m_cpp << "            obj." << prop_name << " = val_" << prop_name << ".get_int64();\n";
          } else if (type == "double") {
            m_cpp << "            obj." << prop_name << " = val_" << prop_name << ".get_double();\n";
          } else if (type == "bool") {
            m_cpp << "            obj." << prop_name << " = val_" << prop_name << ".get_bool();\n";
          } else if (type == "std::string") {
            m_cpp << "            obj." << prop_name << " = std::string(val_" << prop_name << ".get_string().value());\n";
          } else {
            // Nested object or array not fully implemented in this minimal snippet
            m_cpp << "            // nested object or array\n";
          }
          m_cpp << "        }\n";
        }
      }
      
      m_cpp << "        return obj;\n";
      m_cpp << "    }\n\n";

      m_cpp << "    std::string to_json(const " << name << "& obj) {\n";
      m_cpp << "        std::stringstream ss;\n";
      m_cpp << "        ss << \"{\";\n";
      m_cpp << "        bool first = true;\n";
      
      if (schema.properties) {
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          bool is_required = false;
          if (schema.required) {
            for (const auto &req : *schema.required) {
              if (req == prop_name) is_required = true;
            }
          }
          std::string type = map_type(prop_schema);
          
          if (!is_required) {
            m_cpp << "        if (obj." << prop_name << ".has_value()) {\n";
          }
          
          m_cpp << "        if (!first) ss << \",\";\n";
          m_cpp << "        first = false;\n";
          m_cpp << "        escape_string(ss, \"" << prop_name << "\");\n";
          m_cpp << "        ss << \":\";\n";
          
          std::string val_ref = is_required ? ("obj." + prop_name) : ("obj." + prop_name + ".value()");
          if (type == "int" || type == "double") {
            m_cpp << "        ss << " << val_ref << ";\n";
          } else if (type == "bool") {
            m_cpp << "        ss << (" << val_ref << " ? \"true\" : \"false\");\n";
          } else if (type == "std::string") {
            m_cpp << "        escape_string(ss, " << val_ref << ");\n";
          } else {
             m_cpp << "        ss << \"null\";\n";
          }
          
          if (!is_required) {
            m_cpp << "        }\n";
          }
        }
      }
      
      m_cpp << "        ss << \"}\";\n";
      m_cpp << "        return ss.str();\n";
      m_cpp << "    }\n\n";
    }
  }
  m_cpp << "}\n";
  result["models.cpp"] = m_cpp.str();

  // 3. client.hpp
  std::stringstream c_hpp;
  c_hpp << "#pragma once\n";
  c_hpp << "#include \"models.hpp\"\n";
  c_hpp << "#include <expected>\n";
  c_hpp << "#include <string>\n";
  c_hpp << "#include <optional>\n";
  c_hpp << "#include <curl/curl.h>\n\n";
  c_hpp << "namespace cdd_client {\n\n";
  c_hpp << docstrings::emit_api_docstrings(spec);
  c_hpp << "    class Client {\n";
  c_hpp << "        std::string base_url;\n";
  c_hpp << "        CURL* curl;\n\n";
  c_hpp << "        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) noexcept;\n\n";
  c_hpp << "    public:\n";
  
  std::string default_url = "";
  if (spec.servers.has_value() && !spec.servers->empty()) {
    default_url = spec.servers.value().front().url;
  }
  c_hpp << "        Client(const std::string& url = \"" << default_url << "\") noexcept;\n";
  c_hpp << "        ~Client() noexcept;\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method_decl = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value()) return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params;
        if (item.parameters) for (const auto &p : *item.parameters) all_params.push_back(p);
        if (op->parameters) for (const auto &p : *op->parameters) all_params.push_back(p);

        openapi::Operation new_op = op.value();
        new_op.parameters = all_params;
        c_hpp << docstrings::emit_path_docstrings(item);
        c_hpp << docstrings::emit_operation_docstrings(new_op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer") type = "int";
            else if (*p.schema->type == "boolean") type = "bool";
            else if (*p.schema->type == "number") type = "double";
          }
          if (i > 0) param_list += ", ";
          param_list += type + " " + p.name;
        }

        if (op->requestBody) {
          if (!param_list.empty()) param_list += ", ";
          param_list += "const std::string& body";
        }

        c_hpp << "        std::expected<std::string, std::string> " << func_name << "(" << param_list << ") noexcept;\n";
      };

      emit_method_decl(item.get);
      emit_method_decl(item.post);
      emit_method_decl(item.put);
      emit_method_decl(item.delete_op);
      emit_method_decl(item.patch);
    }
  }

  c_hpp << "    };\n";
  c_hpp << "}\n";
  result["client.hpp"] = c_hpp.str();

  // 4. client.cpp
  std::stringstream c_cpp;
  c_cpp << "#include \"client.hpp\"\n\n";
  c_cpp << "namespace cdd_client {\n\n";
  c_cpp << "    size_t Client::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) noexcept {\n";
  c_cpp << "        ((std::string*)userp)->append((char*)contents, size * nmemb);\n";
  c_cpp << "        return size * nmemb;\n";
  c_cpp << "    }\n\n";
  
  c_cpp << "    Client::Client(const std::string& url) noexcept : base_url(url) {\n";
  c_cpp << "        curl_global_init(CURL_GLOBAL_DEFAULT);\n";
  c_cpp << "        curl = curl_easy_init();\n";
  c_cpp << "    }\n\n";
  
  c_cpp << "    Client::~Client() noexcept {\n";
  c_cpp << "        if(curl) curl_easy_cleanup(curl);\n";
  c_cpp << "        curl_global_cleanup();\n";
  c_cpp << "    }\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method_impl = [&](const std::string &method, const std::optional<openapi::Operation> &op) {
        if (!op.has_value()) return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params;
        if (item.parameters) for (const auto &p : *item.parameters) all_params.push_back(p);
        if (op->parameters) for (const auto &p : *op->parameters) all_params.push_back(p);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer") type = "int";
            else if (*p.schema->type == "boolean") type = "bool";
            else if (*p.schema->type == "number") type = "double";
          }
          if (i > 0) param_list += ", ";
          param_list += type + " " + p.name;
        }

        if (op->requestBody) {
          if (!param_list.empty()) param_list += ", ";
          param_list += "const std::string& body";
        }

        c_cpp << "    std::expected<std::string, std::string> Client::" << func_name << "(" << param_list << ") noexcept {\n";
        c_cpp << "        if (!curl) return std::unexpected(\"Curl not initialized\");\n";
        c_cpp << "        std::string readBuffer;\n";
        c_cpp << "        std::string full_url = base_url + \"" << path << "\";\n";

        for (const auto &p : all_params) {
          if (p.in == "path") {
            c_cpp << "        full_url.replace(full_url.find(\"{" << p.name << "}\"), " << p.name.length() + 2 << ", std::to_string(" << p.name << "));\n";
          } else if (p.in == "query") {
            c_cpp << "        full_url += (full_url.find('?') == std::string::npos ? \"?\" : \"&\");\n";
            c_cpp << "        full_url += \"" << p.name << "=\" + std::to_string(" << p.name << ");\n";
          }
        }

        c_cpp << "        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());\n";
        if (method != "GET") {
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, \"" << method << "\");\n";
        }
        c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);\n";
        c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);\n";

        if (op->requestBody) {
          c_cpp << "        struct curl_slist *headers = NULL;\n";
          c_cpp << "        headers = curl_slist_append(headers, \"content-type: application/json\");\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());\n";
        }

        c_cpp << "        CURLcode res = curl_easy_perform(curl);\n";
        if (op->requestBody) {
          c_cpp << "        if(headers) curl_slist_free_all(headers);\n";
        }
        c_cpp << "        if(res != CURLE_OK) {\n";
        c_cpp << "            return std::unexpected(curl_easy_strerror(res));\n";
        c_cpp << "        }\n";
        c_cpp << "        return readBuffer;\n";
        c_cpp << "    }\n\n";
      };

      emit_method_impl("GET", item.get);
      emit_method_impl("POST", item.post);
      emit_method_impl("PUT", item.put);
      emit_method_impl("DELETE", item.delete_op);
      emit_method_impl("PATCH", item.patch);
    }
  }

  c_cpp << "}\n";
  result["client.cpp"] = c_cpp.str();

  return result;
}

} // namespace cdd_cpp::client_sdk
