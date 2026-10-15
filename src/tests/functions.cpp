/// @file functions.cpp
/// @brief Unit tests for standalone C++ function declarations emitter and
/// parser.

#include "../functions/emit.hpp"
#include "../functions/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::functions {

void test_parse() {
  auto empty_err = parse("");
  assert(!empty_err.has_value());
  assert(empty_err.error().code == utils::ErrorCode::CppAstParseError);

  std::string code = R"(
    /// @title Functions API
    /// @version 1.2.0
    namespace cdd_functions {
      /// @route GET /users/{id}
      /// @summary Fetch user
      std::string get_user(int id, double score, bool active, std::string filter) {
        return "";
      }

      /// @route POST /users
      std::string create_user(std::string body) {
        return "";
      }

      std::string fallback_fn() {
        return "";
      }
    }
  )";

  auto res = parse(code);
  assert(res.has_value());
  assert(res->info.title == "Functions API");
  assert(res->info.version == "1.2.0");
  assert(res->paths.has_value());
  assert(res->paths->contains("/users/{id}"));
  assert(res->paths->at("/users/{id}").get.has_value());
  const auto &get_op = *res->paths->at("/users/{id}").get;
  assert(get_op.operationId == "get_user");
  assert(get_op.summary == "Fetch user");
  assert(get_op.parameters.has_value() && get_op.parameters->size() == 4);
  assert(get_op.parameters->at(0).name == "id");
  assert(get_op.parameters->at(0).in == "path");
  assert(get_op.parameters->at(0).schema->type == "integer");
  assert(get_op.parameters->at(1).schema->type == "number");
  assert(get_op.parameters->at(2).schema->type == "boolean");
  assert(get_op.parameters->at(3).schema->type == "string");

  assert(res->paths->contains("/users"));
  assert(res->paths->at("/users").post.has_value());
  assert(res->paths->at("/users").post->requestBody.has_value());

  assert(res->paths->contains("/fallback_fn"));
  assert(res->paths->at("/fallback_fn").get.has_value());

  std::cout << "functions::test_parse passed.\n";
}

void test_emit() {
  std::string empty_res = emit();
  assert(empty_res == "{}");

  openapi::OpenAPI empty_paths_spec;
  empty_paths_spec.paths = std::map<std::string, openapi::PathItem>{};
  assert(emit(empty_paths_spec) == "{}");

  openapi::OpenAPI spec;
  spec.paths = std::map<std::string, openapi::PathItem>{};
  openapi::PathItem pi;
  openapi::Operation op;
  op.operationId = "update_item";
  openapi::Parameter p1;
  p1.name = "id";
  p1.in = "path";
  p1.schema = openapi::Schema{};
  p1.schema->type = "integer";
  openapi::Parameter p2;
  p2.name = "price";
  p2.in = "query";
  p2.schema = openapi::Schema{};
  p2.schema->type = "number";
  openapi::Parameter p3;
  p3.name = "flag";
  p3.in = "query";
  p3.schema = openapi::Schema{};
  p3.schema->type = "boolean";
  openapi::Parameter p4;
  p4.name = "tag";
  p4.in = "query";
  openapi::Parameter p5;
  p5.name = "untyped";
  p5.in = "query";
  p5.schema = openapi::Schema{};
  openapi::Parameter p6;
  p6.name = "str_typed";
  p6.in = "query";
  p6.schema = openapi::Schema{};
  p6.schema->type = "string";
  op.parameters = std::vector<openapi::Parameter>{p1, p2, p3, p4, p5, p6};
  op.requestBody = openapi::RequestBody{};
  pi.put = op;

  openapi::Operation op_no_params;
  op_no_params.operationId = "OnlyBody";
  op_no_params.requestBody = openapi::RequestBody{};
  pi.post = op_no_params;

  openapi::Operation op_no_body;
  op_no_body.operationId = "NoBody";
  pi.get = op_no_body;

  spec.paths->insert({"/items/{id}", pi});

  std::string code = emit(spec);
  assert(code.find("std::string update_item(int id, double price, bool flag, "
                   "std::string tag, std::string untyped, std::string "
                   "str_typed, std::string body);") != std::string::npos);
  assert(code.find("std::string OnlyBody(std::string body);") !=
         std::string::npos);
  assert(code.find("std::string NoBody();") != std::string::npos);

  auto round_trip = parse(code);
  assert(round_trip.has_value());
  assert(round_trip->paths->contains("/items/{id}"));
  assert(round_trip->paths->at("/items/{id}").put.has_value());
  assert(round_trip->paths->at("/items/{id}").post.has_value());

  std::cout << "functions::test_emit passed.\n";
}

} // namespace cdd_cpp::functions
