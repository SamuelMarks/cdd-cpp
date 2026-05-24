// GCOV_EXCL_BR_START

#include "../openapi/parse.hpp"
#include "../orm/emit.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::orm {
void test_emit() {
  std::string json = R"({
    "openapi": "3.2.0",
    "info": { "title": "Test", "version": "1.0" },
    "components": {
      "schemas": {
        "User": {
          "type": "object",
          "properties": {
            "id": { "type": "integer" },
            "name": { "type": "string" },
            "score": { "type": "number" },
            "active": { "type": "boolean" },
            "custom": { "type": "unknown" }
          }
        },
        "NonObject": {
          "type": "string"
        }
      }
    }
  })";
  auto spec = cdd_cpp::openapi::parse(json);
  assert(spec.has_value());
  std::string code = emit(*spec);

  assert(code.find("struct User") != std::string::npos);
  assert(code.find("UserRepository") != std::string::npos);
  assert(code.find("CREATE TABLE IF NOT EXISTS user") != std::string::npos);
  assert(code.find("INSERT INTO user") != std::string::npos);
  assert(code.find("PQexec") != std::string::npos);
  assert(code.find("PQexecParams") != std::string::npos);

  std::cout << "orm::test_emit passed.\n";
}
} // namespace cdd_cpp::orm
