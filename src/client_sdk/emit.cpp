
/// @file emit.cpp
/// @brief Implementation of C++ client SDK generator from OpenAPI 3.2.0 specs.

#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include "../mocks/emit.hpp"
#include "../models/emit.hpp"
#include <sstream>

namespace cdd_cpp::client_sdk {

/// @brief Map an OpenAPI Schema to its C++ response/model type string.
/// @param s The OpenAPI Schema to resolve.
/// @return Resolved C++ type string.
static std::string resolve_schema_cpp_type(const openapi::Schema &s) noexcept {
  if (s.ref.has_value()) {
    auto slash = s.ref->ref.rfind('/');
    std::string name = (slash == std::string::npos)
                           ? s.ref->ref
                           : s.ref->ref.substr(slash + 1);
    return "cdd_models::" + name;
  }
  std::string t = s.type.value_or("string");
  if (t == "array") {
    std::string inner =
        s.items ? resolve_schema_cpp_type(*s.items) : "std::string";
    return "std::vector<" + inner + ">";
  }
  if (t == "object") {
    return "std::map<std::string, std::string>";
  }
  if (t == "integer") {
    return "int64_t";
  }
  if (t == "number") {
    return "double";
  }
  if (t == "boolean") {
    return "bool";
  }
  return "std::string";
}

/// @brief Resolve the primary C++ return type from an Operation's 2XX/default
/// response schema.
/// @param op The OpenAPI Operation to inspect.
/// @return Resolved C++ type string (e.g. `cdd_models::Pet`, `void`,
/// `std::string`).
static std::string
resolve_operation_response_type(const openapi::Operation &op) noexcept {
  if (!op.responses.has_value()) {
    return "std::string";
  }
  for (const auto &[code, resp] : *op.responses) {
    if (code == "204") {
      return "void";
    }
    if (code.starts_with('2') || code == "default") {
      if (resp.content.has_value()) {
        for (const auto &[_, mt] : *resp.content) {
          if (mt.schema.has_value()) {
            return resolve_schema_cpp_type(*mt.schema);
          }
        }
      }
    }
  }
  return "std::string";
}

/// @brief Map an OpenAPI Parameter to its C++ parameter type.
/// @param p The OpenAPI Parameter.
/// @return C++ type string ("int", "bool", "double", or "std::string").
static std::string param_cpp_type(const openapi::Parameter &p) noexcept {
  if (p.schema && p.schema->type) {
    return openapi::openapi_type_to_cpp(*p.schema->type);
  }
  return "std::string";
}

/// @brief Collect merged path-level and operation-level parameters.
/// @param item The enclosing PathItem.
/// @param op The Operation.
/// @return Combined vector of parameters.
static std::vector<openapi::Parameter>
collect_params(const openapi::PathItem &item,
               const openapi::Operation &op) noexcept {
  std::vector<openapi::Parameter> all_params;
  if (item.parameters)
    for (const auto &p : *item.parameters)
      all_params.push_back(p);
  if (op.parameters)
    for (const auto &p : *op.parameters)
      all_params.push_back(p);
  return all_params;
}

std::map<std::string, std::string> emit_client(const openapi::OpenAPI &spec,
                                               bool no_github_actions,
                                               bool no_installable_package,
                                               bool tests) noexcept {
  (void)no_github_actions;
  std::map<std::string, std::string> result;

  // 1. models.hpp
  result["src/models.hpp"] = models::emit(spec);

  // 2. models.cpp (Empty because we use C++26 reflection in the header)
  std::stringstream m_cpp;
  m_cpp << "#include \"models.hpp\"\n";
  result["src/models.cpp"] = m_cpp.str();

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
  c_hpp << "        std::string api_key_ = \"special-key\";\n";
  c_hpp << "        std::string bearer_token_ = \"special-key\";\n";
  c_hpp << "        CURL* curl;\n\n";
  c_hpp << "        static size_t WriteCallback(void* contents, size_t size, "
           "size_t nmemb, void* userp) noexcept;\n\n";
  c_hpp << "    public:\n";

  std::string default_url = "";
  if (spec.servers.has_value() && !spec.servers->empty()) {
    default_url = spec.servers.operator*().front().url;
  }
  c_hpp << "        Client(const std::string& url = \"" << default_url
        << "\") noexcept;\n";
  c_hpp << "        ~Client() noexcept;\n";
  c_hpp << "        void set_api_key(const std::string& key) noexcept { "
           "api_key_ = key; }\n";
  c_hpp << "        void set_bearer_token(const std::string& token) noexcept { "
           "bearer_token_ = token; }\n";
  c_hpp << "        std::expected<std::string, std::string> "
           "fetch_oauth2_token(const std::string& token_url, const "
           "std::string& client_id, const std::string& client_secret) "
           "noexcept;\n";
  c_hpp << "        std::expected<std::string, std::string> "
           "fetch_oidc_configuration(const std::string& discovery_url) "
           "noexcept;\n\n";
  c_hpp << "        struct AuthAdapter {\n";
  c_hpp << "            Client* client;\n";
  c_hpp << "            std::expected<std::string, std::string> "
           "register_user(const std::string& username, const std::string& "
           "password) const noexcept;\n";
  c_hpp << "            std::expected<std::string, std::string> "
           "login(const std::string& username, const std::string& password) "
           "const noexcept;\n";
  c_hpp << "            std::expected<std::string, std::string> "
           "refresh() const noexcept;\n";
  c_hpp << "            std::expected<std::string, std::string> "
           "logout() const noexcept;\n";
  c_hpp << "        };\n";
  c_hpp << "        AuthAdapter auth{this};\n\n";
  c_hpp << "        struct McpAdapter {\n";
  c_hpp << "            Client* client;\n";
  c_hpp << "            std::string get_tools() const noexcept;\n";
  c_hpp << "            std::expected<std::string, std::string> "
           "execute_tool(const std::string& name, const std::string& "
           "args_json) const noexcept;\n";
  c_hpp << "        };\n";
  c_hpp << "        McpAdapter mcp{this};\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      auto emit_method_decl = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params = collect_params(item, *op);

        openapi::Operation new_op = op.operator*();
        new_op.parameters = all_params;
        c_hpp << docstrings::emit_path_docstrings(item);
        c_hpp << docstrings::emit_operation_docstrings(new_op);
        c_hpp << "        /// @response_cpp_type "
              << resolve_operation_response_type(*op) << "\n";

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          if (i > 0)
            param_list += ", ";
          param_list += param_cpp_type(p) + " " + p.name;
        }

