/// @file http.cpp
/// @brief Unit tests for JsonWriter, C++ keyword sanitization, Error types, and
/// local HTTP GET client.

#include "../utils/http.hpp"
#include "../utils/error.hpp"
#include "../utils/httplib.h"
#include "../utils/json_writer.hpp"
#include "../utils/type_mapping.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>

namespace cdd_cpp::utils {

void test_http() {
  // 1. Test Error and ErrorCode helpers
  for (auto code :
       {ErrorCode::JsonParseError, ErrorCode::InvalidOpenApiDocument,
        ErrorCode::MissingRequiredField, ErrorCode::InvalidFieldType,
        ErrorCode::UnsupportedSpecVersion, ErrorCode::HttpInitFailed,
        ErrorCode::HttpPerformFailed, ErrorCode::WasiUnsupportedOperation,
        ErrorCode::FileReadError, ErrorCode::FileWriteError,
        ErrorCode::DirectoryIterationError, ErrorCode::CppAstParseError,
        ErrorCode::CliArgumentError, ErrorCode::JsonRpcInvalidRequest,
        ErrorCode::JsonRpcMethodNotFound, ErrorCode::JsonRpcInvalidParams,
        ErrorCode::McpProtocolError}) {
    assert(!error_code_to_string(code).empty());
  }
  assert(error_code_to_string(static_cast<ErrorCode>(999)) == "UnknownError");

  Error e_default;
  assert(e_default.empty());
  Error e_str(std::string("msg1"));
  Error e_cstr("msg1");
  Error e_null(static_cast<const char *>(nullptr));
  assert(e_null.empty());
  assert(e_str == e_cstr);
  assert(e_str == std::string_view("msg1"));
  assert(e_str == "msg1");
  assert(!(e_str == static_cast<const char *>(nullptr)));
  assert(std::string(e_str.c_str()) == "msg1");
  const std::string &s_ref = e_str;
  assert(s_ref == "msg1");
  assert(e_str.find("msg") == 0);
  assert((std::string("Error: ") + e_str) == "Error: msg1");
  assert(("Error: " + e_str) == "Error: msg1");
  assert((static_cast<const char *>(nullptr) + e_str) == "msg1");

  Error e_ctx(ErrorCode::JsonParseError, "bad token", "/paths/0");
  assert(e_ctx.format() == "JsonParseError [/paths/0]: bad token");
  assert(e_str.format() == "InvalidOpenApiDocument: msg1");

  std::ostringstream err_os;
  err_os << e_str;
  assert(err_os.str() == "msg1");

  // 2. Test C++ keyword sanitization and unsanitization
  assert(is_cpp_keyword("class"));
  assert(!is_cpp_keyword("normal_field"));
  assert(sanitize_cpp_identifier("class") == "class_");
  assert(sanitize_cpp_identifier("normal_field") == "normal_field");
  assert(unsanitize_cpp_identifier("class_") == "class");
  assert(unsanitize_cpp_identifier("normal_") == "normal_");
  assert(unsanitize_cpp_identifier("_") == "_");
  assert(openapi_type_to_sql("integer") == "INTEGER");
  assert(openapi_type_to_sql("number") == "DOUBLE PRECISION");
  assert(openapi_type_to_sql("boolean") == "BOOLEAN");
  assert(openapi_type_to_sql("string") == "TEXT");
  assert(cpp_type_to_openapi(" \t const  int & \t ") == "integer");

  // 3. Test JsonWriter
  JsonWriter jw;
  jw.start_array();
  jw.null_value();
  jw.null_value();
  jw.value(true);
  jw.value(false);
  jw.value("quote\"newline\nbackslash\\tab\tCR\rcarriage\fbackspace\b\x01");
  jw.start_object();
  jw.key("k");
  jw.raw_value("{}");
  jw.key_value("k2", "v2");
  jw.key_value("k3", true);
  jw.key_value("k4", "v4");
  jw.key_optional<std::string>("k5", std::optional<std::string>("opt_v"));
  jw.key_optional<int>("k5_int", std::optional<int>(42));
  jw.key_optional<double>("k5_dbl", std::optional<double>(42.5));
  jw.key_optional<bool>("k5_bool", std::optional<bool>(true));
  jw.key_optional<std::string>("k6", std::nullopt);
  jw.key("k7");
  jw.value(123);
  jw.key("k8");
  jw.value(123.45);
  jw.end_object();
  jw.start_array();
  jw.raw_value("1");
  jw.raw_value("2");
  jw.end_array();
  jw.end_array();

  JsonWriter jw2;
  jw2.value("first");
  jw2.start_object();
  jw2.end_object();
  jw2.start_array();
  jw2.end_array();
  jw2.value(true);
  jw2.value(1);
  jw2.value(1.0);
  jw2.value("str");
  jw2.key_optional<std::string>("k_opt1", std::optional<std::string>("val"));
  jw2.key_optional<int>("k_opt2", std::optional<int>(1));
  jw2.key_optional<double>("k_opt3", std::optional<double>(1.0));
  jw2.key_optional<bool>("k_opt4", std::optional<bool>(false));

  std::string s = jw.str();
  assert(s.find("null") != std::string::npos);

  // 4. Test http_get against a deterministic local ephemeral httplib::Server
  {
    CurlGlobalInitializer init_guard;
  }
  httplib::Server svr;
  svr.Get("/hello", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("{\"status\":\"ok\"}", "application/json");
  });
  int port = svr.bind_to_any_port("127.0.0.1");
  std::thread server_thread([&]() { svr.listen_after_bind(); });

  auto res = http_get("http://127.0.0.1:" + std::to_string(port) + "/hello");
  svr.stop();
  if (server_thread.joinable())
    server_thread.join();

  assert(res.has_value());
  assert(*res == "{\"status\":\"ok\"}");

  // Test invalid url
  auto res2 = http_get("invalid://url");
  assert(!res2.has_value());
  assert(res2.error().code == ErrorCode::HttpPerformFailed);

  std::cout << "utils::test_http passed.\n";
}

} // namespace cdd_cpp::utils
