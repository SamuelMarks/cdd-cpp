#include "../utils/http.hpp"
#include "../utils/json_writer.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::utils {
void test_http() {
  JsonWriter jw;
  jw.start_array();
  jw.null_value();
  jw.value(true);
  jw.value(false);
  jw.value("quote\"newline\nbackslash\\tab\tCR\rcarriage\fbackspace\b\x01");
  jw.start_object();
  jw.key("k");
  jw.raw_value("{}");
  jw.end_object();
  jw.start_array();
  jw.raw_value("1");
  jw.raw_value("2");
  jw.end_array();
  jw.end_array();

  std::string s = jw.str();
  assert(s.find("null") != std::string::npos);

  std::cout << "utils::test_http passed.\n";
}
} // namespace cdd_cpp::utils
