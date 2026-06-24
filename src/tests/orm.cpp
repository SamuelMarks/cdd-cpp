//

#include "../openapi/parse.hpp"
#include "../orm/emit.hpp"
#include <cassert>
#include <iostream>

//
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
  auto files = emit_modular(*spec, true, false);

  assert(files.count("src/orm/UserDao.hpp"));
  std::string code = files["src/orm/UserDao.hpp"];
  std::string rf_code = files["src/orm/RepositoryFactory.hpp"];

  assert(code.find("struct User") != std::string::npos ||
         code.find("User") != std::string::npos);
  assert(rf_code.find("RepositoryFactory") != std::string::npos);
  assert(code.find("CREATE TABLE IF NOT EXISTS user") != std::string::npos);
  assert(code.find("INSERT INTO user") != std::string::npos);
  assert(code.find("PQexec") != std::string::npos);
  assert(code.find("PQexecParams") != std::string::npos);

  auto files_no_pg = emit_modular(*spec, false, false);
  assert(files_no_pg.count("src/orm/RepositoryFactory.hpp"));
  std::string rf_code_no_pg = files_no_pg["src/orm/RepositoryFactory.hpp"];
  assert(rf_code_no_pg.find("RepositoryFactory() {}") != std::string::npos);

  std::string empty = emit(*spec);
  assert(empty.empty());

  std::cout << "orm::test_emit passed.\n";
}
} // namespace cdd_cpp::orm
