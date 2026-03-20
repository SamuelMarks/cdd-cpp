#include "emit.hpp"
#include <sstream>
#include "../docstrings/emit.hpp"

namespace cdd_cpp::client_sdk {
std::string emit_client(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;

  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <string>\n";
  ss << "#include <optional>\n";
  ss << "#include <curl/curl.h>\n";
  ss << "#include <simdjson.h>\n\n";

  ss << "namespace cdd_client {\n\n";

  ss << docstrings::emit_api_docstrings(spec);
  ss << "    class Client {\n";
  ss << "        std::string base_url;\n";
  ss << "        CURL* curl;\n\n";

  ss << "        static size_t WriteCallback(void* contents, size_t size, "
        "size_t nmemb, void* userp) noexcept {\n";
  ss << "            ((std::string*)userp)->append((char*)contents, size * "
        "nmemb);\n";
  ss << "            return size * nmemb;\n";
  ss << "        }\n\n";

  ss << "    public:\n";

  std::string default_url = "";
  if (spec.servers.has_value() && !spec.servers->empty()) {
    default_url = spec.servers.value().front().url;
  }

  ss << "        Client(const std::string& url = \"" << default_url
     << "\") noexcept : base_url(url) {\n";
  ss << "            curl_global_init(CURL_GLOBAL_DEFAULT);\n";
  ss << "            curl = curl_easy_init();\n";
  ss << "        }\n\n";

  ss << "        ~Client() noexcept {\n";
  ss << "            if(curl) curl_easy_cleanup(curl);\n";
  ss << "            curl_global_cleanup();\n";
  ss << "        }\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method = [&](const std::string &method,
                             const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");

        ss << docstrings::emit_path_docstrings(item);
        
        // collect parameters from path and operation
        std::vector<openapi::Parameter> all_params;
        if (item.parameters) {
          for (const auto &p : *item.parameters) all_params.push_back(p);
        }
        if (op->parameters) {
          for (const auto &p : *op->parameters) all_params.push_back(p);
        }
        
        // Now emit operation docstrings and inject missing params
        openapi::Operation new_op = op.value();
        new_op.parameters = all_params;
        ss << docstrings::emit_operation_docstrings(new_op);

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

        ss << "        std::expected<std::string, std::string> " << func_name
           << "(" << param_list << ") noexcept {\n";
        ss << "            if (!curl) return std::unexpected(\"Curl not "
              "initialized\");\n";
        ss << "            std::string readBuffer;\n";
        ss << "            std::string full_url = base_url + \"" << path
           << "\";\n";
        
        // replace url parameters
        for (const auto &p : all_params) {
          if (p.in == "path") {
            ss << "            full_url.replace(full_url.find(\"{" << p.name << "}\"), " << p.name.length() + 2 << ", std::to_string(" << p.name << "));\n";
          } else if (p.in == "query") {
            ss << "            full_url += (full_url.find('?') == std::string::npos ? \"?\" : \"&\");\n";
            ss << "            full_url += \"" << p.name << "=\" + std::to_string(" << p.name << ");\n";
          }
        }
           
        ss << "            curl_easy_setopt(curl, CURLOPT_URL, "
              "full_url.c_str());\n";

        if (method != "GET") {
          ss << "            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, \""
             << method << "\");\n";
        }

        ss << "            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "
              "WriteCallback);\n";
        ss << "            curl_easy_setopt(curl, CURLOPT_WRITEDATA, "
              "&readBuffer);\n";
        
        if (op->requestBody) {
          ss << "            struct curl_slist *headers = NULL;\n";
          ss << "            headers = curl_slist_append(headers, \"content-type: application/json\");\n";
          ss << "            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);\n";
          ss << "            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());\n";
        }

        ss << "            CURLcode res = curl_easy_perform(curl);\n";
        if (op->requestBody) {
          ss << "            if(headers) curl_slist_free_all(headers);\n";
        }
        ss << "            if(res != CURLE_OK) {\n";
        ss << "                return "
              "std::unexpected(curl_easy_strerror(res));\n";
        ss << "            }\n";
        ss << "            return readBuffer;\n";
        ss << "        }\n\n";
      };

      emit_method("GET", item.get);
      emit_method("POST", item.post);
      emit_method("PUT", item.put);
      emit_method("DELETE", item.delete_op);
      emit_method("PATCH", item.patch);
    }
  }

  ss << "    };\n";
  ss << "}\n";

  return ss.str();
}
} // namespace cdd_cpp::client_sdk