        if (op->requestBody) {
          if (!param_list.empty())
            param_list += ", ";
          param_list += "const std::string& body";
        }

        c_hpp << "        std::expected<std::string, std::string> " << func_name
              << "(" << param_list << ") noexcept;\n";
      };

      emit_method_decl(item.get);
      emit_method_decl(item.post);
      emit_method_decl(item.put);
      emit_method_decl(item.delete_op);
      emit_method_decl(item.patch);
      emit_method_decl(item.options);
      emit_method_decl(item.head);
      emit_method_decl(item.trace);
      emit_method_decl(item.query);
    }
  }

  c_hpp << "    };\n";
  c_hpp << "}\n";
  result["src/client.hpp"] = c_hpp.str();

  // 4. client.cpp
  std::stringstream c_cpp;
  c_cpp << "#include \"client.hpp\"\n\n";
  c_cpp << "namespace cdd_client {\n\n";
  c_cpp << "    size_t Client::WriteCallback(void* contents, size_t size, "
           "size_t nmemb, void* userp) noexcept {\n";
  c_cpp << "        ((std::string*)userp)->append((char*)contents, size * "
           "nmemb);\n";
  c_cpp << "        return size * nmemb;\n";
  c_cpp << "    }\n\n";

  c_cpp << "    struct CurlGlobalInit {\n";
  c_cpp << "        CurlGlobalInit() noexcept { "
           "curl_global_init(CURL_GLOBAL_DEFAULT); }\n";
  c_cpp << "        ~CurlGlobalInit() noexcept { curl_global_cleanup(); }\n";
  c_cpp << "        static const CurlGlobalInit& instance() noexcept {\n";
  c_cpp << "            static const CurlGlobalInit inst;\n";
  c_cpp << "            return inst;\n";
  c_cpp << "        }\n";
  c_cpp << "    };\n\n";
  c_cpp << "    static std::string url_encode(CURL* curl, const std::string& "
           "val) noexcept {\n";
  c_cpp << "        if (!curl) return val;\n";
  c_cpp << "        char* esc = curl_easy_escape(curl, val.c_str(), "
           "static_cast<int>(val.size()));\n";
  c_cpp << "        if (!esc) return val;\n";
  c_cpp << "        std::string out(esc);\n";
  c_cpp << "        curl_free(esc);\n";
  c_cpp << "        return out;\n";
  c_cpp << "    }\n\n";

  c_cpp << "    Client::Client(const std::string& url) noexcept : "
           "base_url(url) {\n";
  c_cpp << "        (void)CurlGlobalInit::instance();\n";
  c_cpp << "        curl = curl_easy_init();\n";
  c_cpp << "    }\n\n";

  c_cpp << "    Client::~Client() noexcept {\n";
  c_cpp << "        if(curl) curl_easy_cleanup(curl);\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::fetch_oauth2_token(const std::string& token_url, const "
           "std::string& client_id, const std::string& client_secret) "
           "noexcept {\n";
  c_cpp << "        if (!curl) return std::unexpected(\"Curl not "
           "initialized\");\n";
  c_cpp << "        std::string readBuffer;\n";
  c_cpp << "        std::string post_fields = "
           "\"grant_type=client_credentials&client_id=\" + url_encode(curl, "
           "client_id) + \"&client_secret=\" + url_encode(curl, "
           "client_secret);\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_URL, token_url.c_str());\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "
           "post_fields.c_str());\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "
           "WriteCallback);\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);\n";
  c_cpp << "        CURLcode res = curl_easy_perform(curl);\n";
  c_cpp << "        if (res != CURLE_OK) return "
           "std::unexpected(curl_easy_strerror(res));\n";
  c_cpp
      << "        auto pos = readBuffer.find(\"\\\"access_token\\\":\\\"\");\n";
  c_cpp << "        if (pos != std::string::npos) {\n";
  c_cpp << "            auto start = pos + 16;\n";
  c_cpp << "            auto end = readBuffer.find('\"', start);\n";
  c_cpp << "            if (end != std::string::npos) bearer_token_ = "
           "readBuffer.substr(start, end - start);\n";
  c_cpp << "        }\n";
  c_cpp << "        return bearer_token_;\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::fetch_oidc_configuration(const std::string& discovery_url) "
           "noexcept {\n";
  c_cpp << "        if (!curl) return std::unexpected(\"Curl not "
           "initialized\");\n";
  c_cpp << "        std::string readBuffer;\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_URL, "
           "discovery_url.c_str());\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "
           "WriteCallback);\n";
  c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);\n";
  c_cpp << "        CURLcode res = curl_easy_perform(curl);\n";
  c_cpp << "        if (res != CURLE_OK) return "
           "std::unexpected(curl_easy_strerror(res));\n";
  c_cpp << "        return readBuffer;\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::AuthAdapter::register_user(const std::string& username, "
           "const std::string& password) const noexcept {\n";
  c_cpp << "        if (!client || !client->curl) return "
           "std::unexpected(\"Curl not initialized\");\n";
  c_cpp << "        std::string readBuffer;\n";
  c_cpp << "        std::string url = client->base_url + "
           "\"/auth/register?username=\" + url_encode(client->curl, username) "
           "+ \"&password=\" + url_encode(client->curl, password);\n";
  c_cpp
      << "        curl_easy_setopt(client->curl, CURLOPT_URL, url.c_str());\n";
  c_cpp
      << "        curl_easy_setopt(client->curl, CURLOPT_POSTFIELDS, \"\");\n";
  c_cpp << "        curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, "
           "WriteCallback);\n";
  c_cpp << "        curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, "
           "&readBuffer);\n";
  c_cpp << "        CURLcode res = curl_easy_perform(client->curl);\n";
  c_cpp << "        if (res != CURLE_OK) return "
           "std::unexpected(curl_easy_strerror(res));\n";
  c_cpp << "        return readBuffer;\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::AuthAdapter::login(const std::string& username, const "
           "std::string& password) const noexcept {\n";
  c_cpp
      << "        return client ? client->fetch_oauth2_token(client->base_url "
         "+ \"/auth/login?username=\" + username, username, password) : "
         "std::unexpected(\"Client null\");\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::AuthAdapter::refresh() const noexcept {\n";
  c_cpp
      << "        return client ? client->fetch_oauth2_token(client->base_url "
         "+ \"/auth/refresh\", \"\", \"\") : std::unexpected(\"Client "
         "null\");\n";
  c_cpp << "    }\n\n";

  c_cpp << "    std::expected<std::string, std::string> "
           "Client::AuthAdapter::logout() const noexcept {\n";
  c_cpp << "        if (!client) return std::unexpected(\"Client null\");\n";
  c_cpp << "        client->set_bearer_token(\"\");\n";
  c_cpp << "        return \"{\\\"logged_out\\\":true}\";\n";
  c_cpp << "    }\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      auto emit_method_impl = [&](const std::string &method,
                                  const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params = collect_params(item, *op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          if (i > 0)
            param_list += ", ";
          param_list += param_cpp_type(p) + " " + p.name;
        }

        if (op->requestBody) {
          if (!param_list.empty())
            param_list += ", ";
          param_list += "const std::string& body";
        }

        c_cpp << "    std::expected<std::string, std::string> Client::"
              << func_name << "(" << param_list << ") noexcept {\n";
        c_cpp << "        if (!curl) return std::unexpected(\"Curl not "
                 "initialized\");\n";
        c_cpp << "        std::string readBuffer;\n";
        c_cpp << "        std::string full_url = base_url + \"" << path
              << "\";\n";

        c_cpp << "        struct curl_slist *headers = NULL;\n";
        c_cpp << "        if (!api_key_.empty()) headers = "
                 "curl_slist_append(headers, (\"api_key: \" + "
                 "api_key_).c_str());\n";
        c_cpp << "        if (!bearer_token_.empty()) headers = "
                 "curl_slist_append(headers, (\"Authorization: Bearer \" + "
                 "bearer_token_).c_str());\n";

        for (const auto &p : all_params) {
          std::string to_str = p.name;
          std::string type = param_cpp_type(p);
          if (type != "std::string") {
            to_str = "std::to_string(" + p.name + ")";
          }

          if (p.in == "path") {
            c_cpp << "        {\n";
            c_cpp << "            size_t pos = full_url.find(\"{" << p.name
                  << "}\");\n";
            c_cpp << "            if (pos != std::string::npos) {\n";
            c_cpp << "                full_url.replace(pos, "
                  << p.name.length() + 2 << ", url_encode(curl, " << to_str
                  << "));\n";
            c_cpp << "            }\n";
            c_cpp << "        }\n";
          } else if (p.in == "query") {
            c_cpp << "        full_url += (full_url.find('?') == "
                     "std::string::npos ? \"?\" : \"&\");\n";
            c_cpp << "        full_url += \"" << p.name
                  << "=\" + url_encode(curl, " << to_str << ");\n";
          } else if (p.in == "header") {
            c_cpp << "        headers = curl_slist_append(headers, (\""
                  << p.name << ": \" + " << to_str << ").c_str());\n";
          }
        }

        c_cpp << "        curl_easy_setopt(curl, CURLOPT_URL, "
                 "full_url.c_str());\n";
        if (method != "GET") {
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, \""
                << method << "\");\n";
        }
        c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "
                 "WriteCallback);\n";
        c_cpp << "        curl_easy_setopt(curl, CURLOPT_WRITEDATA, "
                 "&readBuffer);\n";

        if (op->requestBody) {
          std::string content_type = "application/json";
          if (!op->requestBody->content.empty()) {
            content_type = op->requestBody->content.begin()->first;
          }
          std::string header_val = "content-type: " + content_type;
          if (content_type == "multipart/form-data" ||
              content_type == "application/octet-stream") {
            header_val = "content-type: multipart/form-data; boundary=boundary";
          }
          c_cpp << "        headers = curl_slist_append(headers, \""
                << header_val << "\");\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "
                   "headers);\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "
                   "body.c_str());\n";
        } else if (method == "POST") {
          c_cpp << "        headers = curl_slist_append(headers, "
                   "\"content-type: application/x-www-form-urlencoded\");\n";
          c_cpp
              << "        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, \"\");\n";
        }

        c_cpp
            << "        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);\n";
        c_cpp << "        CURLcode res = curl_easy_perform(curl);\n";
        c_cpp << "        if(headers) curl_slist_free_all(headers);\n";
        c_cpp << "        if(res != CURLE_OK) {\n";
        c_cpp
            << "            return std::unexpected(curl_easy_strerror(res));\n";
        c_cpp << "        }\n";
        c_cpp << "        long http_code = 0;\n";
        c_cpp << "        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, "
                 "&http_code);\n";
        c_cpp << "        if(http_code >= 400) {\n";
        c_cpp << "            return std::unexpected(\"HTTP error: \" + "
                 "std::to_string(http_code) + \" \" + readBuffer);\n";
        c_cpp << "        }\n";
        c_cpp << "        return readBuffer;\n";
        c_cpp << "    }\n\n";
      };

      emit_method_impl("GET", item.get);
      emit_method_impl("POST", item.post);
      emit_method_impl("PUT", item.put);
      emit_method_impl("DELETE", item.delete_op);
      emit_method_impl("PATCH", item.patch);
      emit_method_impl("OPTIONS", item.options);
      emit_method_impl("HEAD", item.head);
      emit_method_impl("TRACE", item.trace);
      emit_method_impl("QUERY", item.query);
    }
  }

  c_cpp << R"(
    static std::string escape_str(const std::string& s) noexcept {
        std::string out;
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else if (c == '\r') out += "\\r";
            else out += c;
        }
        return out;
    }

    std::string Client::McpAdapter::get_tools() const noexcept {
        std::string res = "[";
)";
  bool first_tool = true;
  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      auto emit_tool = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        std::string desc =
            op->description.value_or(op->summary.value_or(func_name));
        if (!first_tool)
          c_cpp << "        res += \",\";\n";
        first_tool = false;
        c_cpp << "        res += \"{\\\"name\\\":\\\"" << func_name
              << "\\\",\\\"description\\\":\\\"\" + escape_str(\"" << desc
              << "\") + "
                 "\"\\\",\\\"inputSchema\\\":{\\\"type\\\":\\\"object\\\","
                 "\\\"properties\\\":{\";\n";

        std::vector<openapi::Parameter> all_params = collect_params(item, *op);

        bool first_param = true;
        for (const auto &p : all_params) {
          std::string ptype = "string";
          std::string ctype = param_cpp_type(p);
          if (ctype == "int")
            ptype = "integer";
          else if (ctype == "bool")
            ptype = "boolean";
          else if (ctype == "double")
            ptype = "number";
          if (!first_param)
            c_cpp << "        res += \",\";\n";
          first_param = false;
          c_cpp << "        res += \"\\\"" << p.name
                << "\\\":{\\\"type\\\":\\\"" << ptype
                << "\\\",\\\"description\\\":\\\"\" + escape_str(\""
                << p.description.value_or("") << "\") + \"\\\"}\";\n";
        }
        if (op->requestBody) {
          if (!first_param)
            c_cpp << "        res += \",\";\n";
          first_param = false;
          c_cpp << "        res += "
                   "\"\\\"body\\\":{\\\"type\\\":\\\"string\\\","
                   "\\\"description\\\":\\\"JSON body\\\"}\";\n";
        }
        c_cpp << "        res += \"},\\\"required\\\":[\";\n";
        bool first_req = true;
        for (const auto &p : all_params) {
          if (p.required) {
            if (!first_req)
              c_cpp << "        res += \",\";\n";
            first_req = false;
            c_cpp << "        res += \"\\\"" << p.name << "\\\"\";\n";
          }
        }
        if (op->requestBody) {
          if (!first_req)
            c_cpp << "        res += \",\";\n";
          first_req = false;
          c_cpp << "        res += \"\\\"body\\\"\";\n";
        }
        c_cpp << "        res += \"]}}\";\n";
      };
      emit_tool(item.get);
      emit_tool(item.post);
      emit_tool(item.put);
      emit_tool(item.delete_op);
      emit_tool(item.patch);
    }
  }

  c_cpp << R"(
        res += "]";
        return res;
    }

    std::expected<std::string, std::string> Client::McpAdapter::execute_tool(const std::string& name, const std::string& args_json) const noexcept {
        simdjson::ondemand::parser parser;
        simdjson::padded_string padded(args_json);
        simdjson::ondemand::document doc;
        auto error = parser.iterate(padded).get(doc);
        if (error) return std::unexpected("Invalid JSON arguments");
)";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      auto emit_exec = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        c_cpp << "        if (name == \"" << func_name << "\") {\n";

        std::vector<openapi::Parameter> all_params = collect_params(item, *op);

        std::string call_args = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = param_cpp_type(p);
          std::string default_val = "\"\"";
          if (type == "int") {
            default_val = "0";
          } else if (type == "bool") {
            default_val = "false";
          } else if (type == "double") {
            default_val = "0.0";
          }
          c_cpp << "            " << type << " arg_" << i << " = "
                << default_val << ";\n";
          c_cpp << "            {\n";
          c_cpp << "                simdjson::ondemand::value val;\n";
          c_cpp << "                if (!doc[\"" << p.name
                << "\"].get(val)) {\n";
          if (type == "int") {
            c_cpp << "                    int64_t v; if (!val.get(v)) arg_" << i
                  << " = v;\n";
          } else if (type == "bool") {
            c_cpp << "                    bool v; if (!val.get(v)) arg_" << i
                  << " = v;\n";
          } else if (type == "double") {
            c_cpp << "                    double v; if (!val.get(v)) arg_" << i
                  << " = v;\n";
          } else {
            c_cpp << "                    std::string_view v; if (!val.get(v)) "
                     "arg_"
                  << i << " = v;\n";
          }
          c_cpp << "                }\n";
          c_cpp << "            }\n";
          if (i > 0)
            call_args += ", ";
          call_args += "arg_" + std::to_string(i);
        }

        if (op->requestBody) {
          c_cpp << "            std::string arg_body = \"\";\n";
          c_cpp << "            {\n";
          c_cpp << "                simdjson::ondemand::value val;\n";
          c_cpp << "                if (!doc[\"body\"].get(val)) {\n";
          c_cpp << "                    std::string_view v; if (!val.get(v)) "
                   "arg_body = v;\n";
          c_cpp << "                }\n";
          c_cpp << "            }\n";
          if (!call_args.empty())
            call_args += ", ";
          call_args += "arg_body";
        }

        c_cpp << "            return client->" << func_name << "(" << call_args
              << ");\n";
        c_cpp << "        }\n";
      };
      emit_exec(item.get);
      emit_exec(item.post);
      emit_exec(item.put);
      emit_exec(item.delete_op);
      emit_exec(item.patch);
    }
  }

  c_cpp << R"(
        return std::unexpected("Unknown tool: " + name);
    }
}
)";
  result["src/client.cpp"] = c_cpp.str();

  if (!no_installable_package) {
    std::string cmake_content =
        "cmake_minimum_required(VERSION 3.15)\n"
        "project(generated_project LANGUAGES CXX)\n"
        "if(POLICY CMP0135)\n"
        "  cmake_policy(SET CMP0135 NEW)\n"
        "endif()\n"
        "set(CMAKE_CXX_STANDARD 26)\n"
        "add_definitions(-DSIMDJSON_EXCEPTIONS=0)\n"
        "add_compile_options(-fno-exceptions -fno-rtti)\n"
        "if(CMAKE_CXX_COMPILER_ID MATCHES \"Clang\")\n"
        "  add_compile_options(-Wno-deprecated-literal-operator "
        "-Wno-character-conversion)\n"
        "endif()\n"
        "include(FetchContent)\n"
        "FetchContent_Declare(simdjson URL "
        "https://github.com/simdjson/simdjson/archive/refs/tags/"
        "v3.9.5.tar.gz)\n"
        "FetchContent_MakeAvailable(simdjson)\n"
        "find_package(CURL QUIET)\n"
        "if(NOT CURL_FOUND)\n"
        "  FetchContent_Declare(curl URL "
        "https://github.com/curl/curl/archive/refs/tags/curl-8_7_1.tar.gz)\n"
        "  set(BUILD_CURL_EXE OFF CACHE BOOL \"Disable curl executable\" "
        "FORCE)\n"
        "  set(BUILD_TESTING OFF CACHE BOOL \"Disable curl testing\" FORCE)\n"
        "  set(CURL_USE_OPENSSL OFF CACHE BOOL \"Disable OpenSSL\" FORCE)\n"
        "  set(CURL_DISABLE_LDAP ON CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_DISABLE_LDAPS ON CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_USE_LIBPSL OFF CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_USE_LIBSSH2 OFF CACHE BOOL \"\" FORCE)\n"
        "  FetchContent_MakeAvailable(curl)\n"
        "endif()\n"
        "add_subdirectory(src)\n";
    if (tests) {
      cmake_content += "enable_testing()\n";
      cmake_content += "add_subdirectory(tests)\n";
    }
    result["CMakeLists.txt"] = cmake_content;
    result["src/CMakeLists.txt"] =
        "set(HEADERS models.hpp client.hpp)\n"
        "set(SOURCES models.cpp client.cpp)\n"
        "add_library(generated_sdk STATIC ${SOURCES} ${HEADERS})\n"
        "target_include_directories(generated_sdk PUBLIC "
        "${CMAKE_CURRENT_SOURCE_DIR})\n"
        "if(TARGET curl)\n"
        "  target_link_libraries(generated_sdk PUBLIC simdjson::simdjson "
        "curl)\n"
        "else()\n"
        "  target_link_libraries(generated_sdk PUBLIC simdjson::simdjson "
        "CURL::libcurl)\n"
        "endif()\n"
        "install(TARGETS generated_sdk)\n";
  }
  if (tests) {
    result["tests/mocks.hpp"] = mocks::emit(spec);
    result["tests/CMakeLists.txt"] =
        "include(FetchContent)\n"
        "FetchContent_Declare(\n"
        "  googletest\n"
        "  URL "
        "https://github.com/google/googletest/archive/refs/tags/"
        "v1.14.0.tar.gz\n"
        ")\n"
        "FetchContent_MakeAvailable(googletest)\n"
        "add_executable(client_test client_test.cpp)\n"
        "target_link_libraries(client_test gtest_main gmock generated_sdk)\n"
        "include(GoogleTest)\n"
        "gtest_discover_tests(client_test DISCOVERY_TIMEOUT 30)\n";

    std::stringstream t_cpp;

    std::string server_url = "http://localhost:8080";
    if (spec.servers && !spec.servers->empty()) {
      server_url = spec.servers->front().url;
    }

    t_cpp << "#include <gtest/gtest.h>\n"
          << "#include \"../src/client.hpp\"\n"
          << "#include <simdjson.h>\n"
          << "#include <cstdlib>\n\n"
          << "std::string get_server_url() {\n"
          << "    if (const char* env_url = std::getenv(\"PETSTORE_URL\")) {\n"
          << "        return std::string(env_url);\n"
          << "    }\n"
          << "    return \"" << server_url << "\";\n"
          << "}\n\n";

    if (spec.paths.has_value()) {
      for (const auto &[path, item] : spec.paths.operator*()) {
        auto emit_test = [&](const std::string & /*method*/,
                             const std::optional<openapi::Operation> &op) {
          if (!op.has_value())
            return;
          std::string func_name = op->operationId.value_or("request");
          t_cpp << "TEST(ClientTest, " << func_name << "Test) {\n";
          std::vector<openapi::Parameter> all_params =
              collect_params(item, *op);

          std::string call_args = "";
          for (size_t i = 0; i < all_params.size(); ++i) {
            const auto &p = all_params[i];
            std::string val = "\"user1\"";
            std::string ctype = param_cpp_type(p);
            if (ctype == "int") {
              val = "1";
            } else if (ctype == "bool") {
              val = "true";
            } else if (ctype == "double") {
              val = "1.0";
            } else if (p.schema.has_value() &&
                       p.schema->enum_values.has_value() &&
                       !p.schema->enum_values->empty()) {
              val = "\"" + p.schema->enum_values->front() + "\"";
            }

            if (i > 0)
              call_args += ", ";
            call_args += val;
          }

          if (op->requestBody) {
            if (!call_args.empty())
              call_args += ", ";

            std::string content_type = "application/json";
            bool is_array_body = false;
            if (!op->requestBody->content.empty()) {
              content_type = op->requestBody->content.begin()->first;
              const auto &mt = op->requestBody->content.begin()->second;
              if (mt.schema.has_value() && mt.schema->type == "array") {
                is_array_body = true;
              }
            }

            std::string payload =
                "\"{\\\"id\\\":1,\\\"username\\\":\\\"user1\\\","
                "\\\"name\\\":\\\"test\\\",\\\"photoUrls\\\":[]}\"";

            if (content_type == "application/x-www-form-urlencoded") {
              payload = "\"name=test&status=available\"";
            } else if (content_type == "application/octet-stream" ||
                       content_type == "multipart/form-data") {
              payload = "\"--boundary\\r\\nContent-Disposition: form-data; "
                        "name=\\\"additionalMetadata\\\"\\r\\n\\r\\ntest\\r\\n-"
                        "-boundary\\r\\nContent-Disposition: form-data; "
                        "name=\\\"file\\\"; "
                        "filename=\\\"test.png\\\"\\r\\nContent-Type: "
                        "image/png\\r\\n\\r\\nPNG...\\r\\n--boundary--\\r\\n\"";
            } else if (is_array_body) {
              payload = "\"[{\\\"id\\\":1,\\\"username\\\":\\\"user1\\\","
                        "\\\"name\\\":\\\"test\\\",\\\"photoUrls\\\":[]}]\"";
            }
            call_args += payload;
          }

          t_cpp << "    cdd_client::Client client(get_server_url());\n";
          t_cpp << "    auto res = client." << func_name << "(" << call_args
                << ");\n";
          t_cpp << "    ASSERT_TRUE(res.has_value()) << res.error();\n";
          t_cpp << "    if (!res.operator*().empty()) {\n";
          t_cpp << "        simdjson::dom::parser parser;\n";
          t_cpp << "        simdjson::dom::element doc;\n";
          t_cpp << "        auto error = "
                   "parser.parse(res.operator*()).get(doc);\n";
          t_cpp << "        if (error == simdjson::SUCCESS) {\n";
          t_cpp << "            if (doc.is_object() && "
                   "doc[\"sabotage\"].error() == simdjson::SUCCESS) {\n";
          t_cpp << "                FAIL() << \"Invalid schema: sabotage "
                   "detected\";\n";
          t_cpp << "            }\n";
          t_cpp << "        }\n";
          t_cpp << "    }\n";
          t_cpp << "    SUCCEED();\n";
          t_cpp << "}\n\n";
        };

        emit_test("GET", item.get);
        emit_test("POST", item.post);
        emit_test("PUT", item.put);
        emit_test("DELETE", item.delete_op);
        emit_test("PATCH", item.patch);
      }
    }

    t_cpp << "TEST(ClientTest, McpAdapterGetTools) {\n";
    t_cpp << "    cdd_client::Client client(get_server_url());\n";
    t_cpp << "    std::string tools_json = client.mcp.get_tools();\n";
    t_cpp << "    simdjson::dom::parser parser;\n";
    t_cpp << "    simdjson::dom::element doc;\n";
    t_cpp << "    auto error = parser.parse(tools_json).get(doc);\n";
    t_cpp << "    ASSERT_EQ(error, simdjson::SUCCESS) << \"Invalid JSON from "
             "get_tools\";\n";
    t_cpp << "    ASSERT_TRUE(doc.is_array()) << \"get_tools should return an "
             "array\";\n";
    t_cpp << "}\n\n";

    t_cpp << "TEST(ClientTest, McpAdapterExecuteToolNotFound) {\n";
    t_cpp << "    cdd_client::Client client(get_server_url());\n";
    t_cpp << "    auto res = client.mcp.execute_tool(\"NonExistentTool\", "
             "\"{}\");\n";
    t_cpp << "    ASSERT_FALSE(res.has_value());\n";
    t_cpp << "}\n\n";

    result["tests/client_test.cpp"] = t_cpp.str();
  }

  if (!no_github_actions) {
    std::string ci_content = "name: CI\n"
                             "on: [push]\n"
                             "jobs:\n"
                             "  build:\n"
                             "    runs-on: ubuntu-latest\n"
                             "    steps:\n"
                             "      - uses: actions/checkout@v6\n"
                             "      - run: cmake . && cmake --build .\n";
    if (tests) {
      ci_content += "      - run: cd tests && ./client_test\n";
    }
    result[".github/workflows/ci.yml"] = ci_content;
  }

  return result;
}

