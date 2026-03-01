#include "../openapi/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi {
void test_parse() {
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

  OpenAPI spec = parse(json);
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
  auto error = parser.parse(schema_json).get(doc);
  assert(error == simdjson::SUCCESS);
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
          "maxItems": 10
        }
      }
    }
  })";

  OpenAPI spec_schema = parse(spec_with_schema);
  assert(spec_schema.components.has_value());
  auto &schema = spec_schema.components->schemas->at("MySchema");
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

  OpenAPI spec_latest = parse(spec_3_2_0);
  assert(spec_latest.self_link == "https://example.com/openapi.json");
  assert(spec_latest.tags.has_value() && spec_latest.tags->size() == 1);
  assert(spec_latest.tags->at(0).name == "User");
  assert(spec_latest.tags->at(0).parent == "Core");
  assert(spec_latest.tags->at(0).kind == "entity");
  assert(spec_latest.components.has_value() &&
         spec_latest.components->schemas.has_value());
  assert(spec_latest.components->schemas->at("Pet").discriminator.has_value());
  assert(
      spec_latest.components->schemas->at("Pet").discriminator->propertyName ==
      "petType");
  assert(spec_latest.components->schemas->at("Pet")
             .discriminator->defaultMapping == "#/components/schemas/Dog");

  std::cout << "OpenAPI 3.2.0 specific parser features passed.\n";

  std::cout << "test_parse passed.\n";
}
} // namespace cdd_cpp::openapi
