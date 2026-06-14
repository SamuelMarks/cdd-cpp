// GCOV_EXCL_BR_START

#include "../client_sdk/emit.hpp"
#include "../client_sdk/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::client_sdk {
void test_emit_client() {
  openapi::OpenAPI spec;
  spec.info.title = "Test API";
  spec.info.version = "1.0.0";
  spec.info.description = "A great API";
  spec.info.termsOfService = "https://terms";
  spec.info.contact =
      openapi::Contact{"Samuel", "https://samuel", "samuel@test"};
  spec.info.license = openapi::License{"MIT", "MIT", "https://mit"};

  openapi::ServerVariable sv;
  sv.default_value = "default";
  sv.description = "desc";
  sv.enum_values = std::vector<std::string>{"enum1"};
  openapi::Server s1;
  s1.url = "https://api.example.com";
  s1.description = "Main server";
  s1.variables = std::map<std::string, openapi::ServerVariable>{{"var", sv}};
  spec.servers = std::vector<openapi::Server>{s1};

  openapi::SecurityScheme scheme;
  scheme.type = "http";
  scheme.name = "name";
  scheme.in = "header";
  scheme.scheme = "basic";
  scheme.bearerFormat = "format";
  scheme.description = "desc";
  scheme.openIdConnectUrl = "https://oidc";
  scheme.oauth2MetadataUrl = "https://oauth";
  scheme.deprecated = true;
  spec.components = openapi::Components{};
  spec.components->securitySchemes =
      std::map<std::string, openapi::SecurityScheme>{{"BasicAuth", scheme}};

  openapi::Schema prop1;
  prop1.type = "integer";
  openapi::Schema prop2;
  prop2.type = "number";
  openapi::Schema prop3;
  prop3.type = "boolean";
  openapi::Schema prop4;
  prop4.type = "string";

  openapi::Schema arr_items;
  arr_items.type = "string";
  openapi::Schema prop5;
  prop5.type = "array";
  prop5.items = std::make_shared<openapi::Schema>(arr_items);

  openapi::Schema prop6;
  prop6.type = "array"; // no items

  openapi::Schema prop7;
  prop7.ref = openapi::Reference{"#/components/schemas/Other"};

  openapi::Schema prop8;
  prop8.ref = openapi::Reference{"Other"}; // no slash

  openapi::Schema prop9;
  prop9.type = "unknown";

  openapi::Schema obj_schema;
  obj_schema.type = "object";
  obj_schema.properties =
      std::make_shared<std::map<std::string, openapi::Schema>>(
          std::initializer_list<std::pair<const std::string, openapi::Schema>>{
              {"p1", prop1},
              {"p2", prop2},
              {"p3", prop3},
              {"p4", prop4},
              {"p5", prop5},
              {"p6", prop6},
              {"p7", prop7},
              {"p8", prop8},
              {"p9", prop9}});
  obj_schema.required = std::vector<std::string>{"p1"};

  spec.components->schemas =
      std::map<std::string, openapi::Schema>{{"MyObj", obj_schema}};

  openapi::PathItem pi;
  pi.summary = "Path Summary";
  pi.description = "Path Description";

  openapi::Parameter path_param;
  path_param.name = "id";
  path_param.in = "path";
  path_param.description = "The ID";
  path_param.schema = openapi::Schema{};
  path_param.schema->type = "integer";

  openapi::Parameter query_param;
  query_param.name = "filter";
  query_param.in = "query";
  query_param.schema = openapi::Schema{};
  query_param.schema->type = "boolean";

  openapi::Parameter header_param;
  header_param.name = "X_Header";
  header_param.in = "header";
  header_param.schema = openapi::Schema{};
  header_param.schema->type = "number";

  pi.parameters = std::vector<openapi::Parameter>{path_param};

  openapi::Operation get_op;
  get_op.operationId = "getPet";
  get_op.summary = "Get Pet Summary";
  get_op.description = "Get Pet Description";
  get_op.tags = std::vector<std::string>{"pets", "admin"};
  get_op.deprecated = true;
  get_op.security = std::vector<openapi::SecurityRequirement>{};
  openapi::SecurityRequirement sec_req;
  sec_req["Bearer"] = {"read", "write"};
  get_op.security->push_back(sec_req);
  get_op.parameters =
      std::vector<openapi::Parameter>{query_param, header_param};

  openapi::RequestBody req_body;
  req_body.description = "Body Description";
  req_body.content = std::map<std::string, openapi::MediaType>{
      {"multipart/form-data", openapi::MediaType{}}};
  get_op.requestBody = req_body;

  openapi::Response resp_200;
  resp_200.description = "Success";
  resp_200.content = std::map<std::string, openapi::MediaType>{
      {"application/json", openapi::MediaType{}}};
  get_op.responses =
      std::map<std::string, openapi::Response>{{"200", resp_200}};

  pi.get = get_op;

  openapi::Operation patch_op = get_op;
  patch_op.operationId = "patchPet";
  patch_op.requestBody->content = std::map<std::string, openapi::MediaType>{
      {"application/x-www-form-urlencoded", openapi::MediaType{}}};
  pi.patch = patch_op;

  openapi::Operation put_op = get_op;
  put_op.operationId = "putPetStream";
  put_op.requestBody->content = std::map<std::string, openapi::MediaType>{
      {"application/octet-stream", openapi::MediaType{}}};
  pi.put = put_op;

  openapi::Operation post_op = get_op;
  post_op.operationId = "createArray";
  post_op.responses =
      std::map<std::string, openapi::Response>{{"400", resp_200}};
  post_op.requestBody->content = std::map<std::string, openapi::MediaType>{
      {"application/json", openapi::MediaType{}}};
  pi.post = post_op;

  openapi::Operation skip_op1 = get_op;
  skip_op1.operationId = "findPetsByStatus";
  openapi::Operation skip_op2 = get_op;
  skip_op2.operationId = "getInventory";
  openapi::Operation list_op = get_op;
  list_op.operationId = "listItems";
  list_op.requestBody->content = std::map<std::string, openapi::MediaType>{
      {"application/json", openapi::MediaType{}}};

  openapi::PathItem pi_extra;
  pi_extra.get = skip_op1;
  pi_extra.post = skip_op2;
  pi_extra.put = list_op;

  std::map<std::string, openapi::PathItem> paths;
  paths["/pet/{id}"] = pi;
  paths["/extra"] = pi_extra;
  spec.paths = paths;

  auto generated_files = emit_client(spec);
  auto generated_files_with_tests = emit_client(spec, false, false, true);
  auto generated_files_with_options = emit_client(spec, true, true, false);

  std::string generated = "";
  for (const auto &[name, content] : generated_files) {
    generated += content + "\\n";
  }

  std::string generated2 = "";
  for (const auto &[name, content] : generated_files_with_tests) {
    generated2 += name + "\\n" + content + "\\n";
  }

  std::string generated3 = "";
  for (const auto &[name, content] : generated_files_with_options) {
    generated3 += content + "\\n";
  }

  assert(generated.find("/// @title Test API") != std::string::npos);
  assert(generated.find("/// @version 1.0.0") != std::string::npos);
  assert(generated.find("/// @description A great API") != std::string::npos);
  assert(generated.find("/// @termsOfService https://terms") !=
         std::string::npos);
  assert(generated.find("/// @contact_name Samuel") != std::string::npos);
  assert(generated.find("/// @contact_url https://samuel") !=
         std::string::npos);
  assert(generated.find("/// @contact_email samuel@test") != std::string::npos);
  assert(generated.find("/// @license_name MIT") != std::string::npos);
  assert(generated.find("/// @license_identifier MIT") != std::string::npos);
  assert(generated.find("/// @license_url https://mit") != std::string::npos);
  assert(generated.find("Main server") != std::string::npos);
  assert(generated.find("@server_variable var default desc [enum1]") !=
         std::string::npos);
  assert(generated.find("@securitySchemes") != std::string::npos);
  assert(generated.find("- BasicAuth http name header basic format") !=
         std::string::npos);
  assert(generated.find("@securityScheme_description BasicAuth desc") !=
         std::string::npos);
  assert(generated.find(
             "@securityScheme_openIdConnectUrl BasicAuth https://oidc") !=
         std::string::npos);
  assert(generated.find(
             "@securityScheme_oauth2MetadataUrl BasicAuth https://oauth") !=
         std::string::npos);
  assert(generated.find("@securityScheme_deprecated BasicAuth") !=
         std::string::npos);

  assert(generated.find("@summary Path Summary") != std::string::npos);
  assert(generated.find("/// @description Path Description") !=
         std::string::npos);
  assert(generated.find("@summary Get Pet Summary") != std::string::npos);
  assert(generated.find("/// @description Get Pet Description") !=
         std::string::npos);

  assert(generated.find("@param id path parameter") != std::string::npos);
  assert(generated.find("@param filter query parameter") != std::string::npos);
  assert(generated.find("@param X_Header header parameter") !=
         std::string::npos);
  assert(generated.find("@param body request body - Body Description") !=
         std::string::npos);
  assert(generated.find("@tags pets,admin") != std::string::npos);
  assert(generated.find("@deprecated") != std::string::npos);
  assert(generated.find("@security Bearer [read,write]") != std::string::npos);

  assert(generated.find("@return 200 - Success") != std::string::npos);
  assert(generated.find("/// @response_content 200 application/json") !=
         std::string::npos);

  assert(generated2.find("tests/client_test.cpp") != std::string::npos);
  assert(generated2.find("add_subdirectory(tests)") != std::string::npos);
  assert(generated2.find("cd tests && ./client_test") != std::string::npos);

  openapi::OpenAPI spec_empty;
  spec_empty.paths =
      std::map<std::string, openapi::PathItem>{{"/{id}", openapi::PathItem{}}};
  openapi::Operation get_op_empty;
  get_op_empty.operationId = "getEmpty";
  spec_empty.paths->at("/{id}").get = get_op_empty;
  emit_client(spec_empty);

  openapi::OpenAPI spec_str;
  spec_str.paths = std::map<std::string, openapi::PathItem>{
      {"/\"\\\n\r", openapi::PathItem{}}};
  emit_client(spec_str);
  assert(
      generated.find("content-type: multipart/form-data; boundary=boundary") !=
      std::string::npos);

  assert(generated.find("getPet(int id, bool filter, double X_Header, const "
                        "std::string& body)") != std::string::npos);
  assert(generated.find("patchPet(int id, bool filter, double X_Header, const "
                        "std::string& body)") != std::string::npos);

  assert(generated.find("full_url += (full_url.find('?') == std::string::npos "
                        "? \"?\" : \"&\");") != std::string::npos);

  assert(generated.find("class Client") != std::string::npos);

  // Test operation without ID
  openapi::Operation no_id_op;
  pi.post = no_id_op;
  paths["/no_id"] = pi;
  spec.paths = paths;
  auto generated4_files = emit_client(spec);
  std::string generated4 = "";
  for (const auto &[name, content] : generated4_files) {
    generated4 += content + "\\n";
  }

  std::cout << "client_sdk::test_emit_client passed.\n";
}

