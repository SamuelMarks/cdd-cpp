#include "../openapi/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi {

void test_openapi_parse_details() {
  std::string json = R"({
    "openapi": "3.0.0",
    "info": {"title": "T", "version": "1"},
    "components": {
      "schemas": {
        "S1": {
          "type": "object",
          "properties": {"p1": {"type": "string"}},
          "allOf": [{"type": "string"}],
          "anyOf": [{"type": "string"}],
          "oneOf": [{"type": "string"}],
          "not": {"type": "string"},
          "items": {"type": "string"},
          "xml": {"name": "xml", "namespace": "ns", "prefix": "pref", "attribute": true, "wrapped": true}
        },
        "S2": {
            "$ref": "#/components/schemas/S1",
            "summary": "ref sum",
            "description": "ref desc"
        }
      },
      "responses": {
        "R1": {
            "description": "r1",
            "content": {"app/json": {"schema": {"type": "string"}, "example": {"k": "v"}, "examples": {"ex1": {"value": {"k": "v"}, "summary": "sum", "description": "desc", "externalValue": "http", "$ref": "#/components/examples/Ex1"}}}},
            "headers": {"H1": {"description": "h1", "required": true, "deprecated": true, "allowEmptyValue": true, "style": "simple", "explode": true, "allowReserved": true, "schema": {"type": "string"}, "example": {"k": "v"}}},
            "links": {"L1": {"operationRef": "ref", "operationId": "id", "parameters": {"p1": "v1"}, "requestBody": "body", "description": "desc", "server": {"url": "http"}}}
        }
      },
      "parameters": {
        "P1": {
            "name": "p1",
            "in": "query",
            "description": "desc",
            "required": true,
            "deprecated": true,
            "allowEmptyValue": true,
            "style": "form",
            "explode": true,
            "allowReserved": true,
            "schema": {"type": "string"},
            "example": {"k": "v"},
            "examples": {"ex1": {"value": "v"}},
            "content": {"app/json": {"schema": {"type": "string"}}}
        }
      },
      "examples": {
        "Ex1": {"summary": "sum", "description": "desc", "value": {"k": "v"}, "externalValue": "http"}
      },
      "requestBodies": {
        "Req1": {"description": "desc", "content": {"app/json": {"schema": {"type": "string"}}}, "required": true}
      },
      "headers": {
        "H1": {"description": "h1", "schema": {"type": "string"}}
      },
      "securitySchemes": {
        "Sec1": {
            "type": "oauth2",
            "description": "desc",
            "name": "n",
            "in": "header",
            "scheme": "bearer",
            "bearerFormat": "jwt",
            "openIdConnectUrl": "http",
            "flows": {
                "implicit": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "password": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "clientCredentials": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "authorizationCode": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "deviceAuthorization": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}}
            }
        }
      },
      "links": {
        "L1": {"operationRef": "ref"}
      },
      "callbacks": {
        "Cb1": {"url": {"summary": "sum", "description": "desc"}}
      },
      "pathItems": {
        "PI1": {"summary": "sum", "description": "desc"}
      }
    }
  })";
  auto spec_res = parse(json);
  assert(spec_res.has_value());
}

