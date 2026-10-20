/// @file struct_test.cpp
/// @brief Value-semantics, deep-equality, and strong-typing tests for OpenAPI,
/// MCP, and Arazzo models.

#include "../arazzo/models.hpp"
#include "../mcp/models.hpp"
#include "../openapi/models.hpp"
#include <cassert>
#include <utility>

namespace cdd_cpp::struct_tests {

/// @brief Verify copy/move construction, assignment, reflexivity, and
/// inequality on populated model instances.
/// @tparam T Model struct type supporting value semantics and `operator==`.
/// @param populated_instance Non-empty populated instance.
/// @param distinct_instance Distinct instance that must not compare equal.
template <typename T>
void verify_value_semantics(const T &populated_instance,
                            const T &distinct_instance) {
  assert(populated_instance == populated_instance);
  assert(!(populated_instance == distinct_instance));

  T copy_constructed(populated_instance);
  assert(copy_constructed == populated_instance);

  T move_constructed(std::move(copy_constructed));
  assert(move_constructed == populated_instance);

  T copy_assigned = distinct_instance;
  assert(!(copy_assigned == populated_instance));
  copy_assigned = move_constructed;
  assert(copy_assigned == populated_instance);

  T move_assigned = distinct_instance;
  move_assigned = std::move(copy_assigned);
  assert(move_assigned == populated_instance);
}

void test_structs_openapi() {
  using namespace cdd_cpp::openapi;

  verify_value_semantics(Reference{"#/components/schemas/Pet", "sum", "desc"},
                         Reference{"#/components/schemas/Tag", "sum", "desc"});
  verify_value_semantics(Contact{"Jane", "https://example.com", "j@e.com"},
                         Contact{});
  verify_value_semantics(License{"Apache-2.0", "Apache-2.0", "https://a.org"},
                         License{"MIT", std::nullopt, std::nullopt});
  verify_value_semantics(
      Info{"API", "sum", "desc", "tos", Contact{"Jane", {}, {}}, {}, "1.0.0"},
      Info{"Other", {}, {}, {}, {}, {}, "2.0.0"});
  verify_value_semantics(
      ServerVariable{std::vector<std::string>{"a", "b"}, "a", "desc"},
      ServerVariable{{}, "b", {}});
  verify_value_semantics(Server{"https://api.example.com", "prod", "main", {}},
                         Server{"https://staging.example.com", {}, {}, {}});
  verify_value_semantics(ExternalDocumentation{"docs", "https://docs.io"},
                         ExternalDocumentation{{}, "https://other.io"});
  verify_value_semantics(Tag{"users", "sum", "desc", "parent", "nav", {}},
                         Tag{"admin", {}, {}, {}, {}, {}});
  verify_value_semantics(
      Discriminator{"type", std::map<std::string, std::string>{{"a", "A"}},
                    "defaultA"},
      Discriminator{"kind", {}, {}});
  verify_value_semantics(XML{"item", "http://ns", "p", true, false}, XML{});

  // Deep equality across separately allocated shared_ptr trees on Schema
  Schema s1;
  s1.type = "object";
  s1.properties = std::make_shared<std::map<std::string, Schema>>(
      std::map<std::string, Schema>{{"id", Schema{"integer"}}});
  s1.items = std::make_shared<Schema>(Schema{"string"});
  s1.allOf = std::make_shared<std::vector<Schema>>(
      std::vector<Schema>{Schema{"string"}});

  Schema s1_clone;
  s1_clone.type = "object";
  s1_clone.properties = std::make_shared<std::map<std::string, Schema>>(
      std::map<std::string, Schema>{{"id", Schema{"integer"}}});
  s1_clone.items = std::make_shared<Schema>(Schema{"string"});
  s1_clone.allOf = std::make_shared<std::vector<Schema>>(
      std::vector<Schema>{Schema{"string"}});
  assert(s1.properties.get() != s1_clone.properties.get());
  assert(s1.items.get() != s1_clone.items.get());
  assert(s1 == s1_clone);

  Schema s2;
  s2.type = "object";
  s2.properties = std::make_shared<std::map<std::string, Schema>>(
      std::map<std::string, Schema>{{"id", Schema{"string"}}});
  verify_value_semantics(s1, s2);

  verify_value_semantics(
      Example{"sum", "desc", "val", JsonValue(42), "http://ext", {}},
      Example{});

  // Deep equality across separately allocated shared_ptr on Encoding &
  // MediaType
  Encoding enc1;
  enc1.contentType = "application/json";
  enc1.itemSchema = std::make_shared<Schema>(Schema{"string"});
  Encoding enc1_clone;
  enc1_clone.contentType = "application/json";
  enc1_clone.itemSchema = std::make_shared<Schema>(Schema{"string"});
  assert(enc1.itemSchema.get() != enc1_clone.itemSchema.get());
  assert(enc1 == enc1_clone);
  verify_value_semantics(enc1, Encoding{});

  MediaType mt1;
  mt1.schema = s1;
  mt1.encoding = std::make_shared<std::map<std::string, Encoding>>(
      std::map<std::string, Encoding>{{"field", enc1}});
  MediaType mt1_clone;
  mt1_clone.schema = s1_clone;
  mt1_clone.encoding = std::make_shared<std::map<std::string, Encoding>>(
      std::map<std::string, Encoding>{{"field", enc1_clone}});
  assert(mt1 == mt1_clone);
  verify_value_semantics(mt1, MediaType{});

  Header hdr1;
  hdr1.description = "X-Rate-Limit";
  hdr1.required = true;
  hdr1.schema = s1;
  verify_value_semantics(hdr1, Header{});

  RequestBody rb1;
  rb1.description = "payload";
  rb1.required = true;
  rb1.content["application/json"] = mt1;
  verify_value_semantics(rb1, RequestBody{});

  Link link1;
  link1.operationId = "getUser";
  link1.description = "user link";
  verify_value_semantics(link1, Link{});

  Response resp1;
  resp1.description = "OK";
  resp1.content = std::map<std::string, MediaType>{{"application/json", mt1}};
  verify_value_semantics(resp1, Response{"Not Found", {}, {}, {}, {}});

  Parameter param1;
  param1.name = "id";
  param1.in = "path";
  param1.required = true;
  param1.schema = Schema{"string"};
  verify_value_semantics(param1, Parameter{});

  OAuthFlow flow1;
  flow1.authorizationUrl = "https://auth";
  flow1.tokenUrl = "https://token";
  verify_value_semantics(flow1, OAuthFlow{});

  OAuthFlows flows1;
  flows1.authorizationCode = flow1;
  verify_value_semantics(flows1, OAuthFlows{});

  SecurityScheme sec1;
  sec1.type = "oauth2";
  sec1.flows = flows1;
  verify_value_semantics(sec1, SecurityScheme{});

  Operation op1;
  op1.operationId = "listPets";
  op1.responses = std::map<std::string, Response>{{"200", resp1}};
  op1.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"onData", {{"/cb", PathItem{}}}}});
  verify_value_semantics(op1, Operation{});

  PathItem pi1;
  pi1.summary = "pet operations";
  pi1.get = op1;
  pi1.additionalOperations = std::make_shared<std::map<std::string, Operation>>(
      std::map<std::string, Operation>{{"COPY", op1}});
  verify_value_semantics(pi1, PathItem{});

  Components comp1;
  comp1.schemas = std::map<std::string, Schema>{{"Pet", s1}};
  comp1.mediaTypes = std::make_shared<std::map<std::string, MediaType>>(
      std::map<std::string, MediaType>{{"application/json", mt1}});
  verify_value_semantics(comp1, Components{});

  OpenAPI oas1;
  oas1.openapi = "3.2.0";
  oas1.info.title = "Petstore";
  oas1.info.version = "1.0.0";
  oas1.paths = std::map<std::string, PathItem>{{"/pets", pi1}};
  oas1.components = comp1;
  verify_value_semantics(oas1, OpenAPI{});
}

