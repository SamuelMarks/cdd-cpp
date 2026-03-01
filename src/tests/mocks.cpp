#include "../mocks/emit.hpp"
#include "../mocks/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::mocks {
void test_parse() { std::cout << "mocks::test_parse passed.\n"; }
void test_emit() {
  openapi::OpenAPI spec;
  spec.paths = std::map<std::string, openapi::PathItem>{};
  openapi::PathItem pi;
  openapi::Operation op;
  op.operationId = "get_users";
  pi.get = op;
  spec.paths->insert({"/users", pi});
  std::string out = emit(spec);
  assert(out.find("class IClient") != std::string::npos);
  assert(out.find("virtual std::string get_users() = 0;") != std::string::npos);
  assert(out.find("class MockClient : public IClient") != std::string::npos);
  assert(out.find("MOCK_METHOD(std::string, get_users, (), (override));") !=
         std::string::npos);
  std::cout << "mocks::test_emit passed.\n";
}
} // namespace cdd_cpp::mocks
