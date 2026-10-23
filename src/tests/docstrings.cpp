/// @file docstrings.cpp
/// @brief Unit tests for OpenAPI docstring emitting, parsing, and round-trip
/// symmetry.

#include "../docstrings/emit.hpp"
#include "../docstrings/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::docstrings {

void test_parse() {
  std::string raw_doc = R"(
/// @title Sample Service
/// @summary Short service summary
/// @version 2.5.0
/// @description Detailed service description
/// @termsOfService https://example.com/tos
/// @contact_name API Team
/// @contact_url https://example.com/contact
/// @contact_email api@example.com
/// @license_name Apache 2.0
/// @license_identifier Apache-2.0
/// @license_url https://www.apache.org/licenses/LICENSE-2.0
/// @servers
/// - https://api.example.com/{env} (Production Server)
///   @server_variable env prod Environment name [prod,staging,dev]
/// @securitySchemes
/// - bearerAuth http Authorization header bearer JWT
///   @securityScheme_description bearerAuth JWT Bearer Token
///   @securityScheme_openIdConnectUrl bearerAuth https://example.com/.well-known/openid
///   @securityScheme_oauth2MetadataUrl bearerAuth https://example.com/.well-known/oauth2
///   @securityScheme_deprecated bearerAuth
)";

  openapi::OpenAPI spec = parse(raw_doc);
  assert(spec.info.title == "Sample Service");
  assert(spec.info.summary == "Short service summary");
  assert(spec.info.version == "2.5.0");
  assert(spec.info.description == "Detailed service description");
  assert(spec.info.termsOfService == "https://example.com/tos");
  assert(spec.info.contact.has_value());
  assert(spec.info.contact->name == "API Team");
  assert(spec.info.contact->url == "https://example.com/contact");
  assert(spec.info.contact->email == "api@example.com");
  assert(spec.info.license.has_value());
  assert(spec.info.license->name == "Apache 2.0");
  assert(spec.info.license->identifier == "Apache-2.0");
  assert(spec.info.license->url ==
         "https://www.apache.org/licenses/LICENSE-2.0");
  assert(spec.servers.has_value() && spec.servers->size() == 1);
  assert(spec.servers->at(0).url == "https://api.example.com/{env}");
  assert(spec.servers->at(0).description == "Production Server");
  assert(spec.servers->at(0).variables.has_value());
  assert(spec.servers->at(0).variables->at("env").default_value == "prod");
  assert(spec.servers->at(0).variables->at("env").description ==
         "Environment name");
  assert(spec.servers->at(0).variables->at("env").enum_values->size() == 3);
  assert(spec.components.has_value() &&
         spec.components->securitySchemes.has_value());
  const auto &scheme = spec.components->securitySchemes->at("bearerAuth");
  assert(scheme.type == "http");
  assert(scheme.description == "JWT Bearer Token");
  assert(scheme.openIdConnectUrl == "https://example.com/.well-known/openid");
  assert(scheme.oauth2MetadataUrl == "https://example.com/.well-known/oauth2");
  assert(scheme.deprecated.value_or(false) == true);

  // Test operation, path, and schema docstring parsers
  std::string op_doc = R"(
/// @summary Get user by ID
/// @description Returns a single user
/// @tags users,public
/// @deprecated
/// @security bearerAuth [read:users,profile]
/// @security apiKey []
/// @return std::string
/// @return 200 - User found
/// @response_content 200 application/json
)";
  openapi::Operation op_rc_first;
  parse_operation_docstrings("/// @response_content 201 application/json\n",
                             op_rc_first);
  assert(op_rc_first.responses.has_value() &&
         op_rc_first.responses->contains("201"));
  openapi::Operation op;
  parse_operation_docstrings(op_doc, op);
  assert(op.summary == "Get user by ID");
  assert(op.description == "Returns a single user");
  assert(op.tags.has_value() && op.tags->size() == 2);
  assert(op.deprecated == true);
  assert(op.security.has_value() && op.security->size() == 2);
  assert(op.responses.has_value() && op.responses->contains("200"));
  assert(op.responses->at("200").description == "User found");
  assert(op.responses->at("200").content->contains("application/json"));

  openapi::PathItem pi;
  parse_path_docstrings("/// @summary Path sum\n/// @description Path desc\n",
                        pi);
  assert(pi.summary == "Path sum");
  assert(pi.description == "Path desc");

  openapi::Schema schema;
  std::string schema_doc = R"(
