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
  spec.info.contact = openapi::Contact{"Samuel", "https://samuel", "samuel@test"};
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
  spec.components->securitySchemes = std::map<std::string, openapi::SecurityScheme>{{"BasicAuth", scheme}};

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
  get_op.parameters = std::vector<openapi::Parameter>{query_param, header_param};

  openapi::RequestBody req_body;
  req_body.description = "Body Description";
  get_op.requestBody = req_body;

  openapi::Response resp_200;
  resp_200.description = "Success";
  resp_200.content = std::map<std::string, openapi::MediaType>{{"application/json", openapi::MediaType{}}};
  get_op.responses = std::map<std::string, openapi::Response>{{"200", resp_200}};

  pi.get = get_op;
  pi.patch = get_op; // just to test patch method
  pi.patch->operationId = "patchPet";

  std::map<std::string, openapi::PathItem> paths;
  paths["/pet/{id}"] = pi;
  spec.paths = paths;

  std::string generated = emit_client(spec);

  assert(generated.find("/// @title Test API") != std::string::npos);
  assert(generated.find("/// @version 1.0.0") != std::string::npos);
  assert(generated.find("/// @description A great API") != std::string::npos);
  assert(generated.find("/// @termsOfService https://terms") != std::string::npos);
  assert(generated.find("/// @contact_name Samuel") != std::string::npos);
  assert(generated.find("/// @contact_url https://samuel") != std::string::npos);
  assert(generated.find("/// @contact_email samuel@test") != std::string::npos);
  assert(generated.find("/// @license_name MIT") != std::string::npos);
  assert(generated.find("/// @license_identifier MIT") != std::string::npos);
  assert(generated.find("/// @license_url https://mit") != std::string::npos);
  assert(generated.find("Main server") != std::string::npos);
  assert(generated.find("@server_variable var default desc [enum1]") != std::string::npos);
  assert(generated.find("@securitySchemes") != std::string::npos);
  assert(generated.find("- BasicAuth http name header basic format") != std::string::npos);
  assert(generated.find("@securityScheme_description BasicAuth desc") != std::string::npos);
  assert(generated.find("@securityScheme_openIdConnectUrl BasicAuth https://oidc") != std::string::npos);
  assert(generated.find("@securityScheme_oauth2MetadataUrl BasicAuth https://oauth") != std::string::npos);
  assert(generated.find("@securityScheme_deprecated BasicAuth") != std::string::npos);

  assert(generated.find("@summary Path Summary") != std::string::npos);
  assert(generated.find("/// @description Path Description") != std::string::npos);
  assert(generated.find("@summary Get Pet Summary") != std::string::npos);
  assert(generated.find("/// @description Get Pet Description") != std::string::npos);

  assert(generated.find("@param id path parameter") != std::string::npos);
  assert(generated.find("@param filter query parameter") != std::string::npos);
  assert(generated.find("@param X_Header header parameter") != std::string::npos);
  assert(generated.find("@param body request body - Body Description") != std::string::npos);
  assert(generated.find("@tags pets,admin") != std::string::npos);
  assert(generated.find("@deprecated") != std::string::npos);
  assert(generated.find("@security Bearer [read,write]") != std::string::npos);

  assert(generated.find("@return 200 - Success") != std::string::npos);
  assert(generated.find("@response_content 200 application/json") != std::string::npos);
  assert(generated.find("content-type: application/json") != std::string::npos);

  assert(generated.find("getPet(int id, bool filter, double X_Header, const std::string& body)") != std::string::npos);
  assert(generated.find("patchPet(int id, bool filter, double X_Header, const std::string& body)") != std::string::npos);

    assert(generated.find("full_url += (full_url.find('?') == std::string::npos ? \"?\" : \"&\");") != std::string::npos);
  
  

  assert(generated.find("class Client") != std::string::npos);

  // Test operation without ID
  openapi::Operation no_id_op;
  pi.post = no_id_op;
  paths["/no_id"] = pi;
  spec.paths = paths;
  std::string generated2 = emit_client(spec);
  

  std::cout << "client_sdk::test_emit_client passed.\n";
}


