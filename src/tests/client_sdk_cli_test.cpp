#include "../client_sdk_cli/emit.hpp"
#include "../client_sdk_cli/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::client_sdk_cli {

void test_emit_cli() {
  openapi::OpenAPI spec;
  spec.info.title = "Test CLI";
  spec.info.version = "1.0.0";
  spec.info.description = "A great CLI";
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
  openapi::Operation get_op;
  get_op.operationId = "getPet";
  get_op.summary = "Get a pet";
  get_op.description = "Gets a pet by id";
  get_op.tags = std::vector<std::string>{"cli"};
  get_op.deprecated = true;
  openapi::SecurityRequirement sr;
  sr["ApiKey"] = std::vector<std::string>{};
  get_op.security = std::vector<openapi::SecurityRequirement>{sr};
  openapi::Response resp;
  resp.description = "OK";
  resp.content = std::map<std::string, openapi::MediaType>{{"application/json", openapi::MediaType{}}};
  get_op.responses = std::map<std::string, openapi::Response>{{"200", resp}};
  
  openapi::Parameter p1;
  p1.name = "id";
  p1.in = "query";
  get_op.parameters = std::vector<openapi::Parameter>{p1};
  
  openapi::RequestBody reqBody;
  reqBody.description = "The body";
  get_op.requestBody = reqBody;
  
  pi.get = get_op;
  spec.paths = std::map<std::string, openapi::PathItem>{{"/pet", pi}};

  std::string generated = emit_cli(spec);

  assert(generated.find("Test CLI") != std::string::npos);
  assert(generated.find("@tags cli") != std::string::npos);
  assert(generated.find("@deprecated") != std::string::npos);
  assert(generated.find("@security ApiKey") != std::string::npos);
  assert(generated.find("@response_content 200 application/json") != std::string::npos);
  assert(generated.find("https://api.example.com") != std::string::npos);
  assert(generated.find("@securitySchemes") != std::string::npos);
  assert(generated.find("- BasicAuth http name header basic format") != std::string::npos);
  assert(generated.find("@securityScheme_description BasicAuth desc") != std::string::npos);
  assert(generated.find("@securityScheme_openIdConnectUrl BasicAuth https://oidc") != std::string::npos);
  assert(generated.find("@securityScheme_oauth2MetadataUrl BasicAuth https://oauth") != std::string::npos);
  assert(generated.find("@securityScheme_deprecated BasicAuth") != std::string::npos);
  assert(generated.find("--id") != std::string::npos);
  assert(generated.find("--body") != std::string::npos);
  
  std::cout << "client_sdk_cli::test_emit_cli passed.\n";
}

void test_parse() {
  std::string cli_code = R"(
    /**
     * @title Parsed CLI Tools
     * @version 1.2.3
     * @description Description test
     * @termsOfService https://terms
     * @contact_name Samuel
     * @contact_url https://samuel
     * @contact_email samuel@test
     * @license_name MIT
     * @license_identifier MIT
     * @license_url https://mit
     * @servers
     * - https://custom.api.com (Main)
     *   @server_variable var default desc [enum1]
     * @securitySchemes
     * - BasicAuth http name header basic format
     *   @securityScheme_description BasicAuth desc
     *   @securityScheme_openIdConnectUrl BasicAuth https://oidc
     *   @securityScheme_oauth2MetadataUrl BasicAuth https://oauth
     *   @securityScheme_deprecated BasicAuth
     */
    if (argc < 2) { std::cerr << "Parsed CLI Tools\n"; return 1; }
    std::cerr << "v1.2.3\n";
    std::cerr << "Description test\n" ; // description
    std::string server = "https://custom.api.com";
    if (command == "test_cmd") {
        // @summary Summary of test cmd
        // @tags cli,test
        // @deprecated
        // @security ApiKey [scope1,scope2]
        // @response_content 200 application/json
        if (arg == "--filter") { }
        if (arg == "--body") { }
    }
  )";
  auto spec = parse(cli_code).value();
  assert(spec.info.title == "Parsed CLI Tools\\n");
  assert(spec.info.version == "1.2.3");
  assert(spec.info.description == "Description test");
  assert(spec.servers.has_value() && spec.servers->at(0).url == "https://custom.api.com");
  assert(spec.paths.has_value());
  assert(spec.paths->contains("/test_cmd"));
  auto op = spec.paths->at("/test_cmd").post;
  assert(op.has_value());
  assert(op->summary == "Summary of test cmd");
  assert(op->tags.has_value() && op->tags->at(0) == "cli");
  assert(op->deprecated == true);
  assert(op->security.has_value() && op->security->at(0).count("ApiKey"));
  assert(op->responses.has_value() && op->responses->at("200").content->count("application/json"));
  assert(op->parameters.has_value());
  assert(op->parameters->at(0).name == "filter");
  assert(op->requestBody.has_value());
  
  std::cout << "client_sdk_cli::test_parse passed.\n";
}

} // namespace cdd_cpp::client_sdk_cli