void test_openapi_parse_final_details() {
  std::string json = R"({
    "openapi": "3.0.0",
    "info": {"title": "T", "version": "1"},
    "components": {
      "schemas": {
        "S1": {
          "type": "object",
          "properties": {"p1": {"type": "string"}},
          "allOf": [{"type": "string"}],
          "anyOf": [{"type": "string"}],
          "oneOf": [{"type": "string"}],
          "not": {"type": "string"},
          "items": {"type": "string"},
          "xml": {"name": "xml", "namespace": "ns", "prefix": "pref", "attribute": true, "wrapped": true},
          "discriminator": {"propertyName": "petType", "mapping": {"dog": "#/components/schemas/Dog"}, "defaultMapping": "dog"},
          "readOnly": true,
          "writeOnly": true,
          "nullable": true,
          "deprecated": true,
          "multipleOf": 2.5,
          "maximum": 10.0,
          "exclusiveMaximum": true,
          "minimum": 0.0,
          "exclusiveMinimum": true,
          "maxLength": 100,
          "minLength": 10,
          "pattern": "^[a-z]+$",
          "maxItems": 10,
          "minItems": 1,
          "uniqueItems": true,
          "maxProperties": 10,
          "minProperties": 1
        },
        "S2": {
            "$ref": "#/components/schemas/S1",
            "summary": "ref sum",
            "description": "ref desc"
        }
      },
      "responses": {
        "R1": {
            "description": "r1",
            "content": {"app/json": {"schema": {"type": "string"}, "example": {"k": "v"}, "examples": {"ex1": {"value": {"k": "v"}, "summary": "sum", "description": "desc", "externalValue": "http", "$ref": "#/components/examples/Ex1"}}}},
            "headers": {"H1": {"description": "h1", "required": true, "deprecated": true, "allowEmptyValue": true, "style": "simple", "explode": true, "allowReserved": true, "schema": {"type": "string"}, "example": {"k": "v"}}},
            "links": {"L1": {"operationRef": "ref", "operationId": "id", "parameters": {"p1": "v1"}, "requestBody": "body", "description": "desc", "server": {"url": "http"}}}
        },
        "R2": {"$ref": "#/components/responses/R1"}
      },
      "parameters": {
        "P1": {
            "name": "p1",
            "in": "query",
            "description": "desc",
            "required": true,
            "deprecated": true,
            "allowEmptyValue": true,
            "style": "form",
            "explode": true,
            "allowReserved": true,
            "schema": {"type": "string"},
            "example": {"k": "v"},
            "examples": {"ex1": {"value": "v"}},
            "content": {"app/json": {"schema": {"type": "string"}}}
        },
        "P2": {"$ref": "#/components/parameters/P1"}
      },
      "examples": {
        "Ex1": {"summary": "sum", "description": "desc", "value": {"k": "v"}, "externalValue": "http"}
      },
      "requestBodies": {
        "Req1": {"description": "desc", "content": {"app/json": {"schema": {"type": "string"}}}, "required": true},
        "Req2": {"$ref": "#/components/requestBodies/Req1"}
      },
      "headers": {
        "H1": {"description": "h1", "schema": {"type": "string"}},
        "H2": {"$ref": "#/components/headers/H1"}
      },
      "securitySchemes": {
        "Sec1": {
            "type": "oauth2",
            "description": "desc",
            "name": "n",
            "in": "header",
            "scheme": "bearer",
            "bearerFormat": "jwt",
            "openIdConnectUrl": "http",
            "flows": {
                "implicit": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "password": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "clientCredentials": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "authorizationCode": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}},
                "deviceAuthorization": {"authorizationUrl": "http", "tokenUrl": "http", "refreshUrl": "http", "scopes": {"read": "read access"}}
            }
        },
        "Sec2": {"$ref": "#/components/securitySchemes/Sec1"}
      },
      "links": {
        "L1": {"operationRef": "ref"},
        "L2": {"$ref": "#/components/links/L1"}
      },
      "callbacks": {
        "Cb1": {"url": {"summary": "sum", "description": "desc"}}
      },
      "pathItems": {
        "PI1": {
          "summary": "sum",
          "description": "desc",
          "$ref": "#/components/pathItems/PI2",
          "put": {"operationId": "put1"},
          "post": {"operationId": "post1"},
          "delete": {"operationId": "del1"},
          "options": {"operationId": "opt1"},
          "head": {"operationId": "head1"},
          "patch": {"operationId": "patch1"},
          "trace": {"operationId": "trace1"},
          "servers": [{"url": "http://server"}],
          "parameters": [{"name": "p", "in": "query"}]
        }
      }
    }
  })";
  auto spec_res = parse(json);
  assert(spec_res.has_value());
}

