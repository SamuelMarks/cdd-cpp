/// @file mocks.cpp
/// @brief Unit tests for GoogleMock emitter and parser.

#include "../mocks/emit.hpp"
#include "../mocks/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::mocks {

void test_parse() {
  auto empty_res = parse("");
  assert(!empty_res.has_value());
  assert(empty_res.error().code == utils::ErrorCode::CppAstParseError);

  std::string mock_code = R"(
    /// @title Mock Service
    class MockClient : public IClient {
    public:
      MOCK_METHOD(std::string, get_users, (), (override));
      MOCK_METHOD(std::string, post_users, (), (override));
      MOCK_METHOD(std::string, put_users, (), (override));
      MOCK_METHOD(std::string, delete_users, (), (override));
      MOCK_METHOD(std::string, patch_users, (), (override));
      MOCK_METHOD(std::string, custom_action, (), (override));
    };
  )";
  auto res = parse(mock_code);
  assert(res.has_value());
  assert(res->info.title == "Mock Service");
  assert(res->paths.has_value());
  assert(res->paths->at("/users").get.has_value());
  assert(res->paths->at("/users").post.has_value());
  assert(res->paths->at("/users").put.has_value());
  assert(res->paths->at("/users").delete_op.has_value());
  assert(res->paths->at("/users").patch.has_value());
  assert(res->paths->at("/custom_action").get.has_value());
  std::cout << "mocks::test_parse passed.\n";
}

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

  auto parsed = parse(out);
  assert(parsed.has_value());
  assert(parsed->paths->at("/users").get->operationId == "get_users");

  std::cout << "mocks::test_emit passed.\n";
}

} // namespace cdd_cpp::mocks
