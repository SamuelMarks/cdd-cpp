#include "../routes/emit.hpp"
#include "../routes/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::routes {
void test_parse() {
  std::string input = R"(
    routes["GET /api/v1/test"] = handler;
    routes["POST /api/v1/test"] = handler;
  )";
  auto spec = parse(input);
  assert(spec.paths->contains("/api/v1/test"));
  assert(spec.paths->at("/api/v1/test").get.has_value());
  assert(spec.paths->at("/api/v1/test").post.has_value());
  std::cout << "routes::test_parse passed.\n";
}

void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test";
  spec.info.version = "1.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  openapi::PathItem item;
  openapi::Operation op;
  op.operationId = "testRoute";
  item.get = op;
  spec.paths->insert({"/api/v1/test", item});

  std::string code = emit(spec);
  assert(code.find("on_GET_testRoute") != std::string::npos);

  std::cout << "routes::test_emit passed.\n";
}
} // namespace cdd_cpp::routes
