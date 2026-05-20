#include "../openapi/upgraders/upgrader.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi::upgraders {
void test_upgraders() {
  std::string oas_1_2 =
      R"({"swaggerVersion": "1.2", "apis": [{"path": "/a", "operations": [{"method": "GET", "nickname": "getA"}]}], "info": {"title": "Test 1.2", "description": "d"}})";
  auto res_1_2_res = upgrade_to_latest(oas_1_2);
  assert(res_1_2_res.has_value());
  std::string res_1_2 = *res_1_2_res;
  assert(res_1_2.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_1_2.find("\"description\":\"d\"") != std::string::npos);
  assert(res_1_2.find("\"getA\"") != std::string::npos);

  std::string oas_1_2_no_info = R"({"swaggerVersion": "1.2"})";
  auto res_1_2_no_info_res = upgrade_to_latest(oas_1_2_no_info);
  assert(res_1_2_no_info_res.has_value());
  std::string res_1_2_no_info = *res_1_2_no_info_res;
  assert(res_1_2_no_info.find("\"Upgraded from 1.2\"") != std::string::npos);

  std::string oas_2_0_fallback =
      R"({"swagger": "2.0", "info": {"title": "Fallback Test"}, "paths": {"/b": {"post": {"parameters": [{"in": "body", "name": "b", "schema": {"type": "object"}}], "responses": {"200": {"description": "OK"}}}}}})";
  auto res_2_0_fb = upgrade_to_latest(oas_2_0_fallback);
  assert(res_2_0_fb.has_value());
  assert(res_2_0_fb->find("\"application/json\"") != std::string::npos);

  std::string oas_2_0 =
      R"({"swagger": "2.0", "host": "api.example.com", "basePath": "/v1", "schemes": ["http"], "consumes": ["text/plain"], "produces": ["application/xml"], "info": {"title": "Test 2.0"}, "tags": [{"name": "A"}], "security": [], "definitions": {"D": {"type": "string"}}, "parameters": {"P": {"type": "string", "maxLength": 10, "pattern": "a", "minItems": 1, "uniqueItems": true, "enum": ["a"], "multipleOf": 2}}, "responses": {"R": {"schema": {"type": "string"}, "description": "desc"}}, "securityDefinitions": {"S": {"type": "oauth2", "flow": "implicit", "authorizationUrl": "url"}, "B": {"type": "basic"}, "A": {"type": "apiKey", "in": "header", "name": "X-Auth"}}, "paths": {"/a": {"parameters": [{"in": "query", "name": "p1", "type": "string"}], "$ref": "pathRef", "get": {"consumes": ["application/json", "multipart/form-data"], "produces": ["application/json"], "summary": "Sum", "parameters": [{"in": "formData", "name": "f1", "type": "string", "required": true}, {"$ref": "#/parameters/P"}], "responses": {"200": {"description": "desc", "schema": {"type": "string"}}}}}}})";
  auto res_2_0_res = upgrade_to_latest(oas_2_0);
  assert(res_2_0_res.has_value());
  std::string res_2_0 = *res_2_0_res;
  assert(res_2_0.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_2_0.find("http://api.example.com/v1") != std::string::npos);

  std::string oas_3_0 =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test": {"type": "integer", "minimum": 0, "exclusiveMinimum": true, "maximum": 10, "exclusiveMaximum": true}, "test2": {"type": "number", "minimum": 0.5, "maximum": 10.5}, "arr": {"type": "array", "items": [{"type": "string"}]}, "obj": {"type": "object", "properties": {"schema": "string", "items": ["string"]}}}}})";
  auto res_3_0_res = upgrade_to_latest(oas_3_0);
  assert(res_3_0_res.has_value());
  std::string res_3_0 = *res_3_0_res;
  assert(res_3_0.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_3_0.find("\"info\":{\"title\":\"Test 3.0\"}") !=
         std::string::npos);
  assert(res_3_0.find("\"exclusiveMinimum\":0") != std::string::npos);
  assert(res_3_0.find("\"exclusiveMaximum\":10") != std::string::npos);

  std::string oas_3_1 =
      R"({"openapi": "3.1.0", "info": {"title": "Test 3.1"}})";
  auto res_3_1_res = upgrade_to_latest(oas_3_1);
  assert(res_3_1_res.has_value());
  std::string res_3_1 = *res_3_1_res;
  assert(res_3_1.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(res_3_1.find("\"info\":{\"title\":\"Test 3.1\"}") !=
         std::string::npos);

  auto res_invalid = upgrade_to_latest("{invalid json");
  assert(!res_invalid.has_value());

  std::cout << "openapi::upgraders::test_upgraders passed.\n";
}
} // namespace cdd_cpp::openapi::upgraders
