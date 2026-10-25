/// @file emit.cpp
/// @brief Unit tests for OpenAPI 3.2.0 JSON emitter and round-trip parser
/// symmetry.

#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>

namespace cdd_cpp::openapi {

static void assert_round_trip(const OpenAPI &spec) {
  std::string out = emit(spec);
  assert(!out.empty());
  auto parsed = parse(out);
  assert(parsed.has_value());
  assert(*parsed == spec);
  assert(emit(*parsed) == out);
}

void test_exhaustive() {
  std::FILE *f = std::fopen("spec.json", "r");
  if (!f)
    f = std::fopen("../spec.json", "r");
  assert(f != nullptr);
  std::fseek(f, 0, SEEK_END);
  size_t size = static_cast<size_t>(std::ftell(f));
  std::fseek(f, 0, SEEK_SET);
  std::string json(size, '\0');
  [[maybe_unused]] size_t bytes_read = std::fread(&json[0], 1, size, f);
  std::fclose(f);
  assert(bytes_read == size);

  auto spec_res = parse(json);
  assert(spec_res.has_value());
  OpenAPI spec = *spec_res;

  assert_round_trip(spec);
  std::cout << "test_exhaustive passed." << std::endl;
}

void test_openapi_emit_details() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "A";
  spec.info.version = "1";

  Contact c;
  c.name = "John Doe";
  c.url = "http://example.com";
  c.email = "john@example.com";
  spec.info.contact = c;

  License l;
  l.name = "MIT";
  l.identifier = "MIT";
  l.url = "http://mit.edu";
  spec.info.license = l;

  ExternalDocumentation ed;
  ed.description = "Ext Docs";
  ed.url = "http://extdocs.com";
  spec.externalDocs = ed;

  Tag t;
  t.name = "T";
  t.description = "T desc";
  t.externalDocs = ed;
  spec.tags = std::vector<Tag>{t};

  auto out = emit(spec);
  assert(out.find("John Doe") != std::string::npos);
  assert(out.find("MIT") != std::string::npos);
  assert(out.find("Ext Docs") != std::string::npos);
  assert_round_trip(spec);
}