/// @description User object
/// @example {"id": 1}
/// @format int64
/// @xml_nodeType element
/// @xml_name UserNode
/// @xml_namespace https://example.com/ns
/// @xml_prefix usr
/// @xml_attribute true
/// @xml_wrapped true
/// @externalDocs_url https://example.com/docs
/// @externalDocs_description More info
/// @discriminator_propertyName petType
/// @discriminator_mapping dog #/components/schemas/Dog
)";
  parse_schema_docstrings(schema_doc, schema);
  parse_schema_docstrings(schema_doc, schema);
  assert(schema.description == "User object");
  assert(schema.example == "{\"id\": 1}");
  assert(schema.format == "int64");
  assert(schema.xml.has_value());
  assert(schema.xml->name == "UserNode");
  assert(schema.xml->namespace_url == "https://example.com/ns");
  assert(schema.xml->prefix == "usr");
  assert(schema.xml->attribute == true);
  assert(schema.xml->wrapped == true);
  assert(schema.externalDocs.has_value());
  assert(schema.externalDocs->url == "https://example.com/docs");
  assert(schema.externalDocs->description == "More info");
  assert(schema.discriminator.has_value());
  assert(schema.discriminator->propertyName == "petType");
  assert(schema.discriminator->mapping->at("dog") ==
         "#/components/schemas/Dog");

  // Edge cases in extract_tag & parse_api_docstrings
  assert(extract_tag("/// @title   \r\n", "title")->empty());
  openapi::OpenAPI spec_edge;
  parse_api_docstrings(
      "@servers\n- http://s1\n@server_variable v1 d1\n@server_variable v2 d2 "
      "[e1,e2]\n@other\n@securitySchemes\n- s1 apiKey\n"
      "@securityScheme_description unknown desc\n"
      "@securityScheme_openIdConnectUrl unknown http://u\n"
      "@securityScheme_oauth2MetadataUrl unknown http://o\n"
      "@securityScheme_deprecated unknown\n"
      "@other2\n",
      spec_edge);
  assert(spec_edge.servers.has_value() && spec_edge.servers->size() == 1);
  assert(spec_edge.servers->at(0).variables->size() == 2);

  // Test empty path/schema/operation docstrings and pre-populated responses
  openapi::PathItem empty_pi;
  parse_path_docstrings("", empty_pi);
  assert(!empty_pi.summary.has_value());

  openapi::Schema empty_schema;
  parse_schema_docstrings("", empty_schema);
  assert(!empty_schema.description.has_value());

  openapi::Operation op_pre;
  openapi::Response r_pre;
  r_pre.description = "OK";
  r_pre.content = std::map<std::string, openapi::MediaType>{{"text/plain", {}}};
  op_pre.responses = std::map<std::string, openapi::Response>{{"200", r_pre}};
  parse_operation_docstrings(
      "/// @return 200 - Updated\n/// @response_content 200 application/json\n",
      op_pre);
  assert(op_pre.responses->at("200").description == "Updated");
  assert(op_pre.responses->at("200").content->size() == 2);

  openapi::Operation op_rc_only;
  parse_operation_docstrings("/// @response_content 201 text/html\n",
                             op_rc_only);
  assert(op_rc_only.responses.has_value());

  std::cout << "docstrings::test_parse passed.\n";
}

void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test API";
  spec.info.summary = "API Summary";
  spec.info.version = "1.0";
  spec.info.description = "Line1\r\nLine2";

  openapi::ServerVariable sv;
  sv.default_value = "def";
  sv.enum_values = std::vector<std::string>{"val1", "val2"};
  sv.description = "desc";

  openapi::Server srv;
  srv.url = "http://localhost";
  srv.variables = std::map<std::string, openapi::ServerVariable>{{"myVar", sv}};
  spec.servers = std::vector<openapi::Server>{srv};

  openapi::Operation op;
  op.summary = "Op summary";
  op.description = "Op desc\r\nLine2";

  openapi::Parameter p;
  p.name = "myParam";
  p.in = "query";
  p.description = "Param desc\r\nLine2";
  op.parameters = std::vector<openapi::Parameter>{p};

  openapi::PathItem pi;
  pi.description = "Path desc\r\nLine2";
  pi.get = op;

  spec.paths = std::map<std::string, openapi::PathItem>{{"/test", pi}};

  std::string out = emit_api_docstrings(spec);
  assert(out.find("Line1") != std::string::npos);
  assert(out.find("Line2") != std::string::npos);
  assert(out.find("val1,val2") != std::string::npos);
  assert(out.find("@servers") != std::string::npos);
  assert(out.find("@summary API Summary") != std::string::npos);

  // Verify round-trip back through parse()
  openapi::OpenAPI round_trip = parse(out);
  assert(round_trip.info.title == "Test API");
  assert(round_trip.info.summary == "API Summary");
  assert(round_trip.servers.has_value() &&
         round_trip.servers->at(0).url == "http://localhost");

  // Test emitting minimal/empty optional subfields
  openapi::OpenAPI min_spec;
  min_spec.info.title = "Min";
  min_spec.info.version = "1";
  min_spec.info.contact = openapi::Contact{};
  min_spec.info.license = openapi::License{"MIT", std::nullopt, std::nullopt};
  openapi::Server min_srv;
  min_srv.url = "http://min";
  min_srv.variables = std::map<std::string, openapi::ServerVariable>{
      {"v", openapi::ServerVariable{std::nullopt, "d", std::nullopt}}};
  min_spec.servers = std::vector<openapi::Server>{min_srv};
  min_spec.components = openapi::Components{};
  openapi::SecurityScheme min_ss;
  min_ss.type = "http";
  min_ss.deprecated = false;
  min_spec.components->securitySchemes =
      std::map<std::string, openapi::SecurityScheme>{{"s", min_ss}};
  std::string min_out = emit_api_docstrings(min_spec);
  assert(min_out.find("@license_name MIT") != std::string::npos);

  openapi::OpenAPI empty_srv_spec;
  empty_srv_spec.servers = std::vector<openapi::Server>{};
  assert(emit_api_docstrings(empty_srv_spec).find("@servers") ==
         std::string::npos);

  openapi::Operation min_op;
  min_op.tags = std::vector<std::string>{};
  min_op.security = std::vector<openapi::SecurityRequirement>{};
  openapi::Response r_204;
  r_204.description = "No Content";
  min_op.responses = std::map<std::string, openapi::Response>{{"204", r_204}};
  std::string min_op_out = emit_operation_docstrings(min_op);
  assert(min_op_out.find("204 - No Content") != std::string::npos);

  std::cout << "docstrings::test_emit passed.\n";
}

} // namespace cdd_cpp::docstrings
