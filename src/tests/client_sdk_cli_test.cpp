//

#include "../client_sdk_cli/emit.hpp"
#include "../client_sdk_cli/parse.hpp"
#include <cassert>
#include <iostream>

//
namespace cdd_cpp::client_sdk_cli {

void test_client_sdk_cli_emit_details() {
  openapi::OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";
  spec.servers = std::vector<openapi::Server>{};

  openapi::PathItem pathItem;
  openapi::Operation op;
  op.operationId = "testOp";
  openapi::Parameter p;
  p.name = "reqP";
  p.required = true;
  op.parameters = std::vector<openapi::Parameter>{p};
  openapi::RequestBody reqBody;
  op.requestBody = reqBody;
  pathItem.get = op;

  openapi::Operation op_body_only;
  op_body_only.operationId = "bodyOnly";
  op_body_only.requestBody = reqBody;
  pathItem.post = op_body_only;

  spec.paths = std::map<std::string, openapi::PathItem>{{"/path", pathItem}};

  auto out = cdd_cpp::client_sdk_cli::emit_cli(spec, false, false, false);
  assert(out.contains("src/generated_cli.cpp"));
  assert(out.at("src/generated_cli.cpp").find("--reqP") != std::string::npos);
  assert(out.at("src/generated_cli.cpp").find("bodyOnly") != std::string::npos);
}

void test_client_sdk_cli_emit_details2() {
  openapi::OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  openapi::PathItem pathItem;
  openapi::Operation op;
  op.operationId = "testOp";
  openapi::Parameter p;
  p.name = "reqP";
  p.required = true;
  op.parameters = std::vector<openapi::Parameter>{
      p, openapi::Parameter{"reqP2", "query", "", true}};
  openapi::RequestBody reqBody;
  op.requestBody = reqBody;
  pathItem.get = op;

  spec.paths = std::map<std::string, openapi::PathItem>{
      {"/path/{id}/subpath",
       pathItem}}; // This forces path_params logic to be hit

  auto out = cdd_cpp::client_sdk_cli::emit_cli(spec, false, false, false);
  assert(out.contains("src/generated_cli.cpp"));
  assert(out.at("src/generated_cli.cpp").find("path_params[\"id\"]") !=
         std::string::npos);
}

void test_client_sdk_cli_emit_details3() {
  openapi::OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  openapi::PathItem pathItem;
  openapi::Operation op;
  op.operationId = "testOp";
  openapi::Parameter p;
  p.name = "reqP";
  p.required = true;
  op.parameters = std::vector<openapi::Parameter>{
      p, openapi::Parameter{"reqP2", "query", "", true}};
  openapi::RequestBody reqBody;
  op.requestBody = reqBody;
  pathItem.get = op;

  spec.paths = std::map<std::string, openapi::PathItem>{{"/path", pathItem}};

  // This will trigger the mcp call tool path and require multiple params, and
  // hit those std::cout comma lines.
  auto out = cdd_cpp::client_sdk_cli::emit_cli(spec, false, false, false);
  assert(out.contains("src/generated_cli.cpp"));
  assert(out.at("src/generated_cli.cpp").find("--reqP2") != std::string::npos);
}

void test_client_sdk_cli_emit_details4() {
  openapi::OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  openapi::PathItem pathItem;
  openapi::Operation op;
  op.operationId = "testOp";
  openapi::Parameter p;
  p.name = "reqP";
  p.required = true;
  op.parameters = std::vector<openapi::Parameter>{
      p, openapi::Parameter{"reqP2", "query", "", true}};
  openapi::RequestBody reqBody;
  reqBody.required = true; // Make body required too!
  op.requestBody = reqBody;
  pathItem.get = op;

  spec.paths = std::map<std::string, openapi::PathItem>{{"/path", pathItem}};

  // This will trigger the mcp call tool path and require multiple params, and
  // hit those std::cout comma lines.
  auto out = cdd_cpp::client_sdk_cli::emit_cli(spec, false, false, false);
  assert(out.contains("src/generated_cli.cpp"));
  assert(out.at("src/generated_cli.cpp").find("\\\"body\\\"") !=
         std::string::npos);
}

void test_emit_cli() {
  test_client_sdk_cli_emit_details();
  test_client_sdk_cli_emit_details2();
  test_client_sdk_cli_emit_details3();
  test_client_sdk_cli_emit_details4();

  openapi::OpenAPI spec;
  spec.info.title = "Test CLI";
  spec.info.version = "1.0.0";
  spec.info.description = "A great CLI";
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
  resp.content = std::map<std::string, openapi::MediaType>{
      {"application/json", openapi::MediaType{}}};
  get_op.responses = std::map<std::string, openapi::Response>{{"200", resp}};

  openapi::Parameter p1, p2, p3;
  p1.name = "id\"\\\n\r";
  p1.in = "query";
  p1.description = "The id";
  p1.example = "123";
  p1.deprecated = true;

  p2.name = "filter";
  p2.in = "path";

  p3.name = "limit";
  p3.in = "unknown";

  get_op.parameters = std::vector<openapi::Parameter>{p1, p2, p3};

  openapi::RequestBody reqBody;
  reqBody.description = "The body";
  get_op.requestBody = reqBody;

  pi.get = get_op;

  openapi::PathItem pi2;
  openapi::Operation get_op2;
  get_op2.operationId = "getPetById";
  pi2.get = get_op2;

  openapi::PathItem pi3;
  openapi::Operation get_op3;
  get_op3.operationId = "getPetByStatus";
  pi3.get = get_op3;

  spec.paths = std::map<std::string, openapi::PathItem>{
      {"/pet", pi}, {"/pet/{petId}", pi2}, {"/pet/findByStatus", pi3}};

  auto generated_files = emit_cli(spec);
  auto generated_files_with_tests = emit_cli(spec, false, false, true);
  auto generated_files_with_options = emit_cli(spec, true, true, false);

  std::string generated = "";
  for (const auto &[name, content] : generated_files) {
    generated += content + "\n";
  }
  std::string generated2 = "";
  for (const auto &[name, content] : generated_files_with_tests) {
    generated2 += name + "\n" + content + "\n";
  }
  std::string generated3 = "";
  for (const auto &[name, content] : generated_files_with_options) {
    generated3 += content + "\n";
  }

  assert(generated.find("Test CLI") != std::string::npos);
  assert(generated.find("/// @tags cli") != std::string::npos);
  assert(generated.find("/// @deprecated") != std::string::npos);
  assert(generated.find("@security ApiKey") != std::string::npos);
  assert(generated.find("/// @response_content 200 application/json") !=
         std::string::npos);
  assert(generated.find("https://api.example.com") != std::string::npos);
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
  assert(generated.find("--id") != std::string::npos);
  assert(generated.find("--body") != std::string::npos);

  assert(generated2.find("add_subdirectory(tests)") != std::string::npos);
  assert(generated2.find("tests/cli_test.cpp") != std::string::npos);
  assert(generated2.find("cd tests && ./cli_test") != std::string::npos);
  assert(!generated_files_with_options.contains(".github/workflows/ci.yml"));
  assert(!generated_files_with_options.contains("CMakeLists.txt"));
  assert(!generated3.empty());

  openapi::OpenAPI spec_empty;
  spec_empty.paths =
      std::map<std::string, openapi::PathItem>{{"/{id}", openapi::PathItem{}}};
  openapi::Operation get_op_empty;
  get_op_empty.operationId = "getEmpty";
  spec_empty.paths->at("/{id}").get = get_op_empty;
  auto empty_cli = emit_cli(spec_empty);
  assert(empty_cli.contains("src/generated_cli.cpp"));
  assert(empty_cli.at("src/generated_cli.cpp").find("getEmpty") !=
         std::string::npos);

  openapi::OpenAPI spec_str;
  spec_str.paths = std::map<std::string, openapi::PathItem>{
      {"/\"\\\n\r", openapi::PathItem{}}};
  auto str_cli = emit_cli(spec_str);
  assert(str_cli.contains("src/generated_cli.cpp"));

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
        if (arg == "--filter") 
        if (arg == "--body") 
    }
  )";
  auto spec = parse(cli_code).operator*();
  assert(spec.info.title == "Parsed CLI Tools");
  assert(spec.info.version == "1.2.3");
  assert(spec.info.description == "Description test");
  assert(spec.servers.has_value() &&
         spec.servers->at(0).url == "https://custom.api.com");
  assert(spec.paths.has_value());
  assert(spec.paths->contains("/test_cmd"));
  auto op = spec.paths->at("/test_cmd").post;
  assert(op.has_value());
  assert(op->summary == "@summary Summary of test cmd");
  assert(op->tags.has_value() && op->tags->at(0) == "cli");
  assert(op->deprecated == true);
  assert(op->security.has_value() && op->security->at(0).count("ApiKey"));
  assert(op->responses.has_value() &&
         op->responses->at("200").content->count("application/json"));
  assert(op->parameters.has_value());
  assert(op->parameters->at(0).name == "filter");
  assert(op->requestBody.has_value());

  std::string cli_code2 = R"(
    /**
     * @contact_email e
     * @contact_url u
     * @license_identifier MIT
     * @license_url u
     * @servers
     * - https://srv (Main)
     *   @server_variable var default desc [enum1]
     * @break_servers
     * @securitySchemes
     * - BasicAuth http
     * @securitySchemes
     */
    if (command == "test2") {
        // @param filter Filter
        // @param_example filter 1
        // @param_deprecated filter
        if (arg == "--filter")
    }
    )";
  auto spec2 = parse(cli_code2).operator*();
  assert(spec2.info.contact->email == "e");
  assert(spec2.info.license->url == "u");

  std::string cli_code3 = R"(
  /**
   * @contact_email just_email
   * @license_url just_url
   * @servers
   * - https://srv
   * @some_other_tag
   */
  if (command == "test3") {
      // @param filter 
  }
  )";
  auto spec3 = parse(cli_code3).operator*();
  assert(spec3.info.contact->email == "just_email");
  assert(spec3.info.license->url == "just_url");

  // Round-trip emitted modern CLI code with handle_<op_id> functions
  openapi::OpenAPI emit_spec;
  emit_spec.info.title = "RoundTrip CLI";
  emit_spec.info.version = "1.0.0";
  openapi::PathItem rt_pi;
  openapi::Operation rt_op;
  rt_op.operationId = "createUser";
  openapi::Parameter rt_p1;
  rt_p1.name = "orgId";
  rt_p1.in = "path";
  rt_p1.required = true;
  rt_p1.description = "Organization ID";
  rt_p1.example = "org_123";
  rt_p1.deprecated = true;
  openapi::Parameter rt_p2;
  rt_p2.name = "verbose";
  rt_p2.in = "query";
  openapi::Parameter rt_p3;
  rt_p3.name = "X_Token";
  rt_p3.in = "header";
  openapi::Parameter rt_p4;
  rt_p4.name = "session_id";
  rt_p4.in = "cookie";
  rt_op.parameters =
      std::vector<openapi::Parameter>{rt_p1, rt_p2, rt_p3, rt_p4};
  rt_op.requestBody = openapi::RequestBody{};
  rt_pi.post = rt_op;
  emit_spec.paths =
      std::map<std::string, openapi::PathItem>{{"/orgs/{orgId}/users", rt_pi}};
  auto emitted_files = emit_cli(emit_spec);
  auto rt_parsed = parse(emitted_files["src/generated_cli.cpp"]);
  assert(rt_parsed.has_value());
  assert(rt_parsed->paths->contains("/orgs/{orgId}/users"));
  assert(rt_parsed->paths->at("/orgs/{orgId}/users").post.has_value());
  [[maybe_unused]] const auto &parsed_rt_op =
      *rt_parsed->paths->at("/orgs/{orgId}/users").post;
  assert(parsed_rt_op.operationId == "createUser");
  assert(parsed_rt_op.requestBody.has_value());
  assert(parsed_rt_op.parameters.has_value() &&
         parsed_rt_op.parameters->size() == 4);
  assert(parsed_rt_op.parameters->at(0).in == "path");
  assert(parsed_rt_op.parameters->at(0).example == "org_123");
  assert(parsed_rt_op.parameters->at(0).deprecated == true);

  std::string cli_edge = R"(
    void handle_noRouteOp(cdd_cli::Client& client, std::string undocumentedParam) {}
    if (command == "nocomment") { int x = 0; }
  )";
  auto edge_parsed = parse(cli_edge);
  assert(edge_parsed.has_value());
  assert(edge_parsed->paths->contains("/noRouteOp"));
  assert(edge_parsed->paths->contains("/nocomment"));

  std::cout << "client_sdk_cli::test_parse passed.\n";
}

} // namespace cdd_cpp::client_sdk_cli
