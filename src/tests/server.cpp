#include "../server/emit.hpp"
#include "../server/parse.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace cdd_cpp::server {

void test_parse() {
  std::string input = R"(
    /// @title Awesome API
    /// @version 2.0.0
    /// @description Best API
    /// @termsOfService https://terms
    /// @contact_name Samuel
    /// @contact_url https://samuel
    /// @contact_email samuel@test
    /// @license_name MIT
    /// @license_identifier MIT
    /// @license_url https://mit
    /// @servers
    /// - http://localhost (Local)
    ///   @server_variable var default desc [enum1]
    /// - http://remote
    /// @securitySchemes
    /// - BasicAuth http name header basic format
    ///   @securityScheme_description BasicAuth desc
    ///   @securityScheme_openIdConnectUrl BasicAuth https://oidc
    ///   @securityScheme_oauth2MetadataUrl BasicAuth https://oauth
    ///   @securityScheme_deprecated BasicAuth
    class Router {
    public:
        /// @summary Get a resource
        /// @description Retrieves a specific resource
        /// @tags api,v1
        /// @deprecated
        /// @security Bearer [admin,user]
        /// @response 200 - OK
        /// @response_content 200 application/json
        /// @response 404 - Not found
        void on_GET_getResource(std::function<std::string(int id, std::string name, std::string requestBody)> handler) {
            routes["GET /api/v1/resource/{id}"] = handler;
        }

        void on_POST_addResource(std::function<std::string(const std::string&)> handler) {
            routes["POST /api/v1/resource"] = handler;
        }

        void on_PUT_putResource(std::function<std::string()> handler) {
            routes["PUT /api/v1/resource"] = handler;
        }
        
        void on_DELETE_deleteResource(std::function<std::string(double d)> handler) {
            routes["DELETE /api/v1/resource"] = handler;
        }
        
        void on_PATCH_patchResource(std::function<std::string(bool b)> handler) {
            routes["PATCH /api/v1/resource"] = handler;
        }
    };
  )";

  auto spec = parse(input).value();
  
  assert(spec.info.title == "Awesome API");
  assert(spec.info.version == "2.0.0");
  assert(spec.info.description == "Best API");
  assert(spec.info.termsOfService == "https://terms");
  assert(spec.info.contact->name == "Samuel");
  assert(spec.info.contact->url == "https://samuel");
  assert(spec.info.contact->email == "samuel@test");
  assert(spec.info.license->name == "MIT");
  assert(spec.info.license->identifier == "MIT");
  assert(spec.info.license->url == "https://mit");
  assert(spec.servers.has_value() && spec.servers->size() == 2);
  assert(spec.servers->at(0).url == "http://localhost");
  assert(spec.servers->at(0).description == "Local");
  assert(spec.servers->at(0).variables->at("var").default_value == "default");
  assert(spec.servers->at(1).url == "http://remote");
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
  assert(!spec.servers->at(1).description.has_value());

  assert(spec.paths->contains("/api/v1/resource/{id}"));
  auto get_op = spec.paths->at("/api/v1/resource/{id}").get;
  assert(get_op.has_value());
  assert(get_op->summary == "Get a resource");
  assert(get_op->description == "Retrieves a specific resource");
  assert(get_op->tags.has_value() && get_op->tags->at(0) == "api");
  assert(get_op->deprecated == true);
  assert(get_op->security.has_value() && get_op->security->at(0).count("Bearer"));
  assert(get_op->responses->at("200").content->count("application/json"));
  assert(get_op->responses.has_value() && get_op->responses->size() == 2);
  assert(get_op->responses->at("200").description == "OK");
  assert(get_op->parameters.has_value() && get_op->parameters->size() == 2);
  assert(get_op->parameters->at(0).name == "id");
  assert(get_op->parameters->at(0).in == "path");
  assert(get_op->parameters->at(0).schema->type == "integer");
  assert(get_op->parameters->at(1).name == "name");
  assert(get_op->parameters->at(1).in == "query");
  assert(get_op->parameters->at(1).schema->type == "string");
  assert(get_op->requestBody.has_value());
  
  assert(spec.paths->contains("/api/v1/resource"));
  assert(spec.paths->at("/api/v1/resource").post.has_value());
  assert(!spec.paths->at("/api/v1/resource").post->parameters.has_value());
  
  assert(spec.paths->at("/api/v1/resource").put.has_value());
  assert(spec.paths->at("/api/v1/resource").delete_op.has_value());
  assert(spec.paths->at("/api/v1/resource").delete_op->parameters->at(0).schema->type == "number");
  assert(spec.paths->at("/api/v1/resource").patch.has_value());
  assert(spec.paths->at("/api/v1/resource").patch->parameters->at(0).schema->type == "boolean");

  std::cout << "routes::test_parse passed.\n";
}

