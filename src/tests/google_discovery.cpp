
#include "../google_discovery/parse.hpp"
#include "../openapi/emit.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::google_discovery {
void test_parse() {
  std::string list_json = R"({
            "kind": "discovery#directoryList",
            "items": [
                {
                    "kind": "discovery#directoryItem",
                    "title": "API 1",
                    "version": "v1",
                    "discoveryRestUrl": "https://api1.com"
                },
                {
                    "kind": "discovery#directoryItem",
                    "title": "API 2",
                    "version": "v2",
                    "description": "desc2",
                    "discoveryRestUrl": "https://api2.com"
                }
            ]
        })";

  auto list_res = parse(list_json);
  assert(list_res->size() == 2);
  assert((*list_res)[0].info.title == "API 1");
  assert((*list_res)[1].info.title == "API 2");
  assert((*list_res)[1].info.description == "desc2");

  std::string rest_json = R"({
            "kind": "discovery#restDescription",
            "title": "Test Service",
            "version": "v1",
            "description": "desc",
            "rootUrl": "https://api.test.com/",
            "servicePath": "v1/",
            "schemas": {
                "User": {
                    "type": "object",
                    "description": "A user",
                    "properties": {
                        "id": {"type": "string", "format": "uuid"},
                        "arr": {
                            "type": "array",
                            "items": {"type": "string"}
                        },
                        "refProp": {"$ref": "Other"}
                    }
                }
            },
            "methods": {
                "getTop": {
                    "id": "getTop",
                    "httpMethod": "GET",
                    "path": "top"
                },
                "badMethod": "invalid"
            },
            "resources": {
                "users": {
                    "resources": {
                        "posts": {
                             "methods": {
                                  "getPost": {
                                      "id": "getPost",
                                      "httpMethod": "GET",
                                      "path": "posts/{id}"
                                  }
                             }
                        }
                    },
                    "methods": {
                        "get": {
                            "id": "users.get",
                            "httpMethod": "GET",
                            "description": "desc",
                            "path": "users/{+id}",
                            "parameters": {
                                "id": {
                                    "type": "string",
                                    "location": "path",
                                    "required": true
                                },
                                "filter": {
                                    "type": "string"
                                }
                            },
                            "request": {"$ref": "User"},
                            "response": {"$ref": "User"}
                        },
                        "post": {
                            "id": "users.post",
                            "httpMethod": "POST",
                            "path": "users"
                        },
                        "put": {
                            "id": "users.put",
                            "httpMethod": "PUT",
                            "path": "users/{id}"
                        },
                        "delete": {
                            "id": "users.delete",
                            "httpMethod": "DELETE",
                            "path": "users/{id}"
                        },
                        "patch": {
                            "id": "users.patch",
                            "httpMethod": "PATCH",
                            "path": "users/{id}"
                        }
                    }
                }
            }
        })";

  auto rest_res = parse(rest_json);
  assert(rest_res->size() == 1);
  assert((*rest_res)[0].info.title == "Test Service");
  assert((*rest_res)[0].servers->front().url == "https://api.test.com/v1/");
  assert((*rest_res)[0].components->schemas->contains("User"));
  assert((*rest_res)[0].paths->contains("/users/{id}"));

  auto err_parse = parse("invalid_json");
  assert(!err_parse);
  auto err_parse2 = parse("[]");
  assert(!err_parse2);

  // Test minimal/edge discovery documents
  assert(parse("{}")->empty());
  assert(parse(R"({"kind":"unknown"})")->empty());
  assert(parse(R"({"kind":"discovery#directoryList"})")->empty());
  assert(parse(R"({"kind":"discovery#restDescription"})")->size() == 1);
  auto edge_list =
      parse(R"({"kind":"discovery#directoryList","items":[123, {}]})");
  assert(edge_list && edge_list->size() == 1);

  auto edge_rest = parse(R"({
    "kind": "discovery#restDescription",
    "name": "FallbackName",
    "documentationLink": "https://docs.example.com",
    "baseUrl": "https://base.example.com/v1",
    "parameters": {
      "alt": {
        "type": "string",
        "location": "query",
        "description": "Data format",
        "default": "json",
        "enum": ["json", "media", 123],
        "pattern": "^[a-z]+$",
        "minimum": "1",
        "maximum": "100",
        "deprecated": true,
        "repeated": true
      },
      "rawAny": {
        "type": "any",
        "location": "header",
        "minimum": "not_a_number",
        "maximum": "not_a_number",
        "enum": [123]
      },
      "defaultLoc": {},
      "badGlobal": 123
    },
    "auth": {
      "oauth2": {
        "scopes": {
          "https://www.googleapis.com/auth/cloud": {
            "description": "Cloud access"
          },
          "https://www.googleapis.com/auth/nodedesc": {}
        }
      }
    },
    "schemas": {
      "NonObj": 123,
      "DetailedSchema": {
        "type": "object",
        "pattern": ".*",
        "default": "{}",
        "readOnly": true,
        "deprecated": true,
        "enum": ["a", "b", 123],
        "properties": {
          "reqProp": { "type": "string", "required": true },
          "anyProp": { "type": "any" },
          "badProp": 123
        },
        "additionalProperties": { "type": "string" },
        "variant": {
          "discriminant": "kind",
          "map": [
            { "type_value": "user", "$ref": "User" },
            { "$ref": "Other" },
            { "type_value": "noref" },
            123
          ]
        }
      },
      "EmptyEnumAndVariant": {
        "enum": [123],
        "variant": {}
      }
    },
    "methods": {
      "minimal": { "httpMethod": "GET", "path": "min", "parameters": { "badParam": 123 } },
      "noPath": { "httpMethod": "GET" },
      "noHttpMethod": { "path": "nohttp" },
      "headOp": {
        "httpMethod": "HEAD",
        "flatPath": "v1/items/{/itemId}",
        "deprecated": true,
        "supportsSubscription": true,
        "supportsMediaUpload": true,
        "supportsMediaDownload": true,
        "mediaUpload": {
          "accept": ["image/png", 123]
        },
        "scopes": ["https://www.googleapis.com/auth/cloud", 123]
      },
      "optionsOp": {
        "httpMethod": "OPTIONS",
        "path": "/{/opt}",
        "request": { "$ref": "DetailedSchema" },
        "supportsMediaUpload": true,
        "supportsMediaDownload": true,
        "scopes": [123]
      },
      "unknownVerb": {
        "httpMethod": "CUSTOM",
        "path": "custom"
      }
    },
    "resources": {
      "nonObjRes": 123,
      "emptyRes": {}
    }
  })");
  assert(edge_rest && edge_rest->size() == 1);
  assert((*edge_rest)[0].info.title == "FallbackName");
  assert((*edge_rest)[0].externalDocs->url == "https://docs.example.com");
  assert((*edge_rest)[0].servers->front().url == "https://base.example.com/v1");
  assert((*edge_rest)[0].components->securitySchemes->contains("Oauth2"));
  assert((*edge_rest)[0].components->parameters->contains("alt"));
  assert((*edge_rest)[0].paths->contains("/v1/items/{itemId}"));
  assert((*edge_rest)[0].paths->contains("/{opt}"));

  // Test rootUrl + servicePath with leading slash and no-kind fallback
  auto slash_service = parse(R"({
    "rootUrl": "https://api.test.com/",
    "servicePath": "/v2/items",
    "methods": {}
  })");
  assert(slash_service && slash_service->size() == 1);
  assert((*slash_service)[0].servers->front().url ==
         "https://api.test.com/v2/items");

  auto dir_item_extras = parse(R"({
    "kind": "discovery#directoryList",
    "items": [
      {
        "name": "FallbackItemName",
        "documentationLink": "https://item.docs.com"
      }
    ]
  })");
  assert(dir_item_extras && dir_item_extras->size() == 1);
  assert((*dir_item_extras)[0].info.title == "FallbackItemName");
  assert((*dir_item_extras)[0].externalDocs->url == "https://item.docs.com");

  auto auth_edge = parse(R"({
    "kind": "discovery#restDescription",
    "rootUrl": "https://api.test.com",
    "auth": { "oauth2": { "scopes": { "s1": "not_an_obj" } } },
    "schemas": {
      "EmptyMapVariant": {
        "variant": {
          "discriminant": "k",
          "map": [{ "$ref": "OnlyRef" }]
        }
      }
    },
    "methods": {
      "m1": {
        "httpMethod": "GET",
        "path": "items{/id}/trailing{",
        "parameters": {
          "id": { "location": "path", "format": "int64", "repeated": false },
          "q": { "required": false }
        },
        "request": {},
        "mediaUpload": {},
        "response": { "$ref": "EmptyMapVariant" },
        "supportsMediaDownload": true
      },
      "m2": {
        "httpMethod": "POST",
        "path": "",
        "response": {},
        "supportsMediaDownload": false
      }
    }
  })");
  assert(auth_edge && auth_edge->size() == 1);
  auto auth_edge2 = parse(R"({
    "kind": "discovery#restDescription",
    "rootUrl": "https://api.test.com",
    "servicePath": "/v2",
    "auth": { "oauth2": {} }
  })");
  assert(auth_edge2 && auth_edge2->size() == 1);
  auto auth_edge3 = parse(R"({
    "kind": "discovery#restDescription",
    "auth": {}
  })");
  assert(auth_edge3 && auth_edge3->size() == 1);
  auto fallback_schemas = parse(R"({"schemas":{}})");
  assert(fallback_schemas && fallback_schemas->size() == 1);
  auto fallback_resources = parse(R"({"resources":{}})");
  assert(fallback_resources && fallback_resources->size() == 1);

  std::cout << "google_discovery::test_parse passed.\n";
}
} // namespace cdd_cpp::google_discovery