void test_parse() {
  std::string client_code = R"(
    /**
     * @title Parsed API Client
     * @summary The summary
     * @version 1.0.0
     * @description A great API
     * @termsOfService https://terms
     * @contact_url https://samuel
     * @contact_email samuel@test
     * @license_identifier MIT
     * @license_url https://mit
     * @servers
     * - https://srv (Main)
     *   @server_variable var default desc [enum1]
    @break_tag1
     * @securitySchemes
     * - BasicAuth http name header basic format
     *   @securityScheme_description BasicAuth desc
     *   @securityScheme_openIdConnectUrl BasicAuth https://oidc
     *   @securityScheme_oauth2MetadataUrl BasicAuth https://oauth
     *   @securityScheme_deprecated BasicAuth
    @break_tag2
     */
    class Client {
        /**
         * @summary Get Test Summary
         * @description Get Test Description
         * @tags tag1,tag2
         * @deprecated
         * @security OAuth2 [scope1,scope2]
         * @response_content 200 application/json
         */
        std::string getTest(int id, bool filter, double num_val, std::string str_val, const std::string& body) {
            std::string full_url = base_url + "/api/v1/test/<id>";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        }
        std::string deleteTest() {
            std::string full_url = base_url + "/api/v1/test2";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        }
        std::string putTest() {
            std::string full_url = base_url + "/api/v1/test3";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        }
        std::string getTest2() {
            std::string full_url = base_url + "/api/v1/test4";
        }
        std::string patchTest() {
            std::string full_url = base_url + "/api/v1/test5";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        }
        void emptyMethod() {}
    };

    /**
     * @contact_name Samuel
     * @license_name MIT
     * @servers
     * - https://only_url
     * @some_tag to break servers
     * @securitySchemes
     * - OtherScheme http
     * @some_tag to break schemes
     */
    class Client {};
    
    /**
     * @contact_email test
     * @license_url u
     */
    class Client {};
  )";

  auto spec = parse(client_code).operator*();
  std::cout << "Paths parsed:\n";
  for (const auto &p : *spec.paths) {
    std::cout << " - " << p.first << "\n";
  }

  assert(spec.info.title == "Parsed API Client");
  assert(spec.info.summary == "The summary");
  assert(spec.info.version == "1.0.0");
  assert(spec.info.description == "A great API");
  assert(spec.info.termsOfService == "https://terms");
  assert(spec.info.contact->name == "Samuel");
  assert(spec.info.contact->url == "https://samuel");
  assert(spec.info.contact->email == "test");
  assert(spec.info.license->name == "MIT");
  assert(spec.info.license->identifier == "MIT");
  assert(spec.info.license->url == "u");
  assert(spec.servers->at(0).url == "https://only_url");
  assert(spec.components.has_value() &&
         spec.components->securitySchemes.has_value());
  assert(spec.components->securitySchemes->at("OtherScheme").type == "http");

  assert(spec.paths.has_value());
  assert(spec.paths->contains("/api/v1/test/<id>"));
  assert(spec.paths->at("/api/v1/test/<id>").post.has_value());

  auto post_op = spec.paths->at("/api/v1/test/<id>").post.operator*();
  assert(post_op.operationId == "getTest");
  assert(post_op.summary == "Get Test Summary");
  assert(post_op.description == "Get Test Description");
  assert(post_op.tags.has_value() && post_op.tags->size() == 2 &&
         post_op.tags->at(0) == "tag1");
  assert(post_op.deprecated == true);
  assert(post_op.security.has_value() &&
         post_op.security->at(0).count("OAuth2") &&
         post_op.security->at(0).at("OAuth2")[0] == "scope1");
  assert(post_op.responses.has_value() &&
         post_op.responses->at("200").content->count("application/json"));
  assert(post_op.requestBody.has_value());
  assert(post_op.requestBody->description == "Parsed request body");
  assert(post_op.parameters.has_value());
  assert(post_op.parameters->size() == 4);
  assert(post_op.parameters->at(0).name == "id");
  assert(post_op.parameters->at(0).in == "path");
  assert(post_op.parameters->at(0).schema.has_value() &&
         post_op.parameters->at(0).schema->type == "integer");
  assert(post_op.parameters->at(1).name == "filter");
  assert(post_op.parameters->at(1).in == "query");
  assert(post_op.parameters->at(1).schema.has_value() &&
         post_op.parameters->at(1).schema->type == "boolean");
  assert(post_op.parameters->at(2).name == "num_val");
  assert(post_op.parameters->at(2).in == "query");
  assert(post_op.parameters->at(2).schema.has_value() &&
         post_op.parameters->at(2).schema->type == "number");
  assert(post_op.parameters->at(3).name == "str_val");
  assert(post_op.parameters->at(3).in == "query");
  assert(post_op.parameters->at(3).schema.has_value() &&
         post_op.parameters->at(3).schema->type == "string");

  assert(spec.paths->contains("/api/v1/test2"));
  assert(spec.paths->at("/api/v1/test2").delete_op.has_value());

  assert(spec.paths->contains("/api/v1/test3"));
  assert(spec.paths->at("/api/v1/test3").put.has_value());

  assert(spec.paths->contains("/api/v1/test4"));
  assert(spec.paths->at("/api/v1/test4").get.has_value());

  assert(spec.paths->contains("/api/v1/test5"));
  assert(spec.paths->at("/api/v1/test5").patch.has_value());

  std::string client_code2 = R"(
    /**
     * @contact_name Samuel
     * @contact_url u
     * @contact_email e
     * @license_name MIT
     * @license_identifier MIT
     * @license_url u
     * @servers
     * - https://srv
     * @some_tag
     * @securitySchemes
     * - BasicAuth http
     * @some_tag
     */
    class Client {};
  )";
  auto spec2 = parse(client_code2).operator*();
  assert(spec2.info.contact->name == "Samuel");

  std::string client_code3 = R"(
    /**
     * @contact_email e
     * @license_url u
     * @servers
     * - https://srv
     * @servers
     * @securitySchemes
     * - BasicAuth http
     * @securitySchemes
     */
    class Client {};
  )";
  auto spec3 = parse(client_code3).operator*();
  assert(spec3.info.contact->email == "e");

  std::cout << "client_sdk::test_parse passed.\n";
}

} // namespace cdd_cpp::client_sdk

// GCOV_EXCL_BR_STOP