void test_openapi_emit_more_details() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "B";
  spec.info.version = "1";

  Tag t;
  t.name = "tag";
  t.description = "tag desc";
  ExternalDocumentation ext_doc;
  ext_doc.description = "ext desc";
  ext_doc.url = "http://ext";
  t.externalDocs = ext_doc;
  spec.tags = std::vector<Tag>{t};

  Components comp;
  Schema schema;
  schema.type = "object";
  schema.description = "A schema";
  schema.format = "uuid";

  comp.schemas = std::map<std::string, Schema>{{"MySchema", schema}};

  Response resp;
  resp.description = "resp desc";
  MediaType mt;
  mt.schema = schema;
  mt.example = "an example";
  Example ex;
  ex.summary = "ex summary";
  ex.description = "ex desc";
  ex.value = "val";
  ex.externalValue = "http://val";
  mt.examples = std::map<std::string, Example>{{"ex1", ex}};
  resp.content = std::map<std::string, MediaType>{{"application/json", mt}};
  Header hdr;
  hdr.description = "hdr desc";
  hdr.required = true;
  hdr.deprecated = false;
  hdr.allowEmptyValue = true;
  hdr.schema = schema;
  resp.headers = std::map<std::string, Header>{{"X-Header", hdr}};

  Link link;
  link.operationRef = "ref";
  link.operationId = "id";
  link.parameters = std::map<std::string, std::string>{{"p1", "v1"}};
  link.requestBody = "body";
  link.description = "link desc";
  link.server = Server{"http://link", "link server", std::nullopt, {}};
  resp.links = std::map<std::string, Link>{{"link1", link}};

  comp.responses = std::map<std::string, Response>{{"MyResp", resp}};

  Parameter param;
  param.name = "p1";
  param.in = "query";
  param.description = "param desc";
  param.required = true;
  param.deprecated = true;
  param.allowEmptyValue = true;
  param.style = "form";
  param.explode = true;
  param.allowReserved = true;
  param.schema = schema;
  param.example = "example";
  comp.parameters = std::map<std::string, Parameter>{{"MyParam", param}};

  RequestBody reqBody;
  reqBody.description = "req body desc";
  reqBody.content = std::map<std::string, MediaType>{{"application/json", mt}};
  reqBody.required = true;
  comp.requestBodies =
      std::map<std::string, RequestBody>{{"MyReqBody", reqBody}};

  SecurityScheme secScheme;
  secScheme.type = "apiKey";
  secScheme.description = "sec desc";
  secScheme.name = "api_key";
  secScheme.in = "header";
  secScheme.scheme = "bearer";
  secScheme.bearerFormat = "JWT";
  secScheme.openIdConnectUrl = "http://oidc";
  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};
  OAuthFlows flows;
  flows.implicit = flow;
  secScheme.flows = flows;
  comp.securitySchemes =
      std::map<std::string, SecurityScheme>{{"MySec", secScheme}};

  PathItem pathItem;
  pathItem.summary = "cb summary";
  pathItem.description = "cb desc";
  Operation op;
  op.operationId = "cbOp";
  op.summary = "op summary";
  pathItem.get = op;
  comp.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"MyCb", {{"url", pathItem}}}});

  spec.components = comp;

  PathItem p;
  p.summary = "p summary";
  p.description = "p desc";
  Operation op2;
  op2.tags = std::vector<std::string>{"tag1"};
  op2.summary = "op2 sum";
  op2.description = "op2 desc";
  op2.externalDocs = ext_doc;
  op2.operationId = "op2id";
  op2.parameters = std::vector<Parameter>{param};
  op2.requestBody = reqBody;
  op2.responses = std::map<std::string, Response>{{"200", resp}};
  op2.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"Cb", {{"u", p}}}});
  op2.deprecated = true;
  op2.security = std::vector<std::map<std::string, std::vector<std::string>>>{
      {{"sec", {"read"}}}};
  op2.servers =
      std::vector<Server>{{"http://op2", "op2 server", std::nullopt, {}}};
  p.get = op2;
  p.put = op2;
  p.post = op2;
  p.delete_op = op2;
  p.options = op2;
  p.head = op2;
  p.patch = op2;
  p.trace = op2;
  p.servers = std::vector<Server>{{"http://p", "p server", std::nullopt, {}}};
  p.parameters = std::vector<Parameter>{param};
  spec.paths = std::map<std::string, PathItem>{{"/path", p}};

  XML xml;
  xml.name = "name";
  xml.namespace_url = "ns";
  xml.prefix = "pref";
  xml.attribute = true;
  xml.wrapped = true;
  Schema schema2;
  schema2.xml = xml;
  spec.components->schemas->insert({"MySchema2", schema2});

  spec.security = std::vector<std::map<std::string, std::vector<std::string>>>{
      {{"sec", {"read"}}}};

  auto out = emit(spec);
  assert(out.find("tag desc") != std::string::npos);
  assert(out.find("uuid") != std::string::npos);
  assert(out.find("ex summary") != std::string::npos);
  assert(out.find("bearer") != std::string::npos);
  assert(out.find("cb summary") != std::string::npos);
  assert(out.find("p summary") != std::string::npos);
  assert_round_trip(spec);
}

