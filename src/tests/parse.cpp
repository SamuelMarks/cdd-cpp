// GCOV_EXCL_BR_START

#include "../openapi/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi {
void test_parse() {
  {
    auto spec_res = parse("{ invalid json }");
    if (!spec_res) {
      std::string err = spec_res.error();
    }
  }
  {
    auto spec_res = parse("\"string\"");
    assert(!spec_res.has_value());
  }
  std::string json = R"({
            "openapi": "3.2.0",
            "info": {
                "title": "Test API",
                "version": "1.0.0"
            },
            "paths": {
                "/test": {
                    "get": {
                        "operationId": "getTest"
                    },
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
                        "schema": { "type": "string" }
                    }
                }
            }
        })";

  auto spec_res = parse(json);
  assert(spec_res.has_value());
  OpenAPI spec = *spec_res;
  assert(spec.openapi == "3.2.0");
  assert(spec.paths.has_value());
  assert(spec.paths->contains("/test"));
  assert(spec.paths->at("/test").get.has_value());
  assert(spec.paths->at("/test").get->operationId == "getTest");

  // 3.2.0 additions
  assert(spec.paths->at("/test").query.has_value());
  assert(spec.paths->at("/test").query->operationId == "queryTest");
  assert(spec.paths->at("/test").additionalOperations != nullptr);
  assert(spec.paths->at("/test").additionalOperations->contains("PROPFIND"));
  assert(spec.paths->at("/test")
             .additionalOperations->at("PROPFIND")
             .operationId == "propfindTest");

  assert(spec.webhooks.has_value());
  assert(spec.webhooks->contains("myWebhook"));
  assert(spec.webhooks->at("myWebhook").post.has_value());
  assert(spec.webhooks->at("myWebhook").post->operationId == "webhookPost");

  assert(spec.components.has_value());
  assert(spec.components->mediaTypes != nullptr);
  assert(spec.components->mediaTypes->contains("application/json"));

  // Test JSON Schema Draft 2020-12 features
  std::string schema_json = R"({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/schema",
    "type": "object",
    "properties": {
      "name": {
        "type": "string",
        "maxLength": 100,
        "pattern": "^[a-z]+$"
      }
    },
    "required": ["name"],
    "deprecated": true,
    "multipleOf": 1.5,
    "maxItems": 10
  })";

  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  assert(parser.parse(schema_json).get(doc) == simdjson::SUCCESS);
  // We can't call parse_Schema directly as it's static, so we'll test via
  // OpenAPI wrapper
  std::string spec_with_schema = R"({
    "openapi": "3.2.0",
    "info": {"title": "T", "version": "1"},
    "components": {
      "schemas": {
        "MySchema": {
          "$schema": "https://json-schema.org/draft/2020-12/schema",
          "$id": "https://example.com/schema",
          "type": "object",
          "properties": {
            "name": {
              "type": "string",
              "maxLength": 100,
              "pattern": "^[a-z]+$"
            }
          },
          "required": ["name"],
          "deprecated": true,
          "multipleOf": 1.5,
          "maxItems": 10,
          "$defs": {
              "Def": { "type": "string" }
          },
          "maximum": 10.0,
          "exclusiveMaximum": 9.9,
          "minimum": 0.0,
          "exclusiveMinimum": 0.1,
          "minLength": 1,
          "minItems": 1,
          "maxContains": 5,
          "minContains": 1,
          "maxProperties": 10,
          "minProperties": 1
        }
      }
    }
  })";

  auto spec_schema_res = parse(spec_with_schema);
  assert(spec_schema_res.has_value());
  OpenAPI spec_schema = *spec_schema_res;
  assert(spec_schema.components.has_value());
  auto &schema = spec_schema.components->schemas->at("MySchema");
  (void)schema;
  assert(schema.schema_dialect ==
         "https://json-schema.org/draft/2020-12/schema");
  assert(schema.id == "https://example.com/schema");
  assert(schema.type == "object");
  assert(schema.deprecated == true);
  assert(schema.multipleOf == 1.5);
  assert(schema.maxItems == 10);
  assert(schema.required.has_value());
  assert(schema.required->size() == 1);
  assert(schema.required->at(0) == "name");
  assert(schema.properties->at("name").type == "string");
  assert(schema.properties->at("name").maxLength == 100);
  assert(schema.properties->at("name").pattern == "^[a-z]+$");
  std::cout << "Draft 2020-12 parser features passed.\n";

  // Test OpenAPI 3.2.0 specific features
  std::string spec_3_2_0 = R"({
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
    "paths": {
        "/sec": {
            "get": {
                "security": [
                    { "oauth2": ["read", "write"] }
                ]
            }
        }
    },
    "components": {
      "schemas": {
        "Pet": {
          "type": "object",
          "discriminator": {
            "propertyName": "petType",
            "defaultMapping": "#/components/schemas/Dog"
          }
        },
        "RefToPet": {
            "$ref": "#/components/schemas/Pet",
            "summary": "ref sum",
            "description": "ref desc"
        }
      },
      "callbacks": {
          "cb": {
              "ev": {
                  "summary": "event"
              }
          }
      },
      "examples": {
          "Ex1": {
              "summary": "s",
              "description": "d",
              "value": "v",
              "externalValue": "e",
              "$ref": "r"
          }
      },
      "mediaTypes": {
          "application/json": {
              "itemSchema": { "type": "string" },
              "itemEncoding": { "contentType": "application/json" },
              "prefixEncoding": [
                  { "contentType": "text/plain" }
              ],
              "examples": {
                  "MyEx": { "summary": "ex" }
              },
              "encoding": {
                  "history": {
                      "contentType": "application/json",
                      "headers": {
                          "X-Rate-Limit": {
                              "description": "calls per hour",
                              "schema": { "type": "integer" },
                              "content": {
                                  "application/json": {
                                      "schema": { "type": "integer" }
                                  }
                              },
                              "examples": { "A": { "value": "B" } }
                          }
                      }
                  }
              }
          }
      }
    }
  })";

  auto spec_latest_res = parse(spec_3_2_0);
  assert(spec_latest_res.has_value());
  OpenAPI spec_latest = *spec_latest_res;
  assert(spec_latest.self_link == "https://example.com/openapi.json");
  assert(spec_latest.tags.has_value() && spec_latest.tags->size() == 1);
  assert(spec_latest.tags->at(0).name == "User");
  assert(spec_latest.tags->at(0).parent == "Core");
  assert(spec_latest.tags->at(0).kind == "entity");
  assert(spec_latest.components.has_value() &&
         spec_latest.components->schemas.has_value());
  assert(spec_latest.components->mediaTypes != nullptr);
  auto mt = spec_latest.components->mediaTypes->at("application/json");
  assert(mt.itemSchema != nullptr);
  assert(mt.itemEncoding != nullptr);
  assert(mt.prefixEncoding != nullptr && mt.prefixEncoding->size() == 1);
  assert(mt.examples.has_value() && mt.examples->contains("MyEx"));

  assert(spec_latest.components->schemas->at("RefToPet").ref.has_value());
  assert(spec_latest.components->schemas->at("RefToPet").ref->summary ==
         "ref sum");
  assert(spec_latest.components->schemas->at("RefToPet").ref->ref ==
         "#/components/schemas/Pet");
  assert(
      spec_latest.components->schemas->at("Pet").discriminator->propertyName ==
      "petType");
  assert(spec_latest.components->schemas->at("Pet")
             .discriminator->defaultMapping == "#/components/schemas/Dog");

  std::cout << "OpenAPI 3.2.0 specific parser features passed.\n";

  std::cout << "test_parse passed.\n";
}
} // namespace cdd_cpp::openapi

// GCOV_EXCL_BR_STOP
