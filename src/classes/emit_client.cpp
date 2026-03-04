#include "emit_client.hpp"
#include <sstream>

namespace cdd_cpp::classes {
std::string emit_client(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;

  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <string>\n";
  ss << "#include <optional>\n";
  ss << "#include <curl/curl.h>\n";
  ss << "#include <simdjson.h>\n\n";

  ss << "namespace cdd_client {\n\n";

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

        ss << "        std::expected<std::string, std::string> " << func_name
           << "() noexcept {\n";
        ss << "            if (!curl) return std::unexpected(\"Curl not "
              "initialized\");\n";
        ss << "            std::string readBuffer;\n";
        ss << "            std::string full_url = base_url + \"" << path
           << "\";\n";
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
        ss << "            CURLcode res = curl_easy_perform(curl);\n";
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
} // namespace cdd_cpp::classes
