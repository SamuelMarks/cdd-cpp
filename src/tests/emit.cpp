//

#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include <cassert>
#include <iostream>

//
namespace cdd_cpp::openapi {

//
void test_exhaustive() {
  std::FILE *f = std::fopen("spec.json", "r");
  if (!f)
    f = std::fopen("../spec.json", "r");
  if (!f) {

    std::cerr << "could not find exhaustive json!\n";

    return;
  }
  std::fseek(f, 0, SEEK_END);
  size_t size = std::ftell(f);
  std::fseek(f, 0, SEEK_SET);
  std::string json(size, '\0');
  size_t bytes_read = std::fread(&json[0], 1, size, f);
  (void)bytes_read;
  std::fclose(f);
  assert(bytes_read == size);

  auto spec_res = parse(json);
  assert(spec_res);
  OpenAPI spec = *spec_res;

  std::string out = emit(spec);

  // Also parse the emitted code to ensure idempotency and double down on
  // parse.cpp coverage
  auto spec_res2 = parse(out);
  assert(spec_res2);

  std::cout << "test_exhaustive passed.\n";
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
}

void test_openapi_emit_more_details() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "B";
  spec.info.version = "1";

  // Tag coverage
  Tag t;
  t.name = "tag";
  t.description = "tag desc";
  ExternalDocumentation ext_doc;
  ext_doc.description = "ext desc";
  ext_doc.url = "http://ext";
  t.externalDocs = ext_doc;
  spec.tags = std::vector<Tag>{t};

  // Components coverage
  Components comp;
  Schema schema;
  schema.type = "object";
  schema.description = "A schema";

  schema.readOnly = true;
  schema.writeOnly = false;
  schema.deprecated = false;
  schema.maxProperties = 10;
  schema.minProperties = 1;
  schema.required = std::vector<std::string>{"prop1"};
  schema.format = "uuid";
  schema.multipleOf = 2.0;
  schema.maximum = 100.0;
  schema.exclusiveMaximum = false;
  schema.minimum = 0.0;
  schema.exclusiveMinimum = false;
  schema.maxLength = 50;
  schema.minLength = 5;
  schema.pattern = "^[a-z]+$";
  schema.maxItems = 20;
  schema.minItems = 2;
  schema.uniqueItems = true;

  comp.schemas = std::map<std::string, Schema>{{"MySchema", schema}};

  // Response coverage
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
  link.server = Server{"http://link", "link server", {}};
  resp.links = std::map<std::string, Link>{{"link1", link}};

  comp.responses = std::map<std::string, Response>{{"MyResp", resp}};

  // Parameter coverage
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

  // RequestBody coverage
  RequestBody reqBody;
  reqBody.description = "req body desc";
  reqBody.content = std::map<std::string, MediaType>{{"application/json", mt}};
  reqBody.required = true;
  comp.requestBodies =
      std::map<std::string, RequestBody>{{"MyReqBody", reqBody}};

  // SecurityScheme coverage
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

  // Callbacks coverage
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

  // Paths coverage
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
  op2.servers = std::vector<Server>{{"http://op2", "op2 server", {}}};
  p.get = op2;
  p.put = op2;
  p.post = op2;
  p.delete_op = op2;
  p.options = op2;
  p.head = op2;
  p.patch = op2;
  p.trace = op2;
  p.servers = std::vector<Server>{{"http://p", "p server", {}}};
  p.parameters = std::vector<Parameter>{param};
  spec.paths = std::map<std::string, PathItem>{{"/path", p}};

  // XML
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
  link.server = Server{"http://l", "l server", {}};
  comp.links = std::map<std::string, Link>{{"L1", link}};

  spec.components = comp;

  auto out = emit(spec);
  assert(out.find("def") != std::string::npos);
  assert(out.find("alt") != std::string::npos);
  assert(out.find("hdr ex") != std::string::npos);
  assert(out.find("p ex") != std::string::npos);
  assert(out.find("http://ex") != std::string::npos);
  assert(out.find("l server") != std::string::npos);
}