void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test";
  spec.info.version = "1.0";
  spec.info.description = "Desc";
  spec.info.termsOfService = "https://terms";
  spec.info.contact = openapi::Contact{"Samuel", "https://samuel", "samuel@test"};
  spec.info.license = openapi::License{"MIT", "MIT", "https://mit"};

  openapi::ServerVariable sv;
  sv.default_value = "default";
  sv.description = "desc";
  sv.enum_values = std::vector<std::string>{"enum1"};
  openapi::Server srv;
  srv.url = "http://api";
  srv.description = "API server";
  srv.variables = std::map<std::string, openapi::ServerVariable>{{"var", sv}};
  
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
  openapi::Server srv2;
  srv2.url = "http://remote";
  spec.servers = std::vector<openapi::Server>{srv, srv2};

  spec.paths = std::map<std::string, openapi::PathItem>{};

  openapi::PathItem item;
  
  openapi::Operation op;
  op.operationId = "testRoute";
  op.summary = "Sum";
  op.description = "Desc";
  op.tags = std::vector<std::string>{"api"};
  op.deprecated = true;
  openapi::SecurityRequirement sr;
  sr["Bearer"] = std::vector<std::string>{"admin", "user"};
  op.security = std::vector<openapi::SecurityRequirement>{sr};
  openapi::Response resp;
  resp.description = "OK";
  resp.content = std::map<std::string, openapi::MediaType>{{"application/json", openapi::MediaType{}}};
  op.responses = std::map<std::string, openapi::Response>{{"200", resp}};
  
  openapi::Parameter p1, p2, p3, p4, p5;
  p1.name = "id";
  p1.schema = openapi::Schema{};
  p1.schema->type = "integer";
  p2.name = "name";
  p2.schema = openapi::Schema{};
  p2.schema->type = "string";
  p3.name = "score";
  p3.schema = openapi::Schema{};
  p3.schema->type = "number";
  p4.name = "active";
  p4.schema = openapi::Schema{};
  p4.schema->type = "boolean";
  p5.name = "other";
  op.parameters = std::vector<openapi::Parameter>{p1, p2, p3, p4, p5};
  op.requestBody = openapi::RequestBody{};
  
  item.get = op;
  item.post = op;
  item.put = op;
  item.delete_op = op;
  item.patch = op;
  spec.paths->insert({"/api/v1/test", item});

  openapi::PathItem item2;
  openapi::Operation op2;
  item2.get = op2;
  spec.paths->insert({"/empty", item2});

  std::string code = emit(spec);
  assert(code.find("on_GET_testRoute") != std::string::npos);
  assert(code.find("@tags api") != std::string::npos);
  assert(code.find("@deprecated") != std::string::npos);
  assert(code.find("@security Bearer") != std::string::npos);
  assert(code.find("@response_content 200 application/json") != std::string::npos);
  assert(code.find("@termsOfService https://terms") != std::string::npos);
  assert(code.find("@contact_name Samuel") != std::string::npos);
  assert(code.find("@license_name MIT") != std::string::npos);
  assert(code.find("@server_variable var default desc [enum1]") != std::string::npos);
  assert(code.find("@securitySchemes") != std::string::npos);
  assert(code.find("- BasicAuth http name header basic format") != std::string::npos);
  assert(code.find("@securityScheme_description BasicAuth desc") != std::string::npos);
  assert(code.find("@securityScheme_openIdConnectUrl BasicAuth https://oidc") != std::string::npos);
  assert(code.find("@securityScheme_oauth2MetadataUrl BasicAuth https://oauth") != std::string::npos);
  assert(code.find("@securityScheme_deprecated BasicAuth") != std::string::npos);
  assert(code.find("int id, std::string name, double score, bool active, std::string other, std::string requestBody") != std::string::npos);
  assert(code.find("routes[\"GET /api/v1/test\"] = handler;") != std::string::npos);
  assert(code.find("on_GET_handler") != std::string::npos);

  std::cout << "routes::test_emit passed.\n";
}
} // namespace cdd_cpp::server