void test_openapi_emit_even_more_details() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "C";
  spec.info.version = "1";

  ServerVariable sv;
  sv.default_value = "def";
  sv.description = "sv desc";
  sv.enum_values = std::vector<std::string>{"def", "alt"};
  Server s;
  s.url = "http://server";
  s.description = "server desc";
  s.variables = std::map<std::string, ServerVariable>{{"var1", sv}};
  spec.servers = std::vector<Server>{s};

  Components comp;
  Header hdr;
  hdr.description = "hdr";
  hdr.required = true;
  hdr.deprecated = false;
  hdr.allowEmptyValue = false;
  hdr.style = "simple";
  hdr.explode = false;
  hdr.allowReserved = false;
  Schema sch;
  sch.type = "string";
  hdr.schema = sch;
  hdr.example = "hdr ex";
  comp.headers = std::map<std::string, Header>{{"H1", hdr}};

  Parameter param;
  param.name = "p1";
  param.in = "query";
  param.description = "param";
  param.required = true;
  param.deprecated = false;
  param.allowEmptyValue = false;
  param.style = "form";
  param.explode = true;
  param.allowReserved = true;
  param.schema = sch;
  param.example = "p ex";
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  Example ex;
  ex.summary = "ex sum";
  ex.description = "ex desc";
  ex.value = "val";
  ex.externalValue = "http://ex";
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

  Link link;
  link.operationRef = "ref";
  link.operationId = "id";
  link.parameters = std::map<std::string, std::string>{{"p", "v"}};
  link.requestBody = "body";
  link.description = "link desc";
  link.server = Server{"http://l", "l server", std::nullopt, {}};
  comp.links = std::map<std::string, Link>{{"L1", link}};

  spec.components = comp;

  auto out = emit(spec);
  assert(out.find("def") != std::string::npos);
  assert(out.find("alt") != std::string::npos);
  assert(out.find("hdr ex") != std::string::npos);
  assert(out.find("p ex") != std::string::npos);
  assert(out.find("http://ex") != std::string::npos);
  assert(out.find("l server") != std::string::npos);
  assert_round_trip(spec);
}

void test_emit_schema_combinators_and_oauth() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "Combinators and OAuth";
  spec.info.version = "1";

  Components comp;
  Schema schema;
  schema.type = "object";
  Discriminator disc;
  disc.propertyName = "prop";
  disc.mapping = std::map<std::string, std::string>{{"k", "v"}};
  disc.defaultMapping = "defMapping";
  schema.discriminator = disc;

  schema.properties = std::make_shared<std::map<std::string, Schema>>(
      std::map<std::string, Schema>{{"k", Schema{"string"}}});
  schema.items = std::make_shared<Schema>(Schema{"integer"});

  Schema ref_schema;
  Reference ref;
  ref.ref = "#/components/schemas/Ref";
  ref.summary = "ref sum";
  ref.description = "ref desc";
  ref_schema.description = "ref desc";
  ref_schema.ref = ref;

  Schema min_xml_schema;
  min_xml_schema.xml = XML{};

  comp.schemas = std::map<std::string, Schema>{{"Schema1", schema},
                                               {"RefSchema", ref_schema},
                                               {"MinXml", min_xml_schema}};

  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.deviceAuthorizationUrl = "http://device";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};

  OAuthFlows flows;
  flows.implicit = flow;
  flows.password = flow;
  flows.clientCredentials = flow;
  flows.authorizationCode = flow;
  flows.deviceAuthorization = flow;

  SecurityScheme sec;
  sec.type = "oauth2";
  sec.flows = flows;

  SecurityScheme sec_min_flow;
  sec_min_flow.type = "oauth2";
  OAuthFlows min_flows;
  min_flows.deviceAuthorization = OAuthFlow{};
  sec_min_flow.flows = min_flows;

  comp.securitySchemes = std::map<std::string, SecurityScheme>{
      {"S1", sec}, {"MinFlow", sec_min_flow}};

  Server srv_min_var;
  srv_min_var.url = "http://min";
  ServerVariable sv_min;
  sv_min.default_value = "d";
  srv_min_var.variables = std::map<std::string, ServerVariable>{{"v", sv_min}};
  spec.servers = std::vector<Server>{srv_min_var};

  PathItem p;
  p.summary = "sum";
  comp.pathItems = std::map<std::string, PathItem>{{"P1", p}};

  Parameter param;
  param.name = "p1";
  param.in = "query";
  param.allowEmptyValue = true;
  param.examples = std::map<std::string, Example>{{"Ex1", Example{}}};
  param.content =
      std::map<std::string, MediaType>{{"application/json", MediaType{}}};
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  spec.components = comp;

  auto out = emit(spec);
  assert(out.find(R"("properties":{)") != std::string::npos);
  assert(out.find(R"("items":{)") != std::string::npos);
  assert(out.find(R"("discriminator":{)") != std::string::npos);
  assert(out.find(R"("deviceAuthorization":{)") != std::string::npos);
  assert(out.find(R"("pathItems":{)") != std::string::npos);
  assert_round_trip(spec);
}

