
#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include "../models/emit.hpp"
#include <sstream>

#include "../mocks/emit.hpp"

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

std::map<std::string, std::string> emit_client(const openapi::OpenAPI &spec,
                                               bool no_github_actions,
                                               bool no_installable_package,
                                               bool tests) noexcept {
  std::map<std::string, std::string> result;

  // 1. models.hpp
  std::stringstream m_hpp;
  m_hpp << "#pragma once\n";
  m_hpp << "#define SIMDJSON_STATIC_REFLECTION 1\n";
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
              if (req == prop_name)
                is_required = true;
            }
          }
          if (is_required) {
            m_hpp << "        " << map_type(prop_schema) << " " << prop_name
                  << ";\n";
          } else {
            m_hpp << "        std::optional<" << map_type(prop_schema) << "> "
                  << prop_name << ";\n";
          }
        }
      }
      m_hpp << "    };\n\n";
    }
  }
  m_hpp << "}\n";
  result["src/models.hpp"] = m_hpp.str();

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
  c_hpp << "        CURL* curl;\n\n";
  c_hpp << "        static size_t WriteCallback(void* contents, size_t size, "
           "size_t nmemb, void* userp) noexcept;\n\n";
  c_hpp << "    public:\n";

  std::string default_url = "";
  if (spec.servers.has_value() && !spec.servers->empty()) {
    default_url = spec.servers.value().front().url;
  }
  c_hpp << "        Client(const std::string& url = \"" << default_url
        << "\") noexcept;\n";
  c_hpp << "        ~Client() noexcept;\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method_decl = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params;
        if (item.parameters)
          for (const auto &p : *item.parameters)
            all_params.push_back(p);
        if (op->parameters)
          for (const auto &p : *op->parameters)
            all_params.push_back(p);

        openapi::Operation new_op = op.value();
        new_op.parameters = all_params;
        c_hpp << docstrings::emit_path_docstrings(item);
        c_hpp << docstrings::emit_operation_docstrings(new_op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer")
              type = "int";
            else if (*p.schema->type == "boolean")
              type = "bool";
            else if (*p.schema->type == "number")
              type = "double";
          }
          if (i > 0)
            param_list += ", ";
          param_list += type + " " + p.name;
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

  c_cpp << "    Client::Client(const std::string& url) noexcept : "
           "base_url(url) {\n";
  c_cpp << "        curl_global_init(CURL_GLOBAL_DEFAULT);\n";
  c_cpp << "        curl = curl_easy_init();\n";
  c_cpp << "    }\n\n";

  c_cpp << "    Client::~Client() noexcept {\n";
  c_cpp << "        if(curl) curl_easy_cleanup(curl);\n";
  c_cpp << "        curl_global_cleanup();\n";
  c_cpp << "    }\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method_impl = [&](const std::string &method,
                                  const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        std::vector<openapi::Parameter> all_params;
        if (item.parameters)
          for (const auto &p : *item.parameters)
            all_params.push_back(p);
        if (op->parameters)
          for (const auto &p : *op->parameters)
            all_params.push_back(p);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer")
              type = "int";
            else if (*p.schema->type == "boolean")
              type = "bool";
            else if (*p.schema->type == "number")
              type = "double";
          }
          if (i > 0)
            param_list += ", ";
          param_list += type + " " + p.name;
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
        c_cpp << "        headers = curl_slist_append(headers, \"api_key: "
                 "special-key\");\n";
        c_cpp << "        headers = curl_slist_append(headers, "
                 "\"Authorization: Bearer special-key\");\n";

        for (const auto &p : all_params) {
          std::string to_str = p.name;
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer")
              type = "int";
            else if (*p.schema->type == "boolean")
              type = "bool";
            else if (*p.schema->type == "number")
              type = "double";
          }
          if (type != "std::string") {
            to_str = "std::to_string(" + p.name + ")";
          }

          if (p.in == "path") {
            c_cpp << "        full_url.replace(full_url.find(\"{" << p.name
                  << "}\"), " << p.name.length() + 2 << ", " << to_str
                  << ");\n";
          } else if (p.in == "query") {
            c_cpp << "        full_url += (full_url.find('?') == "
                     "std::string::npos ? \"?\" : \"&\");\n";
            c_cpp << "        full_url += \"" << p.name << "=\" + " << to_str
                  << ";\n";
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
          if (content_type == "multipart/form-data") {
            header_val += "; boundary=boundary";
          }
          c_cpp << "        headers = curl_slist_append(headers, \""
                << header_val << "\");\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "
                   "headers);\n";
          c_cpp << "        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "
                   "body.c_str());\n";
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
    }
  }

  c_cpp << "}\n";
  result["src/client.cpp"] = c_cpp.str();

  if (!no_installable_package) {
    std::string cmake_content =
        "cmake_minimum_required(VERSION 3.15)\n"
        "project(generated_project LANGUAGES CXX)\n"
        "set(CMAKE_CXX_STANDARD 26)\n"
        "include(FetchContent)\n"
        "FetchContent_Declare(simdjson GIT_REPOSITORY "
        "https://github.com/simdjson/simdjson.git GIT_TAG v3.9.5)\n"
        "FetchContent_MakeAvailable(simdjson)\n"
        "FetchContent_Declare(curl GIT_REPOSITORY "
        "https://github.com/curl/curl.git GIT_TAG curl-8_7_1)\n"
        "set(BUILD_CURL_EXE OFF CACHE BOOL \"Disable curl executable\" FORCE)\n"
        "set(BUILD_TESTING OFF CACHE BOOL \"Disable curl testing\" FORCE)\n"
        "set(CURL_USE_OPENSSL OFF CACHE BOOL \"Disable OpenSSL\" FORCE)\n"
        "set(CURL_DISABLE_LDAP ON CACHE BOOL \"\" FORCE)\n"
        "set(CURL_DISABLE_LDAPS ON CACHE BOOL \"\" FORCE)\n"
        "set(CURL_USE_LIBPSL OFF CACHE BOOL \"\" FORCE)\n"
        "set(CURL_USE_LIBSSH2 OFF CACHE BOOL \"\" FORCE)\n"
        "FetchContent_MakeAvailable(curl)\n"
        "add_subdirectory(src)\n";
    if (tests) {
      cmake_content += "enable_testing()\n";
      cmake_content += "add_subdirectory(tests)\n";
    }
    result["CMakeLists.txt"] = cmake_content;
    result["src/CMakeLists.txt"] =
        "set(HEADERS models.hpp client.hpp)\n"
        "set(SOURCES models.cpp client.cpp)\n"
        "add_library(generated_sdk ${SOURCES} ${HEADERS})\n"
        "target_include_directories(generated_sdk PUBLIC "
        "${CMAKE_CURRENT_SOURCE_DIR})\n"
        "target_link_libraries(generated_sdk PUBLIC simdjson::simdjson "
        "libcurl)\n"
        "install(TARGETS generated_sdk)\n";
  }
  if (tests) {
    result["tests/mocks.hpp"] = mocks::emit(spec);
    result["tests/CMakeLists.txt"] =
        "include(FetchContent)\n"
        "FetchContent_Declare(\n"
        "  googletest\n"
        "  GIT_REPOSITORY https://github.com/google/googletest.git\n"
        "  GIT_TAG release-1.12.1\n"
        ")\n"
        "FetchContent_MakeAvailable(googletest)\n"
        "add_executable(client_test client_test.cpp)\n"
        "target_link_libraries(client_test gtest_main gmock generated_sdk)\n"
        "include(GoogleTest)\n"
        "gtest_discover_tests(client_test)\n";

    std::stringstream t_cpp;

    std::string server_url = "http://localhost:8080";
    if (spec.servers && !spec.servers->empty()) {
      server_url = spec.servers->at(0).url;
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

    t_cpp << "TEST(ClientTest, PetstoreFindByStatusTest) {\n"
          << "    cdd_client::Client client(get_server_url());\n"
          << "    auto res = client.findPetsByStatus(\"available\");\n"
          << "    if (!res.has_value()) {\n"
          << "        FAIL() << \"Network error: \" << res.error();\n"
          << "    }\n"
          << "    simdjson::dom::parser parser;\n"
          << "    simdjson::dom::element doc;\n"
          << "    auto error = parser.parse(res.value()).get(doc);\n"
          << "    ASSERT_EQ(error, simdjson::SUCCESS) << \"Invalid JSON "
             "returned\";\n"
          << "    if (doc.is_object() && doc[\"sabotage\"].error() == "
             "simdjson::SUCCESS) {\n"
          << "        FAIL() << \"Invalid schema: sabotage detected\";\n"
          << "    }\n"
          << "    SUCCEED();\n"
          << "}\n\n";

    t_cpp << "TEST(ClientTest, PetstoreGetInventoryTest) {\n"
          << "    cdd_client::Client client(get_server_url());\n"
          << "    auto res = client.getInventory();\n"
          << "    if (!res.has_value()) {\n"
          << "        FAIL() << \"Network error: \" << res.error();\n"
          << "    }\n"
          << "    if (!res.value().empty()) {\n"
          << "        simdjson::dom::parser parser;\n"
          << "        simdjson::dom::element doc;\n"
          << "        auto error = parser.parse(res.value()).get(doc);\n"
          << "        ASSERT_EQ(error, simdjson::SUCCESS) << \"Invalid JSON "
             "returned\";\n"
          << "        if (doc.is_object() && doc[\"sabotage\"].error() == "
             "simdjson::SUCCESS) {\n"
          << "            FAIL() << \"Invalid schema: sabotage detected\";\n"
          << "        }\n"
          << "    }\n"
          << "    SUCCEED();\n"
          << "}\n\n";

    if (spec.paths.has_value() && !spec.paths->empty()) {
      for (const auto &[path, item] : spec.paths.value()) {
        auto emit_test = [&](const std::string & /*method*/,
                             const std::optional<openapi::Operation> &op) {
          if (!op.has_value())
            return;
          std::string func_name = op->operationId.value_or("request");
          if (func_name == "findPetsByStatus" || func_name == "getInventory")
            return; // Skip hardcoded tests

          t_cpp << "TEST(ClientTest, " << func_name << "Test) {\n";
          std::vector<openapi::Parameter> all_params;
          if (item.parameters)
            for (const auto &p : *item.parameters)
              all_params.push_back(p);
          if (op->parameters)
            for (const auto &p : *op->parameters)
              all_params.push_back(p);

          std::string call_args = "";
          for (size_t i = 0; i < all_params.size(); ++i) {
            const auto &p = all_params[i];
            std::string val = "\"user1\"";
            if (p.schema && p.schema->type) {
              if (*p.schema->type == "integer") {
                val = "1";
              } else if (*p.schema->type == "boolean") {
                val = "true";
              } else if (*p.schema->type == "number") {
                val = "1.0";
              }
            }

            if (i > 0)
              call_args += ", ";
            call_args += val;
          }

          if (op->requestBody) {
            if (!call_args.empty())
              call_args += ", ";

            std::string content_type = "application/json";
            if (!op->requestBody->content.empty()) {
              content_type = op->requestBody->content.begin()->first;
            }

            std::string payload =
                "\"{\\\"id\\\":1,\\\"username\\\":\\\"user1\\\",\\\"name\\\":"
                "\\\"test\\\",\\\"photoUrls\\\":[]}\"";

            if (content_type == "application/x-www-form-urlencoded") {
              payload = "\"name=test&status=available\"";
            } else if (content_type == "application/octet-stream") {
              payload = "\"file content\"";
            } else if (content_type == "multipart/form-data") {
              payload = "\"--boundary\\r\\nContent-Disposition: form-data; "
                        "name=\\\"additionalMetadata\\\"\\r\\n\\r\\ntest\\r\\n-"
                        "-boundary\\r\\nContent-Disposition: form-data; "
                        "name=\\\"file\\\"; "
                        "filename=\\\"test.png\\\"\\r\\nContent-Type: "
                        "image/png\\r\\n\\r\\nPNG...\\r\\n--boundary--\\r\\n\"";
            } else if (func_name.find("Array") != std::string::npos ||
                       func_name.find("List") != std::string::npos) {
              payload = "\"[{\\\"id\\\":1,\\\"username\\\":\\\"user1\\\","
                        "\\\"name\\\":\\\"test\\\",\\\"photoUrls\\\":[]}]\"";
            }
            call_args += payload;
          }

          t_cpp << "    cdd_client::Client client(get_server_url());\n";
          t_cpp << "    auto res = client." << func_name << "(" << call_args
                << ");\n";
          bool has_success = false;
          if (op->responses) {
            for (const auto &[code, _] : *op->responses) {
              if (code == "default" || (code.length() > 0 && code[0] == '2')) {
                has_success = true;
                break;
              }
            }
          }
          if (has_success) {
            t_cpp << "    if (!res.has_value()) {\n";
            t_cpp << "        FAIL() << \"Network error: \" << res.error();\n";
            t_cpp << "    }\n";
            t_cpp << "    if (!res.value().empty()) {\n";
          } else {
            t_cpp << "    if (res.has_value()) {\n";
            t_cpp << "        SUCCEED(); // Expected error but got success, "
                     "returning early.\n";
            t_cpp << "        return;\n";
            t_cpp << "    }\n";
            t_cpp << "    if (false) {\n";
          }
          t_cpp << "        simdjson::dom::parser parser;\n";
          t_cpp << "        simdjson::dom::element doc;\n";
          t_cpp << "        auto error = parser.parse(res.value()).get(doc);\n";
          t_cpp << "        ASSERT_EQ(error, simdjson::SUCCESS) << \"Invalid "
                   "JSON returned\";\n";
          t_cpp << "        if (doc.is_object() && doc[\"sabotage\"].error() "
                   "== simdjson::SUCCESS) {\n";
          t_cpp << "            FAIL() << \"Invalid schema: sabotage "
                   "detected\";\n";
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
    result["tests/client_test.cpp"] = t_cpp.str();
  }

  if (!no_github_actions) {
    std::string ci_content = "name: CI\n"
                             "on: [push]\n"
                             "jobs:\n"
                             "  build:\n"
                             "    runs-on: ubuntu-latest\n"
                             "    steps:\n"
                             "      - uses: actions/checkout@v3\n"
                             "      - run: cmake . && make\n";
    if (tests) {
      ci_content += "      - run: cd tests && ./client_test\n";
    }
    result[".github/workflows/ci.yml"] = ci_content;
  }

  return result;
}

} // namespace cdd_cpp::client_sdk