void test_openapi_emit_final_details() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;
  Schema schema;
  schema.type = "object";
  Discriminator disc;
  disc.propertyName = "prop";
  disc.mapping = std::map<std::string, std::string>{{"k", "v"}};
  disc.defaultMapping = "defMapping";
  schema.discriminator = disc;

  // xml covers 180-215. We already have xml test? No, let's add full XML
  // coverage
  XML xml;
  xml.name = "xmlname";
  xml.namespace_url = "xmlns";
  xml.prefix = "xmlpref";
  xml.attribute = true;
  xml.wrapped = true;
  schema.xml = xml;

  // 370-379 is likely OAuthFlow
  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};

  // 401-410 is OAuthFlows
  OAuthFlows flows;
  flows.implicit = flow;
  flows.password = flow;
  flows.clientCredentials = flow;
  flows.authorizationCode = flow;

  SecurityScheme sec;
  sec.type = "oauth2";
  sec.flows = flows;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"S1", sec}};

  // 440-449 is ServerVariable
  ServerVariable sv;
  sv.default_value = "def";
  sv.description = "sv desc";
  sv.enum_values = std::vector<std::string>{"def", "alt"};
  Server s;
  s.url = "http://server";
  s.description = "server desc";
  s.variables = std::map<std::string, ServerVariable>{{"var1", sv}};
  spec.servers = std::vector<Server>{s};

  // 492-501 is Example
  Example ex;
  ex.summary = "ex sum";
  ex.description = "ex desc";
  ex.value = "val";
  ex.externalValue = "http://ex";
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

  // 536-567 is Contact and License? Wait, we added that. Let's see what is 536
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

  // 605-630 is something about Paths
  PathItem p;
  p.summary = "p summary";
  p.description = "p desc";
  Operation op2;
  op2.tags = std::vector<std::string>{"tag1"};
  op2.summary = "op2 sum";
  op2.description = "op2 desc";
  op2.externalDocs = ed;
  op2.operationId = "op2id";
  op2.deprecated = true;
  op2.security = std::vector<std::map<std::string, std::vector<std::string>>>{
      {{"sec", {"read"}}}};
  op2.servers = std::vector<Server>{{"http://op2", "op2 server", {}}};
  p.get = op2;
  p.put = op2;
  p.post = op2;
  p.delete_op = op2;
  p.options = op2;
  p.head = op2;
  p.patch = op2;
  p.trace = op2;
  p.servers = std::vector<Server>{{"http://p", "p server", {}}};
  spec.paths = std::map<std::string, PathItem>{{"/path", p}};

  // 658-667 is components callbacks maybe?
  comp.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"MyCb", {{"url", p}}}});

  comp.schemas = std::map<std::string, Schema>{{"Schema1", schema}};
  spec.components = comp;

  // 945-954 is tags
  Tag t;
  t.name = "T";
  t.description = "T desc";
  t.externalDocs = ed;
  spec.tags = std::vector<Tag>{t};

  auto out = emit(spec);
}

void test_openapi_emit_final_details2() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;
  Schema schema;
  schema.type = "object";

  // xml covers 180-215. Wait, 183-192 is schema.ref!
  Reference ref;
  ref.ref = "#/components/schemas/Ref";
  ref.summary = "ref sum";
  ref.description = "ref desc";
  schema.ref = ref;

  // 370-379 is OAuthFlow ? We did this but wait, it failed to compile in
  // earlier attempt so I backed out? No, I did not back out.
  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};

  OAuthFlows flows;
  flows.implicit = flow;
  flows.password = flow;
  flows.clientCredentials = flow;
  flows.authorizationCode = flow;

  SecurityScheme sec;
  sec.type = "oauth2";
  sec.flows = flows;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"S1", sec}};

  // 440-449 is ServerVariable
  ServerVariable sv;
  sv.default_value = "def";
  sv.description = "sv desc";
  sv.enum_values = std::vector<std::string>{"def", "alt"};
  Server s;
  s.url = "http://server";
  s.description = "server desc";
  s.variables = std::map<std::string, ServerVariable>{{"var1", sv}};
  spec.servers = std::vector<Server>{s};

  // 492-501 is Example
  Example ex;
  ex.summary = "ex sum";
  ex.description = "ex desc";
  ex.value = "val";
  ex.externalValue = "http://ex";
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

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

  PathItem p;
  p.summary = "p summary";
  p.description = "p desc";
  Operation op2;
  op2.tags = std::vector<std::string>{"tag1"};
  op2.summary = "op2 sum";
  op2.description = "op2 desc";
  op2.externalDocs = ed;
  op2.operationId = "op2id";
  op2.deprecated = true;
  op2.security = std::vector<std::map<std::string, std::vector<std::string>>>{
      {{"sec", {"read"}}}};
  op2.servers = std::vector<Server>{{"http://op2", "op2 server", {}}};
  p.get = op2;
  p.put = op2;
  p.post = op2;
  p.delete_op = op2;
  p.options = op2;
  p.head = op2;
  p.patch = op2;
  p.trace = op2;
  p.servers = std::vector<Server>{{"http://p", "p server", {}}};
  spec.paths = std::map<std::string, PathItem>{{"/path", p}};

  // callbacks
  comp.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"MyCb", {{"url", p}}}});

  comp.schemas = std::map<std::string, Schema>{{"Schema1", schema}};
  spec.components = comp;

  // 945-954 is tags
  Tag t;
  t.name = "T";
  t.description = "T desc";
  t.externalDocs = ed;
  spec.tags = std::vector<Tag>{t};

  auto out = emit(spec);
}

