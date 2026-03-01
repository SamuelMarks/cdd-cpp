#include "../openapi/upgraders/upgrader.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi::upgraders {
void test_upgraders() {
  std::string oas_1_2 =
      R"({"swaggerVersion": "1.2", "apis": [], "info": {"title": "Test 1.2"}})";
  std::string res_1_2 = upgrade_to_latest(oas_1_2);
  assert(res_1_2.find("\"openapi\":\"3.2.0\"") != std::string::npos);

  std::string oas_2_0 =
      R"({"swagger": "2.0", "host": "api.example.com", "basePath": "/v1", "info": {"title": "Test 2.0"}})";
  std::string res_2_0 = upgrade_to_latest(oas_2_0);
  assert(res_2_0.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_2_0.find("https://api.example.com/v1") != std::string::npos);

  std::string oas_3_0 =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}})";
  std::string res_3_0 = upgrade_to_latest(oas_3_0);
  assert(res_3_0.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_3_0.find("\"info\":{\"title\":\"Test 3.0\"}") !=
         std::string::npos);

  std::cout << "openapi::upgraders::test_upgraders passed.\n";
}
} // namespace cdd_cpp::openapi::upgraders
