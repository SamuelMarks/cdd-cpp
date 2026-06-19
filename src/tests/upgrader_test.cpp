
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

  auto res_invalid_root = upgrade_to_latest("[]");
  assert(!res_invalid_root.has_value());

  auto res_no_version =
      upgrade_to_latest("{\"info\": {\"title\": \"No Version\"}}");
  assert(!res_no_version.has_value());

  auto res_invalid_swagger_version = upgrade_to_latest("{\"swagger\": 2.0}");
  assert(!res_invalid_swagger_version.has_value());

  auto res_unknown_swagger = upgrade_to_latest("{\"swagger\": \"3.0\"}");
  assert(!res_unknown_swagger.has_value());

  auto res_invalid_openapi_version = upgrade_to_latest("{\"openapi\": 3.0}");
  assert(!res_invalid_openapi_version.has_value());

  auto res_unknown_openapi = upgrade_to_latest("{\"openapi\": \"2.0\"}");
  assert(!res_unknown_openapi.has_value());

  std::string oas_2_0_flows =
      R"({"swagger": "2.0", "info": {"title": "Test 2.0"}, "securityDefinitions": {"O1": {"type": "oauth2", "flow": "accessCode", "authorizationUrl": "u1", "tokenUrl": "u2"}, "O2": {"type": "oauth2", "flow": "application", "tokenUrl": "u3"}}})";
  auto res_2_0_flows_res = upgrade_to_latest(oas_2_0_flows);
  assert(res_2_0_flows_res.has_value());
  std::string res_2_0_flows_out = *res_2_0_flows_res;
  assert(res_2_0_flows_out.find("\"authorizationCode\":") != std::string::npos);
  assert(res_2_0_flows_out.find("\"clientCredentials\":") != std::string::npos);

  std::string oas_2_0_no_schemes =
      R"({"swagger": "2.0", "info": {"title": "Test 2.0"}, "host": "api.example.com"})";
  auto res_2_0_no_schemes_res = upgrade_to_latest(oas_2_0_no_schemes);
  assert(res_2_0_no_schemes_res.has_value());
  assert(res_2_0_no_schemes_res->find("https://api.example.com") !=
         std::string::npos);

  std::string oas_3_0_arr =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_arr": {"type": "array", "items": [{"type": "string"}]}}}})";
  auto res_3_0_arr_res = upgrade_to_latest(oas_3_0_arr);
  assert(res_3_0_arr_res.has_value());
  std::string res_3_0_arr_out = *res_3_0_arr_res;
  assert(res_3_0_arr_out.find("\"prefixItems\"") != std::string::npos);

  std::string oas_3_0_obj_schema =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_obj": {"type": "object", "properties": {"schema": {"type": "string"}, "items": {"type": "string"}}}}}})";
  auto res_3_0_obj_schema_res = upgrade_to_latest(oas_3_0_obj_schema);
  assert(res_3_0_obj_schema_res.has_value());

  std::string oas_3_0_minmax_uint =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_minmax": {"type": "integer", "minimum": 18446744073709551615, "maximum": 18446744073709551615, "exclusiveMinimum": true, "exclusiveMaximum": true}}}})";
  auto res_3_0_minmax_uint_res = upgrade_to_latest(oas_3_0_minmax_uint);
  assert(res_3_0_minmax_uint_res.has_value());

  std::string oas_1_2_no_title =
      R"({"swaggerVersion": "1.2", "info": {"description": "d"}})";
  auto res_1_2_no_title_res = upgrade_to_latest(oas_1_2_no_title);
  assert(res_1_2_no_title_res.has_value());
  assert(res_1_2_no_title_res->find("Upgraded from 1.2") != std::string::npos);

  std::string oas_1_2_no_apis =
      R"({"swaggerVersion": "1.2", "info": {"title": "Test 1.2"}})";
  auto res_1_2_no_apis_res = upgrade_to_latest(oas_1_2_no_apis);
  assert(res_1_2_no_apis_res.has_value());

  std::string oas_1_2_root_version = R"({"swaggerVersion": "1.2"})";
  auto res_1_2_root_version_res = upgrade_to_latest(oas_1_2_root_version);
  assert(res_1_2_root_version_res.has_value());

  std::string oas_1_2_swagger_key = R"({"swagger": "1.2"})";
  auto res_1_2_swagger_key_res = upgrade_to_latest(oas_1_2_swagger_key);
  assert(res_1_2_swagger_key_res.has_value());

  std::string oas_3_0_param_schema =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "paths": {"/a": {"parameters": [{"name": "p1", "in": "query", "type": "string", "schema": {"type": "integer"}, "items": {"type": "number"}, "collectionFormat": "csv", "default": "val", "maximum": 1, "exclusiveMaximum": true, "minimum": 0, "exclusiveMinimum": true, "maxLength": 10, "minLength": 1, "pattern": "a", "maxItems": 10, "minItems": 1, "uniqueItems": true, "multipleOf": 2, "enum": ["a"], "unhandled_param_prop": "val"}]}}})";
  auto res_3_0_param_schema_res = upgrade_to_latest(oas_3_0_param_schema);
  assert(res_3_0_param_schema_res.has_value());

  std::string oas_3_0_param_no_schema =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "paths": {"/a": {"parameters": [{"name": "p1", "in": "query", "type": "string", "pattern": "a", "collectionFormat": "csv", "default": "val", "maximum": 1, "exclusiveMaximum": true, "minimum": 0, "exclusiveMinimum": true, "maxLength": 10, "minLength": 1, "pattern": "a", "maxItems": 10, "minItems": 1, "uniqueItems": true, "multipleOf": 2, "enum": ["a"], "unhandled_param_prop": "val"}]}}})";
  auto res_3_0_param_no_schema_res = upgrade_to_latest(oas_3_0_param_no_schema);
  assert(res_3_0_param_no_schema_res.has_value());

  std::string oas_3_0_not_schema =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_not_schema": {"not_type": "string", "schema": ["not_obj_schema"], "items": ["not_obj_schema"]}}}})";
  auto res_3_0_not_schema_res = upgrade_to_latest(oas_3_0_not_schema);
  assert(res_3_0_not_schema_res.has_value());

  std::string oas_3_0_not_schema_obj =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "components": {"schemas": {"test_not_schema": {"not_type": "string", "schema": {"type": "object"}, "items": {"type": "string"}}}}})";
  auto res_3_0_not_schema_obj_res = upgrade_to_latest(oas_3_0_not_schema_obj);
  assert(res_3_0_not_schema_obj_res.has_value());

  std::string oas_3_0_recursive_not_schema =
      R"({"openapi": "3.0.0", "info": {"title": "Test 3.0"}, "paths": {"/a": {"parameters": [{"name": "p1", "in": "query", "schema": ["not_obj_schema"], "items": ["not_obj_schema"]}]}}})";
  auto res_3_0_recursive_not_schema_res =
      upgrade_to_latest(oas_3_0_recursive_not_schema);
  assert(res_3_0_recursive_not_schema_res.has_value());

  std::string oas_invalid_swagger_format = R"({"swagger": ["2.0"]})";
  auto res_invalid_swagger_format =
      upgrade_to_latest(oas_invalid_swagger_format);
  assert(!res_invalid_swagger_format.has_value());

  std::cout << "openapi::upgraders::test_upgraders passed.\n";
}
} // namespace cdd_cpp::openapi::upgraders
