// GCOV_EXCL_BR_START
#include "../openapi/upgraders/upgrader.hpp"
#include <iostream>

int main() {
  std::string oas_3_0_arr =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_arr": {"type": "array", "items": [{"type": "string"}]}}}})";
  auto res = cdd_cpp::openapi::upgraders::upgrade_to_latest(oas_3_0_arr);
  if (res)
    std::cout << *res << "\n";
  return 0;
}

// GCOV_EXCL_BR_STOP