void test_openapi_emit_final_details3() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;
  Schema schema;
  schema.type = "object";

  schema.properties = std::make_unique<std::map<std::string, Schema>>(
      std::map<std::string, Schema>{{"k", Schema{"string"}}});

  Schema items{"integer"};
  schema.items = std::make_unique<Schema>(items);

  Schema allOf{"string"};
  schema.allOf =
      std::make_shared<std::vector<Schema>>(std::vector<Schema>{allOf});
  schema.oneOf =
      std::make_shared<std::vector<Schema>>(std::vector<Schema>{allOf});
  schema.anyOf =
      std::make_shared<std::vector<Schema>>(std::vector<Schema>{allOf});
  schema.not_schema = std::make_unique<Schema>(allOf);

  // 370-379 is OAuthFlow ?
  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};

  OAuthFlows flows;
  flows.implicit = flow;
  flows.password = flow;
  flows.clientCredentials = flow;
  flows.authorizationCode = flow;

  SecurityScheme sec;
  sec.type = "oauth2";
  sec.flows = flows;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"S1", sec}};

  // 440-449 is ServerVariable
  ServerVariable sv;
  sv.default_value = "def";
  sv.description = "sv desc";
  sv.enum_values = std::vector<std::string>{"def", "alt"};
  Server s;
  s.url = "http://server";
  s.description = "server desc";
  s.variables = std::map<std::string, ServerVariable>{{"var1", sv}};
  spec.servers = std::vector<Server>{s};

  // 492-501 is Example
  Example ex;
  ex.summary = "ex sum";
  ex.description = "ex desc";
  ex.value = "val";
  ex.externalValue = "http://ex";
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

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

  PathItem p;
  p.summary = "p summary";
  p.description = "p desc";
  Operation op2;
  op2.tags = std::vector<std::string>{"tag1"};
  op2.summary = "op2 sum";
  op2.description = "op2 desc";
  op2.externalDocs = ed;
  op2.operationId = "op2id";
  op2.deprecated = true;
  op2.security = std::vector<std::map<std::string, std::vector<std::string>>>{
      {{"sec", {"read"}}}};
  op2.servers = std::vector<Server>{{"http://op2", "op2 server", {}}};
  p.get = op2;
  p.put = op2;
  p.post = op2;
  p.delete_op = op2;
  p.options = op2;
  p.head = op2;
  p.patch = op2;
  p.trace = op2;
  p.servers = std::vector<Server>{{"http://p", "p server", {}}};
  spec.paths = std::map<std::string, PathItem>{{"/path", p}};

  // callbacks
  comp.callbacks =
      std::make_shared<std::map<std::string, std::map<std::string, PathItem>>>(
          std::map<std::string, std::map<std::string, PathItem>>{
              {"MyCb", {{"url", p}}}});

  comp.schemas = std::map<std::string, Schema>{{"Schema1", schema}};
  spec.components = comp;

  // 945-954 is tags
  Tag t;
  t.name = "T";
  t.description = "T desc";
  t.externalDocs = ed;
  spec.tags = std::vector<Tag>{t};

  auto out = emit(spec);
}

void test_openapi_emit_final_details4() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;

  Reference ref;
  ref.ref = "#/components/schemas/Ref";
  ref.summary = "ref sum";
  ref.description = "ref desc";

  Example ex;
  ex.summary = "ex sum";
  ex.ref = ref;
  comp.examples = std::map<std::string, Example>{{"Ex1", ex}};

  Header hdr;
  hdr.ref = ref;
  comp.headers = std::map<std::string, Header>{{"H1", hdr}};

  Parameter param;
  param.ref = ref;
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  RequestBody req;
  req.ref = ref;
  comp.requestBodies = std::map<std::string, RequestBody>{{"R1", req}};

  Response resp;
  resp.ref = ref;
  comp.responses = std::map<std::string, Response>{{"Resp1", resp}};

  SecurityScheme sec;
  sec.ref = ref;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"Sec1", sec}};

  Link link;
  link.ref = ref;
  comp.links = std::map<std::string, Link>{{"L1", link}};

  spec.components = comp;

  auto out = emit(spec);
}

