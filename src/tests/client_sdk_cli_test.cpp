// GCOV_EXCL_BR_START

#include "../client_sdk_cli/emit.hpp"
#include "../client_sdk_cli/parse.hpp"
#include <cassert>
#include <iostream>

// GCOV_EXCL_BR_START
namespace cdd_cpp::client_sdk_cli {

void test_emit_cli() {
  openapi::OpenAPI spec;
  spec.info.title = "Test CLI";               // GCOV_EXCL_BR_LINE
  spec.info.version = "1.0.0";                // GCOV_EXCL_BR_LINE
  spec.info.description = "A great CLI";      // GCOV_EXCL_BR_LINE
  spec.info.termsOfService = "https://terms"; // GCOV_EXCL_BR_LINE
  spec.info.contact =                         // GCOV_EXCL_BR_LINE
      openapi::Contact{"Samuel", "https://samuel",
                       "samuel@test"}; // GCOV_EXCL_BR_LINE
  spec.info.license =
      openapi::License{"MIT", "MIT", "https://mit"}; // GCOV_EXCL_BR_LINE

  openapi::ServerVariable sv;
  sv.default_value = "default";                       // GCOV_EXCL_BR_LINE
  sv.description = "desc";                            // GCOV_EXCL_BR_LINE
  sv.enum_values = std::vector<std::string>{"enum1"}; // GCOV_EXCL_BR_LINE
  openapi::Server s1;
  s1.url = "https://api.example.com"; // GCOV_EXCL_BR_LINE
  s1.description = "Main server";     // GCOV_EXCL_BR_LINE
  s1.variables = std::map<std::string, openapi::ServerVariable>{
      {"var", sv}};                                // GCOV_EXCL_BR_LINE
  spec.servers = std::vector<openapi::Server>{s1}; // GCOV_EXCL_BR_LINE

  openapi::SecurityScheme scheme;
  scheme.type = "http";                       // GCOV_EXCL_BR_LINE
  scheme.name = "name";                       // GCOV_EXCL_BR_LINE
  scheme.in = "header";                       // GCOV_EXCL_BR_LINE
  scheme.scheme = "basic";                    // GCOV_EXCL_BR_LINE
  scheme.bearerFormat = "format";             // GCOV_EXCL_BR_LINE
  scheme.description = "desc";                // GCOV_EXCL_BR_LINE
  scheme.openIdConnectUrl = "https://oidc";   // GCOV_EXCL_BR_LINE
  scheme.oauth2MetadataUrl = "https://oauth"; // GCOV_EXCL_BR_LINE
  scheme.deprecated = true;                   // GCOV_EXCL_BR_LINE
  spec.components = openapi::Components{};    // GCOV_EXCL_BR_LINE
  spec.components->securitySchemes =          // GCOV_EXCL_BR_LINE
      std::map<std::string, openapi::SecurityScheme>{
          {"BasicAuth", scheme}}; // GCOV_EXCL_BR_LINE

  openapi::PathItem pi;
  openapi::Operation get_op;
  get_op.operationId = "getPet";                 // GCOV_EXCL_BR_LINE
  get_op.summary = "Get a pet";                  // GCOV_EXCL_BR_LINE
  get_op.description = "Gets a pet by id";       // GCOV_EXCL_BR_LINE
  get_op.tags = std::vector<std::string>{"cli"}; // GCOV_EXCL_BR_LINE
  get_op.deprecated = true;                      // GCOV_EXCL_BR_LINE
  openapi::SecurityRequirement sr;
  sr["ApiKey"] = std::vector<std::string>{}; // GCOV_EXCL_BR_LINE
  get_op.security =
      std::vector<openapi::SecurityRequirement>{sr}; // GCOV_EXCL_BR_LINE
  openapi::Response resp;
  resp.description = "OK"; // GCOV_EXCL_BR_LINE
  resp.content = std::map<std::string, openapi::MediaType>{
      // GCOV_EXCL_BR_LINE
      {"application/json", openapi::MediaType{}}}; // GCOV_EXCL_BR_LINE
  get_op.responses = std::map<std::string, openapi::Response>{
      {"200", resp}}; // GCOV_EXCL_BR_LINE

  openapi::Parameter p1, p2, p3;
  p1.name = "id\"\\\n\r";    // GCOV_EXCL_BR_LINE
  p1.in = "query";           // GCOV_EXCL_BR_LINE
  p1.description = "The id"; // GCOV_EXCL_BR_LINE
  p1.example = "123";        // GCOV_EXCL_BR_LINE
  p1.deprecated = true;

  p2.name = "filter"; // GCOV_EXCL_BR_LINE
  p2.in = "path";     // GCOV_EXCL_BR_LINE

  p3.name = "limit"; // GCOV_EXCL_BR_LINE
  p3.in = "unknown"; // GCOV_EXCL_BR_LINE

  get_op.parameters =
      std::vector<openapi::Parameter>{p1, p2, p3}; // GCOV_EXCL_BR_LINE

  openapi::RequestBody reqBody;
  reqBody.description = "The body"; // GCOV_EXCL_BR_LINE
  get_op.requestBody = reqBody;     // GCOV_EXCL_BR_LINE

  pi.get = get_op; // GCOV_EXCL_BR_LINE

  openapi::PathItem pi2;
  openapi::Operation get_op2;
  get_op2.operationId = "getPetById"; // GCOV_EXCL_BR_LINE
  pi2.get = get_op2;                  // GCOV_EXCL_BR_LINE

  openapi::PathItem pi3;
  openapi::Operation get_op3;
  get_op3.operationId = "getPetByStatus"; // GCOV_EXCL_BR_LINE
  pi3.get = get_op3;                      // GCOV_EXCL_BR_LINE

  spec.paths = std::map<std::string, openapi::PathItem>{
      // GCOV_EXCL_BR_LINE
      {"/pet", pi},
      {"/pet/{petId}", pi2},
      {"/pet/findByStatus", pi3}}; // GCOV_EXCL_BR_LINE

  auto generated_files = emit_cli(spec);
  auto generated_files_with_tests = emit_cli(spec, false, false, true);
  auto generated_files_with_options = emit_cli(spec, true, true, false);

  std::string generated = "";                           // GCOV_EXCL_BR_LINE
  for (const auto &[name, content] : generated_files) { // GCOV_EXCL_BR_LINE
    generated += content + "\n";                        // GCOV_EXCL_BR_LINE
  }
  std::string generated2 = ""; // GCOV_EXCL_BR_LINE
  for (const auto &[name, content] :
       generated_files_with_tests) {            // GCOV_EXCL_BR_LINE
    generated2 += name + "\n" + content + "\n"; // GCOV_EXCL_BR_LINE
  }
  std::string generated3 = ""; // GCOV_EXCL_BR_LINE
  for (const auto &[name, content] :
       generated_files_with_options) { // GCOV_EXCL_BR_LINE
    generated3 += content + "\n";      // GCOV_EXCL_BR_LINE
  }

  assert(generated.find("Test CLI") != std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated.find("/// @tags cli") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated.find("/// @deprecated") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated.find("@security ApiKey") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(
      generated.find(
          "/// @response_content 200 application/json") != // GCOV_EXCL_BR_LINE
      std::string::npos);
  assert(generated.find("https://api.example.com") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated.find("@securitySchemes") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(
      generated.find(
          "- BasicAuth http name header basic format") != // GCOV_EXCL_BR_LINE
      std::string::npos);
  assert(
      generated.find(
          "@securityScheme_description BasicAuth desc") != // GCOV_EXCL_BR_LINE
      std::string::npos);
  assert(generated.find( // GCOV_EXCL_BR_LINE
             "@securityScheme_openIdConnectUrl BasicAuth https://oidc") !=
         std::string::npos);
  assert(generated.find( // GCOV_EXCL_BR_LINE
             "@securityScheme_oauth2MetadataUrl BasicAuth https://oauth") !=
         std::string::npos);
  assert(generated.find(
             "@securityScheme_deprecated BasicAuth") != // GCOV_EXCL_BR_LINE
         std::string::npos);
  assert(generated.find("--id") != std::string::npos);   // GCOV_EXCL_BR_LINE
  assert(generated.find("--body") != std::string::npos); // GCOV_EXCL_BR_LINE

  assert(generated2.find("add_subdirectory(tests)") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated2.find("tests/cli_test.cpp") !=
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(generated2.find("cd tests && ./cli_test") !=
         std::string::npos); // GCOV_EXCL_BR_LINE

  openapi::OpenAPI spec_empty;
  spec_empty.paths = // GCOV_EXCL_BR_LINE
      std::map<std::string, openapi::PathItem>{
          {"/{id}", openapi::PathItem{}}}; // GCOV_EXCL_BR_LINE
  openapi::Operation get_op_empty;
  get_op_empty.operationId = "getEmpty";            // GCOV_EXCL_BR_LINE
  spec_empty.paths->at("/{id}").get = get_op_empty; // GCOV_EXCL_BR_LINE
  emit_cli(spec_empty);

  openapi::OpenAPI spec_str;
  spec_str.paths = std::map<std::string, openapi::PathItem>{
      // GCOV_EXCL_BR_LINE
      {"/\"\\\n\r", openapi::PathItem{}}}; // GCOV_EXCL_BR_LINE
  emit_cli(spec_str);

  std::cout << "client_sdk_cli::test_emit_cli passed.\n"; // GCOV_EXCL_BR_LINE
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
  auto spec = parse(cli_code).operator*();             // GCOV_EXCL_BR_LINE
  assert(spec.info.title == "Parsed CLI Tools");       // GCOV_EXCL_BR_LINE
  assert(spec.info.version == "1.2.3");                // GCOV_EXCL_BR_LINE
  assert(spec.info.description == "Description test"); // GCOV_EXCL_BR_LINE
  assert(spec.servers.has_value() &&                   // GCOV_EXCL_BR_LINE
         spec.servers->at(0).url == "https://custom.api.com");
  assert(spec.paths.has_value());                           // GCOV_EXCL_BR_LINE
  assert(spec.paths->contains("/test_cmd"));                // GCOV_EXCL_BR_LINE
  auto op = spec.paths->at("/test_cmd").post;               // GCOV_EXCL_BR_LINE
  assert(op.has_value());                                   // GCOV_EXCL_BR_LINE
  assert(op->summary == "@summary Summary of test cmd");    // GCOV_EXCL_BR_LINE
  assert(op->tags.has_value() && op->tags->at(0) == "cli"); // GCOV_EXCL_BR_LINE
  assert(op->deprecated == true);                           // GCOV_EXCL_BR_LINE
  assert(op->security.has_value() &&
         op->security->at(0).count("ApiKey")); // GCOV_EXCL_BR_LINE
  assert(op->responses.has_value() &&          // GCOV_EXCL_BR_LINE
         op->responses->at("200").content->count("application/json"));
  assert(op->parameters.has_value());             // GCOV_EXCL_BR_LINE
  assert(op->parameters->at(0).name == "filter"); // GCOV_EXCL_BR_LINE
  assert(op->requestBody.has_value());            // GCOV_EXCL_BR_LINE

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
  auto spec2 = parse(cli_code2).operator*(); // GCOV_EXCL_BR_LINE
  assert(spec2.info.contact->email == "e");  // GCOV_EXCL_BR_LINE
  assert(spec2.info.license->url == "u");    // GCOV_EXCL_BR_LINE

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
  auto spec3 = parse(cli_code3).operator*();         // GCOV_EXCL_BR_LINE
  assert(spec3.info.contact->email == "just_email"); // GCOV_EXCL_BR_LINE
  assert(spec3.info.license->url == "just_url");     // GCOV_EXCL_BR_LINE

  std::cout << "client_sdk_cli::test_parse passed.\n"; // GCOV_EXCL_BR_LINE
}

} // namespace cdd_cpp::client_sdk_cli
// GCOV_EXCL_BR_STOP