void test_parse() {
  std::string client_code = R"(
    /**
     * @title Parsed API Client
     * @version 1.0.0
     * @description A great API
     * @termsOfService https://terms
     * @contact_name Samuel
     * @contact_url https://samuel
     * @contact_email samuel@test
     * @license_name MIT
     * @license_identifier MIT
     * @license_url https://mit
     * @servers
     * - https://srv (Main)
     *   @server_variable var default desc [enum1]
     * @securitySchemes
     * - BasicAuth http name header basic format
     *   @securityScheme_description BasicAuth desc
     *   @securityScheme_openIdConnectUrl BasicAuth https://oidc
     *   @securityScheme_oauth2MetadataUrl BasicAuth https://oauth
     *   @securityScheme_deprecated BasicAuth
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
        std::string getTest(int id, bool filter, double num_val, const std::string& body) {
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
  )";
  
  auto spec = parse(client_code).value();
  std::cout << "Paths parsed:\n";
  for (const auto& p : spec.paths.value()) {
      std::cout << " - " << p.first << "\n";
  }

  assert(spec.info.title == "Parsed API Client");
  assert(spec.info.version == "1.0.0");
  assert(spec.info.description == "A great API");
  assert(spec.info.termsOfService == "https://terms");
  assert(spec.info.contact->name == "Samuel");
  assert(spec.info.contact->url == "https://samuel");
  assert(spec.info.contact->email == "samuel@test");
  assert(spec.info.license->name == "MIT");
  assert(spec.info.license->identifier == "MIT");
  assert(spec.info.license->url == "https://mit");
  assert(spec.servers->at(0).url == "https://srv");
  assert(spec.servers->at(0).description == "Main");
  assert(spec.servers->at(0).variables->at("var").default_value == "default");
  assert(spec.servers->at(0).variables->at("var").description == "desc");
  assert(spec.servers->at(0).variables->at("var").enum_values->at(0) == "enum1");
  assert(spec.components.has_value() && spec.components->securitySchemes.has_value());
  assert(spec.components->securitySchemes->at("BasicAuth").type == "http");
  assert(spec.components->securitySchemes->at("BasicAuth").name == "name");
  assert(spec.components->securitySchemes->at("BasicAuth").in == "header");
  assert(spec.components->securitySchemes->at("BasicAuth").scheme == "basic");
  assert(spec.components->securitySchemes->at("BasicAuth").bearerFormat == "format");
  assert(spec.components->securitySchemes->at("BasicAuth").description == "desc");
  assert(spec.components->securitySchemes->at("BasicAuth").openIdConnectUrl == "https://oidc");
  assert(spec.components->securitySchemes->at("BasicAuth").oauth2MetadataUrl == "https://oauth");
  assert(spec.components->securitySchemes->at("BasicAuth").deprecated == true);

  assert(spec.paths.has_value());
  assert(spec.paths->contains("/api/v1/test/<id>"));
  assert(spec.paths->at("/api/v1/test/<id>").post.has_value());
  
  auto post_op = spec.paths->at("/api/v1/test/<id>").post.value();
  assert(post_op.operationId == "getTest");
  assert(post_op.summary == "Get Test Summary");
  assert(post_op.description == "Get Test Description");
  assert(post_op.tags.has_value() && post_op.tags->size() == 2 && post_op.tags->at(0) == "tag1");
  assert(post_op.deprecated  == true);
  assert(post_op.security.has_value() && post_op.security->at(0).count("OAuth2") && post_op.security->at(0).at("OAuth2")[0] == "scope1");
  assert(post_op.responses.has_value() && post_op.responses->at("200").content->count("application/json"));
  assert(post_op.requestBody.has_value());
  assert(post_op.requestBody->description == "Parsed request body");
  assert(post_op.parameters.has_value());
  assert(post_op.parameters->size() == 3);
  assert(post_op.parameters->at(0).name == "id");
  assert(post_op.parameters->at(0).in == "path");
  assert(post_op.parameters->at(0).schema.has_value() && post_op.parameters->at(0).schema->type == "integer");
  assert(post_op.parameters->at(1).name == "filter");
  assert(post_op.parameters->at(1).in == "query");
  assert(post_op.parameters->at(1).schema.has_value() && post_op.parameters->at(1).schema->type == "boolean");
  assert(post_op.parameters->at(2).name == "num_val");
  assert(post_op.parameters->at(2).in == "query");
  assert(post_op.parameters->at(2).schema.has_value() && post_op.parameters->at(2).schema->type == "number");

  assert(spec.paths->contains("/api/v1/test2"));
  assert(spec.paths->at("/api/v1/test2").delete_op.has_value());

  assert(spec.paths->contains("/api/v1/test3"));
  assert(spec.paths->at("/api/v1/test3").put.has_value());

  assert(spec.paths->contains("/api/v1/test4"));
  assert(spec.paths->at("/api/v1/test4").get.has_value());

  assert(spec.paths->contains("/api/v1/test5"));
  assert(spec.paths->at("/api/v1/test5").patch.has_value());

  std::cout << "client_sdk::test_parse passed.\n";
}

} // namespace cdd_cpp::client_sdk