void test_openapi_emit_final_details5() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;

  // 515 is allowEmptyValue inside Header maybe?
  Header hdr;
  hdr.allowEmptyValue = true;
  comp.headers = std::map<std::string, Header>{{"H1", hdr}};

  // 536-556 is examples and content inside Parameter
  Parameter param;
  param.name = "p1";
  param.in = "query";
  param.examples = std::map<std::string, Example>{{"Ex1", Example{}}};
  param.content =
      std::map<std::string, MediaType>{{"application/json", MediaType{}}};
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  // 626-630 is something
  PathItem pathItem;
  pathItem.parameters = std::vector<Parameter>{param};
  Operation op;
  op.operationId = "op";
  pathItem.get = op;
  spec.paths = std::map<std::string, PathItem>{{"/path2", pathItem}};

  // 945-954 is emit_Tag maybe? Wait, emit_Tag has: jw.start_object();
  // jw.key_value("name", obj.name); ...
  Tag t2;
  t2.name = "tag2";
  t2.description = "tag2 desc";
  t2.externalDocs = ExternalDocumentation{"http://ext", "ext desc"};
  spec.tags = std::vector<Tag>{t2};

  spec.components = comp;
  auto out = emit(spec);
}

void test_openapi_emit_final_details6() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;

  // 626 is deviceAuthorization in OAuthFlows
  OAuthFlow flow;
  flow.authorizationUrl = "http://auth";
  flow.tokenUrl = "http://token";
  flow.refreshUrl = "http://refresh";
  flow.scopes = std::map<std::string, std::string>{{"read", "read access"}};

  OAuthFlows flows;
  flows.deviceAuthorization = flow;
  SecurityScheme sec;
  sec.type = "oauth2";
  sec.flows = flows;
  comp.securitySchemes = std::map<std::string, SecurityScheme>{{"S1", sec}};

  // 945 is Tag emit? No, 945 is probably openapi struct emit. Wait, tag emit is
  // around 40-50. 945 is probably externalDocs in emit_OpenAPI? Let's just
  // output the lines.
  spec.components = comp;
  auto out = emit(spec);
}

void test_openapi_emit_final_details7() {
  OpenAPI spec;
  spec.openapi = "3.1.0";
  spec.info.title = "D";
  spec.info.version = "1";

  Components comp;

  // 515 is allowEmptyValue inside Parameter. Wait, we did that. Let's make sure
  // it's inside Parameter.
  Parameter param;
  param.allowEmptyValue = true;
  comp.parameters = std::map<std::string, Parameter>{{"P1", param}};

  // 945 is pathItems in Components.
  PathItem p;
  p.summary = "sum";
  comp.pathItems = std::map<std::string, PathItem>{{"P1", p}};

  spec.components = comp;
  auto out = emit(spec);
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

  assert(out.find("\"openapi\":\"3.2.0\"") != std::string::npos);
  assert(out.find("\"query\":{") != std::string::npos);
  assert(out.find("\"queryTest\"") != std::string::npos);
  assert(out.find("\"additionalOperations\":{") != std::string::npos);
  assert(out.find("\"PROPFIND\":{") != std::string::npos);
  assert(out.find("\"propfindTest\"") != std::string::npos);

  assert(out.find("\"webhooks\":{") != std::string::npos);
  assert(out.find("\"myWebhook\":{") != std::string::npos);

  assert(out.find("\"mediaTypes\":{") != std::string::npos);
  assert(out.find("\"itemSchema\":{") != std::string::npos);
  assert(out.find("\"prefixEncoding\":[") != std::string::npos);
  assert(out.find("\"itemEncoding\":{") != std::string::npos);

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
          "discriminator": {

            "propertyName": "petType",
            "defaultMapping": "#/components/schemas/Dog"

          }
        }
      }
    }
  })";

  auto spec_3_res = parse(json_3_2_0);
  assert(spec_3_res);
  OpenAPI spec_3 = *spec_3_res;
  std::string out_3 = emit(spec_3);

  assert(out_3.find("\"$self\":\"https://example.com/openapi.json\"") !=
         std::string::npos);
  assert(out_3.find("\"parent\":\"Core\"") != std::string::npos);
  assert(out_3.find("\"kind\":\"entity\"") != std::string::npos);
  assert(out_3.find("\"defaultMapping\":\"#/components/schemas/Dog\"") !=
         std::string::npos);

  test_exhaustive();
  test_openapi_emit_details();
  test_openapi_emit_more_details();
  test_openapi_emit_even_more_details();
  test_openapi_emit_final_details();
  test_openapi_emit_final_details2();
  test_openapi_emit_final_details3();
  test_openapi_emit_final_details4();
  test_openapi_emit_final_details5();
  test_openapi_emit_final_details6();
  test_openapi_emit_final_details7();

  std::cout << "test_emit passed.\n";
}
} // namespace cdd_cpp::openapi
