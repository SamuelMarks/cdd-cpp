/// @file mcp_parse_fail_tests.cpp
/// @brief Unit tests verifying MCP parser error handling and additional valid
/// edge cases.

#include "../mcp/parse.hpp"
#include <cassert>
#include <iostream>
#include <simdjson.h>

namespace cdd_cpp::mcp {

void test_mcp_parse_failures() {
  std::cout << "test_mcp_parse_failures executed!\n";
  simdjson::ondemand::parser parser;
  auto expect_fail = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    [[maybe_unused]] auto error = parser.iterate(padded).get(doc);
    assert(!error);
    simdjson::ondemand::value val;
    [[maybe_unused]] auto val_err = doc.get_value().get(val);
    assert(!val_err);
    auto res = func(val);
    assert(!res.has_value());
    assert(!res.error().empty());
  };

  auto expect_ok = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    [[maybe_unused]] auto error = parser.iterate(padded).get(doc);
    assert(!error);
    simdjson::ondemand::value val;
    [[maybe_unused]] auto val_err = doc.get_value().get(val);
    assert(!val_err);
    auto res = func(val);
    assert(res.has_value());
    return *res;
  };

  expect_fail(parse_annotated, "[]");
  expect_fail(parse_blob_resource_contents, "{}");
  expect_fail(parse_call_tool_request, "{}");
  expect_fail(parse_text_content, "{}");
  expect_fail(parse_image_content, "{}");
  expect_fail(parse_text_resource_contents, "{}");
  expect_fail(parse_embedded_resource, "{}");
  expect_fail(parse_call_tool_result, "{}");
  expect_fail(parse_cancelled_notification, "{}");
  expect_fail(parse_client_capabilities, "[]");
  expect_fail(parse_complete_request, "{}");
  expect_fail(parse_complete_result, "{}");
  expect_fail(parse_model_hint, "[]");
  expect_fail(parse_model_preferences, "[]");
  expect_fail(parse_create_message_request, "{}");
  expect_fail(parse_create_message_result, "{}");
  expect_fail(parse_tool, "{}");
  expect_fail(parse_tool_list_changed_notification, "{}");
  expect_fail(parse_paginated_request, "{}");
  expect_fail(parse_paginated_result, "[]");
  expect_fail(parse_implementation, "{}");
  expect_fail(parse_initialize_request, "{}");
  expect_fail(parse_server_capabilities, "[]");
  expect_fail(parse_initialize_result, "{}");
  expect_fail(parse_initialized_notification, "{}");
  expect_fail(parse_jsonrpc_error, "{}");
  expect_fail(parse_jsonrpc_notification, "{}");
  expect_fail(parse_jsonrpc_request, "{}");
  expect_fail(parse_jsonrpc_response, "{}");
  expect_fail(parse_list_prompts_request, "{}");
  expect_fail(parse_prompt_argument, "{}");
  expect_fail(parse_prompt, "{}");
  expect_fail(parse_list_prompts_result, "{}");
  expect_fail(parse_get_prompt_request, "{}");
  expect_fail(parse_get_prompt_result, "{}");
  expect_fail(parse_prompt_list_changed_notification, "{}");
  expect_fail(parse_prompt_reference, "{}");
  expect_fail(parse_list_resources_request, "{}");
  expect_fail(parse_resource, "{}");
  expect_fail(parse_list_resources_result, "{}");
  expect_fail(parse_list_resource_templates_request, "{}");
  expect_fail(parse_resource_template, "{}");
  expect_fail(parse_list_resource_templates_result, "{}");
  expect_fail(parse_read_resource_request, "{}");
  expect_fail(parse_read_resource_result, "{}");
  expect_fail(parse_resource_updated_notification, "{}");
  expect_fail(parse_resource_list_changed_notification, "{}");
  expect_fail(parse_resource_reference, "{}");
  expect_fail(parse_result, "[]");
  expect_fail(parse_root, "{}");
  expect_fail(parse_roots_list_changed_notification, "{}");
  expect_fail(parse_list_roots_request, "{}");
  expect_fail(parse_list_roots_result, "{}");
  expect_fail(parse_set_level_request, "{}");
  expect_fail(parse_logging_message_notification, "{}");
  expect_fail(parse_progress_notification, "{}");
  expect_fail(parse_empty_result, "[]");
  expect_fail(parse_sampling_message, "{}");
  expect_fail(parse_ping_request, "{}");
  expect_fail(parse_subscribe_request, "{}");
  expect_fail(parse_unsubscribe_request, "{}");
  expect_fail(parse_list_tools_request, "{}");
  expect_fail(parse_list_tools_result, "{}");

  // Specific error cases:
  expect_fail(parse_sampling_message, "[]");
  expect_fail(parse_sampling_message, R"({"role":"system"})");
  expect_fail(parse_ping_request, "[]");
  expect_fail(parse_model_preferences, R"({"hints":[1]})");
  expect_fail(parse_implementation, R"({"name":"n"})");
  expect_fail(parse_prompt, R"({"name":"p","arguments":[{}]})");
  expect_fail(parse_list_prompts_result, R"({"prompts":[{}]})");
  expect_fail(parse_get_prompt_result, R"({"messages":[1]})");
  expect_fail(parse_get_prompt_result, R"({"messages":[{}]})");
  expect_fail(parse_get_prompt_result, R"({"messages":[{"role":"user"}]})");
  expect_fail(
      parse_progress_notification,
      R"({"method":"m","params":{"progressToken":null,"progress":1.0}})");
  expect_fail(parse_subscribe_request, "[]");
  expect_fail(parse_subscribe_request, R"({"method":"m"})");
  expect_fail(parse_subscribe_request, R"({"method":"m","params":{}})");
  expect_fail(parse_unsubscribe_request, "[]");
  expect_fail(parse_unsubscribe_request, R"({"method":"m"})");
  expect_fail(parse_unsubscribe_request, R"({"method":"m","params":{}})");
  expect_fail(parse_list_tools_request, "[]");
  expect_fail(parse_list_tools_result, "[]");
  expect_fail(parse_list_tools_result, R"({"tools":[{}]})");

  // Valid edge cases with explicit field assertions:
  auto sm =
      expect_ok(parse_sampling_message, R"({"role":"user", "content": {}})");
  assert(sm.role == "user");
  assert(sm.typed_role() == RoleEnum::User);

  // CreateMessageRequest missing maxTokens (error case) and valid with
  // maxTokens
  expect_fail(parse_create_message_request, R"({
      "method":"m",
      "params":{
          "messages":[],
          "includeContext": "thisServer"
      }
  })");
  auto cmr = expect_ok(
      parse_create_message_request,
      R"({"method":"m","params":{"maxTokens":100,"messages":[],"includeContext":"thisServer"}})");
  assert(cmr.params.includeContext.has_value());
  assert(cmr.params.typed_include_context() == IncludeContextEnum::ThisServer);

  auto sub = expect_ok(parse_subscribe_request, R"({
      "method":"m",
      "params":{
          "uri": "some_uri"
      }
  })");
  assert(sub.params.uri == "some_uri");

  auto unsub = expect_ok(parse_unsubscribe_request, R"({
      "method":"m",
      "params":{
          "uri": "some_uri"
      }
  })");
  assert(unsub.params.uri == "some_uri");

  auto ltr = expect_ok(parse_list_tools_request, R"({
      "method":"m",
      "params":{
          "cursor": "cursor"
      }
  })");
  assert(ltr.params.has_value() && ltr.params->cursor == "cursor");

  auto ltres = expect_ok(parse_list_tools_result, R"({
      "tools":[
        {
          "name": "tool",
          "description": "desc",
          "inputSchema": {"type":"object"}
        }
      ],
      "nextCursor": "cursor",
      "_meta": {}
  })");
  assert(ltres.tools.size() == 1);
  assert(ltres.tools[0].name == "tool");
  assert(ltres.nextCursor == "cursor");
}

} // namespace cdd_cpp::mcp
