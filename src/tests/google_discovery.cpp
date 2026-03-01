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
                    "discoveryRestUrl": "https://api2.com"
                }
            ]
        })";

  auto list_res = parse(list_json);
  assert(list_res.size() == 2);
  assert(list_res[0].info.title == "API 1");
  assert(list_res[1].info.title == "API 2");

  std::string rest_json = R"({
            "kind": "discovery#restDescription",
            "title": "Test Service",
            "version": "v1",
            "rootUrl": "https://api.test.com/",
            "servicePath": "v1/",
            "schemas": {
                "User": {
                    "type": "object",
                    "properties": {
                        "id": {"type": "string"}
                    }
                }
            },
            "resources": {
                "users": {
                    "methods": {
                        "get": {
                            "id": "users.get",
                            "httpMethod": "GET",
                            "path": "users/{+id}",
                            "parameters": {
                                "id": {"type": "string", "location": "path"}
                            },
                            "response": {"$ref": "User"}
                        }
                    }
                }
            }
        })";

  auto rest_res = parse(rest_json);
  assert(rest_res.size() == 1);
  assert(rest_res[0].info.title == "Test Service");
  assert(rest_res[0].servers->front().url == "https://api.test.com/v1/");
  assert(rest_res[0].components->schemas->contains("User"));
  assert(rest_res[0].paths->contains("/users/{id}"));

  std::cout << "google_discovery::test_parse passed.\n";
}
} // namespace cdd_cpp::google_discovery