void test_openapi_parse() {
  std::string json = R"({
    "openapi": "3.0.0",
    "info": {
      "title": "Test",
      "version": "1.0.0",
      "description": "Test Desc",
      "termsOfService": "http://test.com",
      "contact": {"name": "API Support", "url": "http://www.example.com/support", "email": "support@example.com"},
      "license": {"name": "Apache 2.0", "url": "https://www.apache.org/licenses/LICENSE-2.0.html"}
    },
    "servers": [
      {
        "url": "https://api.example.com/v1",
        "description": "Main server",
        "variables": {
            "port": { "default": "8443", "enum": ["8443", "443"], "description": "Port" }
        }
      }
    ],
    "paths": {
      "/test": {
        "summary": "Path summary",
        "description": "Path desc",
        "get": {
          "tags": ["test"],
          "summary": "Get test",
          "description": "Get test desc",
          "externalDocs": {"description": "Find more info here", "url": "https://example.com"},
          "operationId": "getTest",
          "parameters": [
            {
              "name": "id",
              "in": "query",
              "description": "ID",
              "required": true,
              "deprecated": false,
              "allowEmptyValue": false,
              "example": "123",
              "schema": {"type": "integer"}
            }
          ],
          "requestBody": {
            "description": "user to add",
            "content": {
              "application/json": {
                "schema": {"$ref": "#/components/schemas/User"}
              }
            },
            "required": true
          },
          "responses": {
            "200": {
              "description": "OK",
              "headers": {
                "X-Rate-Limit": {"description": "calls per hour", "schema": {"type": "integer"}}
              },
              "content": {
                "application/json": {
                  "schema": {"type": "string"}
                }
              }
            }
          },
          "deprecated": true,
          "security": [{"api_key": []}],
          "servers": [{"url": "https://test.com"}]
        }
      }
    },
    "components": {
      "schemas": {
        "User": {
          "type": "object",
          "required": ["id"],
          "properties": {
            "id": {"type": "integer"},
            "name": {"type": "string"}
          }
        }
      },
      "responses": {
        "NotFound": {"description": "Entity not found."}
      },
      "parameters": {
        "skipParam": {"name": "skip", "in": "query", "description": "number of items to skip", "required": true, "schema": {"type": "integer", "format": "int32"}}
      },
      "examples": {
        "userExample": {"summary": "User Example", "value": {"id": 1, "name": "Jessica"}}
      },
      "requestBodies": {
        "UserArray": {"description": "List of user object", "content": {"application/json": {"schema": {"type": "array", "items": {"$ref": "#/components/schemas/User"}}}}}
      },
      "headers": {
        "X-Rate-Limit": {"description": "calls per hour", "schema": {"type": "integer"}}
      },
      "securitySchemes": {
        "api_key": {"type": "apiKey", "name": "api_key", "in": "header"}
      },
      "links": {
        "Unsubscribe": {"operationId": "cancel", "parameters": {"Id": "$response.body#/id"}}
      },
      "callbacks": {
        "onData": {"{$request.query.callbackUrl}": {"post": {"requestBody": {"content": {"application/json": {"schema": {"$ref": "#/components/schemas/User"}}}}, "responses": {"200": {"description": "webhook processed"}}}}}
      }
    },
    "security": [{"api_key": []}],
    "tags": [{"name": "test", "description": "Test Tag", "externalDocs": {"url": "http://test.com"}}],
    "externalDocs": {"description": "Find more info here", "url": "https://example.com"}
  })";

  auto spec_res = parse(json);
  assert(spec_res.has_value());
  auto spec = *spec_res;
  assert(spec.info.title == "Test");
  assert(spec.info.contact->name == "API Support");
  assert(spec.info.license->name == "Apache 2.0");
  assert(spec.servers->size() == 1);
  assert(spec.servers->front().variables->size() == 1);
  assert(spec.paths->size() == 1);
  assert(spec.paths->begin()->second.get->responses->size() == 1);
  assert(spec.components->schemas->size() == 1);
  assert(spec.components->responses->size() == 1);
  assert(spec.components->parameters->size() == 1);
  assert(spec.components->examples->size() == 1);
  assert(spec.components->requestBodies->size() == 1);
  assert(spec.components->headers->size() == 1);
  assert(spec.components->securitySchemes->size() == 1);
  assert(spec.components->links->size() == 1);
  assert(spec.components->callbacks->size() == 1);
  assert(spec.security->size() == 1);
  assert(spec.tags->size() == 1);
  assert(spec.externalDocs->url == "https://example.com");

  // Discriminator test
  std::string json2 = R"({
      "openapi": "3.0.0",
      "info": {"title": "Test", "version": "1.0.0"},
      "components": {
          "schemas": {
              "Pet": {
                  "type": "object",
                  "discriminator": {"propertyName": "petType", "mapping": {"dog": "#/components/schemas/Dog"}},
                  "required": ["petType"],
                  "properties": {"petType": {"type": "string"}}
              },
              "XMLTest": {
                  "type": "object",
                  "xml": {"name": "animal", "namespace": "http://example.com/schema", "prefix": "smp", "attribute": true, "wrapped": false}
              }
          }
      }
  })";
  auto spec2 = parse(json2);
  assert(spec2.has_value());
  auto pet = spec2->components->schemas->at("Pet");
  assert(pet.discriminator->propertyName == "petType");
  assert(pet.discriminator->mapping->at("dog") == "#/components/schemas/Dog");
  auto xmlTest = spec2->components->schemas->at("XMLTest");
  assert(xmlTest.xml->name == "animal");

  test_openapi_parse_details();
  test_openapi_parse_final_details();

  std::string cb_json = R"({
    "openapi": "3.1.0",
    "info": {"title": "x", "version": "1"},
    "paths": {
      "/": {
        "post": {
          "callbacks": {
            "myEvent": {
              "{$request.body#/callbackUrl}": {
                "post": {
                  "requestBody": {
                    "content": {
                      "application/json": {}
                    }
                  },
                  "responses": {
                    "200": {
                      "description": "ok"
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  })";
  auto cb_res = parse(cb_json);
  assert(cb_res);
  assert(cb_res->paths->at("/").post->callbacks);
  assert(cb_res->paths->at("/").post->callbacks->contains("myEvent"));

  std::cout << "openapi::test_openapi_parse passed.\n";
}
} // namespace cdd_cpp::openapi