void test_structs_mcp() {
  using namespace cdd_cpp::mcp;

  verify_value_semantics(Annotations{"user", 0.8}, Annotations{});
  verify_value_semantics(TextContent{"text", "hello", {}},
                         TextContent{"text", "world", {}});
  verify_value_semantics(ImageContent{"image", "base64", "image/png", {}},
                         ImageContent{});

  Resource res1;
  res1.uri = "https://r";
  res1.name = "name";
  res1.description = "desc";
  res1.mimeType = "text/plain";
  verify_value_semantics(res1, Resource{});

  ResourceTemplate rt1;
  rt1.uriTemplate = "https://{id}";
  rt1.name = "tpl";
  rt1.description = "desc";
  rt1.mimeType = "text/plain";
  verify_value_semantics(rt1, ResourceTemplate{});

  TextResourceContents trc1;
  trc1.uri = "https://r";
  trc1.mimeType = "text/plain";
  trc1.text = "body";
  verify_value_semantics(trc1, TextResourceContents{});

  BlobResourceContents brc1;
  brc1.blob = "blob";
  brc1.mimeType = "application/octet-stream";
  brc1.uri = "https://r";
  verify_value_semantics(brc1, BlobResourceContents{});

  Tool tool1;
  tool1.name = "my_tool";
  tool1.description = "desc";
  tool1.inputSchema = ToolInputSchema{"object", "{}", {{"a"}}};
  verify_value_semantics(tool1, Tool{});

  Prompt prompt1;
  prompt1.name = "my_prompt";
  prompt1.description = "desc";
  prompt1.arguments = std::vector<PromptArgument>{{"arg", "d", true}};
  verify_value_semantics(prompt1, Prompt{});

  Root root1;
  root1.uri = "file:///workspace";
  root1.name = "ws";
  verify_value_semantics(root1, Root{});

  verify_value_semantics(Implementation{"cdd-cpp", "0.0.4"}, Implementation{});

  JSONRPCErrorError err1{400, "Bad Request", "details"};
  verify_value_semantics(err1, JSONRPCErrorError{});
  verify_value_semantics(JSONRPCError{"2.0", "1", err1}, JSONRPCError{});

  TypedRequestId req_id_int = int64_t{42};
  TypedRequestId req_id_str = std::string("req-1");
  verify_value_semantics(req_id_int, req_id_str);

  CallToolResult ctr1;
  ctr1.content_json = "[]";
  ctr1.typed_content =
      std::vector<ContentBlockVariant>{TextContent{"text", "ok", {}}};
  verify_value_semantics(ctr1, CallToolResult{});

  // Test Arazzo model structs value semantics
  {
    using namespace cdd_cpp::arazzo;
    verify_value_semantics(
        Info{"Workflows", "sum", "desc", "1.0.0", std::nullopt},
        Info{"Other", std::nullopt, std::nullopt, "2.0.0", std::nullopt});
    verify_value_semantics(
        SourceDescription{"petstore", "./openapi.json",
                          SourceDescriptionType::OpenApi, std::nullopt},
        SourceDescription{"other", "./other.json", std::nullopt, std::nullopt});
    verify_value_semantics(Criterion{"$statusCode", "$statusCode == 200",
                                     CriterionType::Simple, std::nullopt},
                           Criterion{std::nullopt, "$statusCode == 404",
                                     std::nullopt, std::nullopt});
    Step step1;
    step1.stepId = "login";
    step1.operationId = "loginUser";
    Step step2;
    step2.stepId = "logout";
    step2.operationId = "logoutUser";
    verify_value_semantics(step1, step2);

    Workflow wf1;
    wf1.workflowId = "authFlow";
    wf1.steps = {step1};
    openapi::Schema wf_in;
    wf_in.type = "object";
    wf_in.properties = std::make_shared<std::map<std::string, openapi::Schema>>(
        std::map<std::string, openapi::Schema>{
            {"token", openapi::Schema{"string"}}});
    wf1.inputs = wf_in;

    Workflow wf2;
    wf2.workflowId = "otherFlow";
    wf2.steps = {step2};
    verify_value_semantics(wf1, wf2);

    Arazzo ar1;
    ar1.info.title = "T";
    ar1.info.version = "1.0.0";
    ar1.sourceDescriptions = {
        {"s", "./o.json", SourceDescriptionType::OpenApi, std::nullopt}};
    ar1.workflows = {wf1};
    verify_value_semantics(ar1, Arazzo{});
  }

  // Test strong typing enum helpers
  for (auto pt : {cdd_cpp::openapi::SchemaPrimitiveType::String,
                  cdd_cpp::openapi::SchemaPrimitiveType::Number,
                  cdd_cpp::openapi::SchemaPrimitiveType::Integer,
                  cdd_cpp::openapi::SchemaPrimitiveType::Boolean,
                  cdd_cpp::openapi::SchemaPrimitiveType::Array,
                  cdd_cpp::openapi::SchemaPrimitiveType::Object,
                  cdd_cpp::openapi::SchemaPrimitiveType::Null}) {
    assert(cdd_cpp::openapi::schema_primitive_type_from_string(
               cdd_cpp::openapi::to_string(pt)) == pt);
  }
  assert(!cdd_cpp::openapi::schema_primitive_type_from_string(" bad ")
              .has_value());

  for (auto loc : {cdd_cpp::openapi::ParameterLocation::Query,
                   cdd_cpp::openapi::ParameterLocation::Header,
                   cdd_cpp::openapi::ParameterLocation::Path,
                   cdd_cpp::openapi::ParameterLocation::Cookie}) {
    assert(cdd_cpp::openapi::parameter_location_from_string(
               cdd_cpp::openapi::to_string(loc)) == loc);
  }
  assert(!cdd_cpp::openapi::parameter_location_from_string("bad").has_value());

  for (auto st : {cdd_cpp::openapi::ParameterStyle::Matrix,
                  cdd_cpp::openapi::ParameterStyle::Label,
                  cdd_cpp::openapi::ParameterStyle::Form,
                  cdd_cpp::openapi::ParameterStyle::Simple,
                  cdd_cpp::openapi::ParameterStyle::SpaceDelimited,
                  cdd_cpp::openapi::ParameterStyle::PipeDelimited,
                  cdd_cpp::openapi::ParameterStyle::DeepObject}) {
    assert(cdd_cpp::openapi::parameter_style_from_string(
               cdd_cpp::openapi::to_string(st)) == st);
  }
  assert(!cdd_cpp::openapi::parameter_style_from_string("bad").has_value());

  for (auto sst : {cdd_cpp::openapi::SecuritySchemeType::ApiKey,
                   cdd_cpp::openapi::SecuritySchemeType::Http,
                   cdd_cpp::openapi::SecuritySchemeType::MutualTLS,
                   cdd_cpp::openapi::SecuritySchemeType::OAuth2,
                   cdd_cpp::openapi::SecuritySchemeType::OpenIdConnect}) {
    assert(cdd_cpp::openapi::security_scheme_type_from_string(
               cdd_cpp::openapi::to_string(sst)) == sst);
  }
  assert(
      !cdd_cpp::openapi::security_scheme_type_from_string("bad").has_value());

  for (auto akl : {cdd_cpp::openapi::ApiKeyLocation::Query,
                   cdd_cpp::openapi::ApiKeyLocation::Header,
                   cdd_cpp::openapi::ApiKeyLocation::Cookie}) {
    assert(cdd_cpp::openapi::api_key_location_from_string(
               cdd_cpp::openapi::to_string(akl)) == akl);
  }
  assert(!cdd_cpp::openapi::api_key_location_from_string("bad").has_value());

  for (auto hm :
       {cdd_cpp::openapi::HttpMethod::Get, cdd_cpp::openapi::HttpMethod::Put,
        cdd_cpp::openapi::HttpMethod::Post,
        cdd_cpp::openapi::HttpMethod::Delete,
        cdd_cpp::openapi::HttpMethod::Options,
        cdd_cpp::openapi::HttpMethod::Head, cdd_cpp::openapi::HttpMethod::Patch,
        cdd_cpp::openapi::HttpMethod::Trace,
        cdd_cpp::openapi::HttpMethod::Query}) {
    assert(cdd_cpp::openapi::http_method_from_string(
               cdd_cpp::openapi::to_uppercase_string(hm)) == hm);
    assert(cdd_cpp::openapi::http_method_from_string(
               cdd_cpp::openapi::to_lowercase_string(hm)) == hm);
  }
  assert(!cdd_cpp::openapi::http_method_from_string("INVALID").has_value());

  cdd_cpp::openapi::Schema s_typed;
  assert(!s_typed.typed_type().has_value());
  s_typed.set_typed_type(cdd_cpp::openapi::SchemaPrimitiveType::Integer);
  auto tt1 = s_typed.typed_type();
  assert(tt1.has_value());
  auto *p_prim = std::get_if<cdd_cpp::openapi::SchemaPrimitiveType>(&*tt1);
  assert(p_prim != nullptr &&
         *p_prim == cdd_cpp::openapi::SchemaPrimitiveType::Integer);
  s_typed.set_typed_types({cdd_cpp::openapi::SchemaPrimitiveType::String,
                           cdd_cpp::openapi::SchemaPrimitiveType::Null});
  auto tt2 = s_typed.typed_type();
  assert(tt2.has_value());
  auto *p_vec =
      std::get_if<std::vector<cdd_cpp::openapi::SchemaPrimitiveType>>(&*tt2);
  assert(p_vec != nullptr && p_vec->size() == 2);

  cdd_cpp::openapi::Parameter p_typed;
  p_typed.set_typed_in(cdd_cpp::openapi::ParameterLocation::Path);
  assert(p_typed.typed_in() == cdd_cpp::openapi::ParameterLocation::Path);
  assert(!p_typed.typed_style().has_value());
  p_typed.set_typed_style(cdd_cpp::openapi::ParameterStyle::Simple);
  assert(p_typed.typed_style() == cdd_cpp::openapi::ParameterStyle::Simple);

  cdd_cpp::openapi::Header h_typed;
  assert(!h_typed.typed_style().has_value());
  h_typed.set_typed_style(cdd_cpp::openapi::ParameterStyle::Simple);
  assert(h_typed.typed_style() == cdd_cpp::openapi::ParameterStyle::Simple);

  cdd_cpp::openapi::Encoding enc_typed;
  assert(!enc_typed.typed_style().has_value());
  enc_typed.set_typed_style(cdd_cpp::openapi::ParameterStyle::Form);
  assert(enc_typed.typed_style() == cdd_cpp::openapi::ParameterStyle::Form);

  cdd_cpp::openapi::SecurityScheme ss_typed;
  ss_typed.set_typed_type(cdd_cpp::openapi::SecuritySchemeType::ApiKey);
  assert(ss_typed.typed_type() == cdd_cpp::openapi::SecuritySchemeType::ApiKey);
  assert(!ss_typed.typed_in().has_value());
  ss_typed.set_typed_in(cdd_cpp::openapi::ApiKeyLocation::Header);
  assert(ss_typed.typed_in() == cdd_cpp::openapi::ApiKeyLocation::Header);

  cdd_cpp::openapi::JsonValue jv_null(nullptr);
  cdd_cpp::openapi::JsonValue jv_bool(true);
  cdd_cpp::openapi::JsonValue jv_int(42);
  cdd_cpp::openapi::JsonValue jv_i64(int64_t{42});
  cdd_cpp::openapi::JsonValue jv_dbl(3.14);
  cdd_cpp::openapi::JsonValue jv_str(std::string("hi"));
  cdd_cpp::openapi::JsonValue jv_cstr("hi");
  cdd_cpp::openapi::JsonValue jv_arr(
      std::vector<cdd_cpp::openapi::JsonValue>{jv_int});
  cdd_cpp::openapi::JsonValue jv_obj(
      std::map<std::string, cdd_cpp::openapi::JsonValue>{{"k", jv_str}});
  assert(jv_int == jv_i64);
  assert(jv_str == jv_cstr);
  assert(!(jv_bool == jv_null));
  verify_value_semantics(jv_obj, jv_arr);

  for (auto r :
       {cdd_cpp::mcp::RoleEnum::User, cdd_cpp::mcp::RoleEnum::Assistant}) {
    assert(cdd_cpp::mcp::role_from_string(cdd_cpp::mcp::to_string(r)) == r);
  }
  assert(!cdd_cpp::mcp::role_from_string("invalid").has_value());

  for (auto lvl : {cdd_cpp::mcp::LoggingLevelEnum::Debug,
                   cdd_cpp::mcp::LoggingLevelEnum::Info,
                   cdd_cpp::mcp::LoggingLevelEnum::Notice,
                   cdd_cpp::mcp::LoggingLevelEnum::Warning,
                   cdd_cpp::mcp::LoggingLevelEnum::Error,
                   cdd_cpp::mcp::LoggingLevelEnum::Critical,
                   cdd_cpp::mcp::LoggingLevelEnum::Alert,
                   cdd_cpp::mcp::LoggingLevelEnum::Emergency}) {
    assert(cdd_cpp::mcp::logging_level_from_string(
               cdd_cpp::mcp::to_string(lvl)) == lvl);
  }
  assert(!cdd_cpp::mcp::logging_level_from_string("invalid").has_value());

  for (auto ic : {cdd_cpp::mcp::IncludeContextEnum::None,
                  cdd_cpp::mcp::IncludeContextEnum::ThisServer,
                  cdd_cpp::mcp::IncludeContextEnum::AllServers}) {
    assert(cdd_cpp::mcp::include_context_from_string(
               cdd_cpp::mcp::to_string(ic)) == ic);
  }
  assert(!cdd_cpp::mcp::include_context_from_string("invalid").has_value());

  cdd_cpp::mcp::JSONRPCRequest jreq_typed;
  jreq_typed.set_typed_id(int64_t{42});
  assert(std::get<int64_t>(jreq_typed.typed_id()) == 42);
  jreq_typed.set_typed_id(std::string("req-abc"));
  assert(std::get<std::string>(jreq_typed.typed_id()) == "req-abc");
  jreq_typed.id = "unquoted_str";
  assert(std::get<std::string>(jreq_typed.typed_id()) == "unquoted_str");
  jreq_typed.id = "";
  assert(std::get<std::string>(jreq_typed.typed_id()).empty());

  cdd_cpp::mcp::JSONRPCResponse jresp_typed;
  jresp_typed.set_typed_id(int64_t{7});
  assert(std::get<int64_t>(jresp_typed.typed_id()) == 7);

  cdd_cpp::mcp::JSONRPCError jerr_typed;
  jerr_typed.set_typed_id(std::string("err-1"));
  assert(std::get<std::string>(jerr_typed.typed_id()) == "err-1");

  cdd_cpp::mcp::CancelledNotificationParams cancel_typed;
  cancel_typed.set_typed_request_id(int64_t{99});
  assert(std::get<int64_t>(cancel_typed.typed_request_id()) == 99);

  cdd_cpp::mcp::ProgressNotificationParams prog_typed;
  prog_typed.set_typed_progress_token(std::string("tok-1"));
  assert(std::get<std::string>(prog_typed.typed_progress_token()) == "tok-1");
}

} // namespace cdd_cpp::struct_tests
