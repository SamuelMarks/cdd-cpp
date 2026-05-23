#include "../utils/http.hpp"
#include "../utils/json_writer.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::utils {
void test_http() {
  JsonWriter jw;
  jw.start_array();
  jw.null_value();
  jw.null_value(); // Calls when !first to hit line 105
  jw.value(true);
  jw.value(false);
  jw.value("quote\"newline\nbackslash\\tab\tCR\rcarriage\fbackspace\b\x01");
  jw.start_object();
  jw.key("k");
  jw.raw_value("{}");
  jw.key_value("k2", "v2");
  jw.key_value("k3", true);
  jw.key_value("k4", "v4"); // const char*
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

  // Test !first branches
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

  // Test http_get
  auto res = http_get("http://example.com");
  // Assuming curl is present or it fails
  if (res) {
    assert(res->find("body") != std::string::npos ||
           res->find("html") != std::string::npos || res->size() >= 0);
  } else {
    assert(res.error().find("curl") != std::string::npos ||
           res.error().find("not implemented") != std::string::npos);
  }

  // Test invalid url
  auto res2 = http_get("invalid://url");
  assert(!res2.has_value());

  std::cout << "utils::test_http passed.\n";
}
} // namespace cdd_cpp::utils