void test_emit_component_refs() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "Component Refs";
  spec.info.version = "1";

  Components comp;
  Reference ref;
  ref.ref = "#/components/schemas/Ref";
  ref.summary = "ref sum";
  ref.description = "ref desc";

  Example ex;
  ex.summary = "ref sum";
  ex.description = "ref desc";
  ex.ref = ref;
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

  Header hdr;
  hdr.description = "ref desc";
  hdr.ref = ref;
  comp.headers = std::map<std::string, Header>{{"H1", hdr}};

  Parameter param;
  param.description = "ref desc";
  param.ref = ref;
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  RequestBody req;
  req.description = "ref desc";
  req.ref = ref;
  comp.requestBodies = std::map<std::string, RequestBody>{{"R1", req}};

  Response resp;
  resp.description = "ref desc";
  resp.ref = ref;
  comp.responses = std::map<std::string, Response>{{"Resp1", resp}};

  SecurityScheme sec;
  sec.description = "ref desc";
  sec.ref = ref;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"Sec1", sec}};

  Link link;
  link.description = "ref desc";
  link.ref = ref;
  comp.links = std::map<std::string, Link>{{"L1", link}};

  spec.components = comp;

  auto out = emit(spec);
  assert(out.find(R"("$ref":"#/components/schemas/Ref")") != std::string::npos);
  assert(out.find(R"("summary":"ref sum")") != std::string::npos);
  assert(out.find(R"("description":"ref desc")") != std::string::npos);
  assert_round_trip(spec);
}

void test_emit() {
  std::string json = R"({
            "openapi": "3.2.0",
            "info": {
                "title": "Test API",
                "version": "1.0.0"
            },
            "paths": {
                "/test": {
                    "query": {
                        "operationId": "queryTest"
                    },
                    "additionalOperations": {
                        "PROPFIND": {
                            "operationId": "propfindTest"
                        }
                    }
                }
            },
            "webhooks": {
                "myWebhook": {
                    "post": {
                        "operationId": "webhookPost"
                    }
                }
            },
            "components": {
                "mediaTypes": {
                    "application/json": {
                        "schema": { "type": "string" },
                        "examples": {
                            "MyEx": { "summary": "ex" }
                        },
                        "itemSchema": { "type": "string" },
                        "prefixEncoding": [
                            {"contentType": "text/plain", "encoding": { "innerEnc1": { "style": "simple" } } }
                        ],
                        "itemEncoding": {
                            "contentType": "application/json",
                            "encoding": { "innerEnc2": { "style": "simple" } }
                        },
                        "encoding": {
                            "encProp": {
                                "encoding": { "inner": { "style": "simple", "encoding": { "innerinner": { "style": "simple" } } } },
                                "style": "form",
                                "contentType": "application/json",
                                "headers": {
                                    "X-Rate-Limit": {
                                        "description": "calls per hour",
                                        "required": true,
                                        "deprecated": true,
                                        "allowEmptyValue": true,
                                        "explode": true,
                                        "allowReserved": true,
                                        "schema": { "type": "string" },
                                        "examples": { "A": { "value": "B" } },
                                        "content": { "application/json": { "schema": { "type": "string" }, "encoding": { "innerEnc": { "style": "simple" } } } }
                                    }
                                },
                                "explode": true,
                                "allowReserved": false,
                                "itemSchema": { "type": "string" },
                                "prefixEncoding": [
                                    {"contentType": "text/plain", "encoding": { "innerEnc3": { "style": "simple" } } }
                                ],
                                "itemEncoding": {
                                    "contentType": "application/json",
                                    "encoding": { "innerEnc4": { "style": "simple" } }
                                }
                            }
                        }
                    }
                }
            }
        })";

  auto spec_res = parse(json);
  assert(spec_res);
  OpenAPI spec = *spec_res;
  std::string out = emit(spec);

  assert(out.find(R"("openapi":"3.2.0")") != std::string::npos);
  assert(out.find(R"("query":{)") != std::string::npos);
  assert(out.find(R"("queryTest")") != std::string::npos);
  assert(out.find(R"("additionalOperations":{)") != std::string::npos);
  assert(out.find(R"("PROPFIND":{)") != std::string::npos);
  assert(out.find(R"("propfindTest")") != std::string::npos);
  assert(out.find(R"("webhooks":{)") != std::string::npos);
  assert(out.find(R"("myWebhook":{)") != std::string::npos);
  assert(out.find(R"("mediaTypes":{)") != std::string::npos);
  assert(out.find(R"("itemSchema":{)") != std::string::npos);
  assert(out.find(R"("prefixEncoding":[)") != std::string::npos);
  assert(out.find(R"("itemEncoding":{)") != std::string::npos);
  assert_round_trip(spec);

  std::string json_3_2_0 = R"({
    "openapi": "3.2.0",
    "$self": "https://example.com/openapi.json",
    "info": {
      "title": "Test 3.2.0",
      "version": "1.0.0"
    },
    "tags": [
      {
        "name": "User",
        "parent": "Core",
        "kind": "entity"
      }
    ],
    "components": {
      "schemas": {
        "Pet": {
          "type": "object",
          "default": "dog",
          "const": "dog",
          "enum": ["dog", "cat"],
          "examples": ["dog"],
          "additionalProperties": false,
          "xml": {
            "nodeType": "element",
            "name": "pet"
          },
          "discriminator": {
            "propertyName": "petType",
            "defaultMapping": "#/components/schemas/Dog"
          }
        }
      },
      "examples": {
        "PetEx": {
          "summary": "Pet example",
          "dataValue": {"petType": "dog"},
          "serializedValue": "{\"petType\":\"dog\"}"
        }
      }
    }
  })";

  auto spec_3_res = parse(json_3_2_0);
  assert(spec_3_res);
  OpenAPI spec_3 = *spec_3_res;
  std::string out_3 = emit(spec_3);

  assert(out_3.find(R"("$self":"https://example.com/openapi.json")") !=
         std::string::npos);
  assert(out_3.find(R"("parent":"Core")") != std::string::npos);
  assert(out_3.find(R"("kind":"entity")") != std::string::npos);
  assert(out_3.find(R"("defaultMapping":"#/components/schemas/Dog")") !=
         std::string::npos);
  assert(out_3.find(R"("nodeType":"element")") != std::string::npos);
  assert(out_3.find(R"("dataValue":{"petType":"dog"})") != std::string::npos);
  assert(out_3.find(R"("serializedValue":"{\"petType\":\"dog\"}")") !=
         std::string::npos);
  assert_round_trip(spec_3);

  test_exhaustive();
  test_openapi_emit_details();
  test_openapi_emit_more_details();
  test_openapi_emit_even_more_details();
  test_emit_schema_combinators_and_oauth();
  test_emit_component_refs();

  std::cout << "test_emit passed." << std::endl;
}

} // namespace cdd_cpp::openapi
