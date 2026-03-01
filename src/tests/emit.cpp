#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::openapi {
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
                        "itemSchema": { "type": "string" },
                        "prefixEncoding": [
                            {"contentType": "text/plain"}
                        ],
                        "itemEncoding": {
                            "contentType": "application/json"
                        }
                    }
                }
            }
        })";

  OpenAPI spec = parse(json);
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

  OpenAPI spec_3 = parse(json_3_2_0);
  std::string out_3 = emit(spec_3);
  
  assert(out_3.find("\"$self\":\"https://example.com/openapi.json\"") != std::string::npos);
  assert(out_3.find("\"parent\":\"Core\"") != std::string::npos);
  assert(out_3.find("\"kind\":\"entity\"") != std::string::npos);
  assert(out_3.find("\"defaultMapping\":\"#/components/schemas/Dog\"") != std::string::npos);

  std::cout << "test_emit passed.\n";
}
} // namespace cdd_cpp::openapi
