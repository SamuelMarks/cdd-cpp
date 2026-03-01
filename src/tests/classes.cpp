#include "../classes/emit.hpp"
#include "../classes/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::classes {
void test_parse() {
  std::string client_code = R"(
    class Client {
        std::string getTest() {
            std::string full_url = base_url + "/api/v1/test";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        }
    };
  )";
  auto spec = parse(client_code);
  assert(spec.paths.has_value());
  assert(spec.paths->contains("/api/v1/test"));
  assert(spec.paths->at("/api/v1/test").post.has_value());
  assert(spec.paths->at("/api/v1/test").post->operationId == "getTest");
  std::cout << "classes::test_parse passed.\n";
}

void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test";
  spec.info.version = "1.0";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};

  openapi::Schema s;
  s.type = "object";
  s.description = "Test description";
  s.properties = std::make_shared<std::map<std::string, openapi::Schema>>();

  openapi::Schema p;
  p.type = "string";
  s.properties->insert({"name", p});

  spec.components->schemas->insert({"TestStruct", s});

  std::string code = emit(spec);
  assert(code.find("struct TestStruct") != std::string::npos);
  assert(code.find("std::string name;") != std::string::npos);
  assert(code.find("@description Test description") != std::string::npos);

  std::cout << "classes::test_emit passed.\n";
}
} // namespace cdd_cpp::classes