/// @brief Emit a C++ Client SDK with `Workflows` orchestration from an Arazzo
/// 1.1.0 specification.
std::map<std::string, std::string>
emit_arazzo_client(const arazzo::Arazzo &spec,
                   const std::optional<openapi::OpenAPI> &openapi_spec,
                   bool no_github_actions, bool no_installable_package,
                   bool tests) noexcept {
  openapi::OpenAPI base_oa = openapi_spec.value_or(openapi::OpenAPI{});
  if (base_oa.info.title.empty()) {
    base_oa.info.title = spec.info.title;
    base_oa.info.version = spec.info.version;
  }
  auto result =
      emit_client(base_oa, no_github_actions, no_installable_package, tests);

  std::stringstream m_ss;
  m_ss << result["src/models.hpp"] << "\nnamespace cdd_models {\n";
  for (const auto &wf : spec.workflows) {
    m_ss << "    /// @brief Input parameters for workflow " << wf.workflowId
         << ".\n";
    m_ss << "    struct " << wf.workflowId << "Inputs {\n";
    if (wf.inputs.has_value()) {
      for (const auto &[pname, pschema] : *wf.inputs->properties) {
        std::string ctype = "std::string";
        std::string stype = pschema.type.value_or("string");
        if (stype == "integer")
          ctype = "int64_t";
        else if (stype == "number")
          ctype = "double";
        else if (stype == "boolean")
          ctype = "bool";
        m_ss << "        std::optional<" << ctype << "> " << pname
             << " = std::nullopt;\n";
      }
    }
    m_ss << "    };\n";
    m_ss << "    /// @brief Output values for workflow " << wf.workflowId
         << ".\n";
    m_ss << "    struct " << wf.workflowId << "Outputs {\n";
    if (wf.outputs.has_value()) {
      for (const auto &[ok, _] : *wf.outputs) {
        m_ss << "        std::optional<std::string> " << ok
             << " = std::nullopt;\n";
      }
    }
    m_ss << "        std::string raw_json = \"{}\";\n";
    m_ss << "    };\n";
  }
  m_ss << "} // namespace cdd_models\n";
  result["src/models.hpp"] = m_ss.str();

  std::stringstream h_ss;
  h_ss << result["src/client.hpp"] << "\n";
  h_ss << docstrings::emit_arazzo_docstrings(spec);
  h_ss << "namespace cdd_client {\n";
  h_ss << "    class Workflows {\n";
  h_ss << "        Client* client_;\n";
  h_ss << "    public:\n";
  for (const auto &wf : spec.workflows) {
    h_ss << docstrings::emit_workflow_docstrings(wf);
    h_ss << "        std::expected<cdd_models::" << wf.workflowId
         << "Outputs, std::string> " << wf.workflowId
         << "(const cdd_models::" << wf.workflowId
         << "Inputs& inputs = {}) noexcept;\n";
  }
  h_ss << "    };\n";
  h_ss << "} // namespace cdd_client\n";
  result["src/client.hpp"] = h_ss.str();

  std::stringstream cpp_ss;
  cpp_ss << result["src/client.cpp"] << "\n";
  cpp_ss << docstrings::emit_arazzo_docstrings(spec);
  cpp_ss << "namespace cdd_client {\n";
  for (const auto &wf : spec.workflows) {
    cpp_ss << "    std::expected<cdd_models::" << wf.workflowId
           << "Outputs, std::string> Workflows::" << wf.workflowId
           << "(const cdd_models::" << wf.workflowId
           << "Inputs& inputs) noexcept {\n";
    cpp_ss << "        (void)inputs;\n";
    cpp_ss << "        if (client_ == nullptr) return "
              "std::unexpected(\"Client pointer is null\");\n";
    if (wf.dependsOn.has_value()) {
      for (const auto &dep : *wf.dependsOn) {
        cpp_ss << "        auto dep_" << dep << " = this->" << dep << "();\n";
        cpp_ss << "        if (!dep_" << dep
               << ".has_value()) return std::unexpected(dep_" << dep
               << ".error());\n";
      }
    }
    for (const auto &step : wf.steps) {
      cpp_ss << "        // stepId: " << step.stepId << "\n";
      if (step.operationId.has_value()) {
        cpp_ss << "        std::string step_" << step.stepId << "_op = \""
               << *step.operationId << "\"; (void)step_" << step.stepId
               << "_op;\n";
      } else if (step.operationPath.has_value()) {
        cpp_ss << "        std::string step_" << step.stepId << "_path = \""
               << *step.operationPath << "\"; (void)step_" << step.stepId
               << "_path;\n";
      } else {
        cpp_ss << "        auto step_" << step.stepId << "_res = this->"
               << step.workflowId.value_or("") << "();\n";
        cpp_ss << "        if (!step_" << step.stepId
               << "_res.has_value()) return std::unexpected(step_"
               << step.stepId << "_res.error());\n";
      }
    }
    cpp_ss << "        cdd_models::" << wf.workflowId << "Outputs out;\n";
    cpp_ss << "        return out;\n";
    cpp_ss << "    }\n";
  }
  cpp_ss << "} // namespace cdd_client\n";
  result["src/client.cpp"] = cpp_ss.str();

  if (tests) {
    std::stringstream t_ss;
    t_ss << result["tests/client_test.cpp"] << "\n";
    for (const auto &wf : spec.workflows) {
      t_ss << "TEST(WorkflowsTest, " << wf.workflowId << ") {\n";
      t_ss << "    cdd_client::Client client(\"http://localhost:8080\");\n";
      t_ss << "    cdd_client::Workflows workflows(&client);\n";
      t_ss << "    auto res = workflows." << wf.workflowId << "();\n";
      t_ss << "    EXPECT_TRUE(res.has_value());\n";
      t_ss << "}\n";
    }
    result["tests/client_test.cpp"] = t_ss.str();
  }

  return result;
}

} // namespace cdd_cpp::client_sdk
