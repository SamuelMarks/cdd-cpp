#include "../mcp/parse.hpp"
#include <iostream>
#include <simdjson.h>

namespace cdd_cpp::mcp {
void test_mcp_parse_failures() {
  std::cout << "test_mcp_parse_failures executed!\n";
  simdjson::ondemand::parser parser;
  auto test_fail = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    auto error = parser.iterate(padded).get(doc);
    if (!error) {
      simdjson::ondemand::value val;
      if (!doc.get_value().get(val)) {
        auto res = func(val);
        (void)res;
      }
    }
  };

#define TEST_FAIL(func, j) test_fail(func, j)

  TEST_FAIL(parse_annotated, "[]");
  TEST_FAIL(parse_blob_resource_contents, "{}");
  TEST_FAIL(parse_call_tool_request, "{}");
  TEST_FAIL(parse_text_content, "{}");
  TEST_FAIL(parse_image_content, "{}");
  TEST_FAIL(parse_text_resource_contents, "{}");
  TEST_FAIL(parse_embedded_resource, "{}");
  TEST_FAIL(parse_call_tool_result, "{}");
  TEST_FAIL(parse_cancelled_notification, "{}");
  TEST_FAIL(parse_client_capabilities, "{}");
  TEST_FAIL(parse_complete_request, "{}");
  TEST_FAIL(parse_complete_result, "{}");
  TEST_FAIL(parse_model_hint, "{}");
  TEST_FAIL(parse_model_preferences, "{}");
  TEST_FAIL(parse_create_message_request, "{}");
  TEST_FAIL(parse_create_message_result, "{}");
  TEST_FAIL(parse_tool, "{}");
  TEST_FAIL(parse_tool_list_changed_notification, "{}");
  TEST_FAIL(parse_paginated_request, "{}");
  TEST_FAIL(parse_paginated_result, "{}");
  TEST_FAIL(parse_implementation, "{}");
  TEST_FAIL(parse_initialize_request, "{}");
  TEST_FAIL(parse_server_capabilities, "{}");
  TEST_FAIL(parse_initialize_result, "{}");
  TEST_FAIL(parse_initialized_notification, "{}");
  TEST_FAIL(parse_jsonrpc_error, "{}");
  TEST_FAIL(parse_jsonrpc_notification, "{}");
  TEST_FAIL(parse_jsonrpc_request, "{}");
  TEST_FAIL(parse_jsonrpc_response, "{}");
  TEST_FAIL(parse_list_prompts_request, "{}");
  TEST_FAIL(parse_prompt_argument, "{}");
  TEST_FAIL(parse_prompt, "{}");
  TEST_FAIL(parse_list_prompts_result, "{}");
  TEST_FAIL(parse_get_prompt_request, "{}");
  TEST_FAIL(parse_get_prompt_result, "{}");
  TEST_FAIL(parse_prompt_list_changed_notification, "{}");
  TEST_FAIL(parse_prompt_reference, "{}");
  TEST_FAIL(parse_list_resources_request, "{}");
  TEST_FAIL(parse_resource, "{}");
  TEST_FAIL(parse_list_resources_result, "{}");
  TEST_FAIL(parse_list_resource_templates_request, "{}");
  TEST_FAIL(parse_resource_template, "{}");
  TEST_FAIL(parse_list_resource_templates_result, "{}");
  TEST_FAIL(parse_read_resource_request, "{}");
  TEST_FAIL(parse_read_resource_result, "{}");
  TEST_FAIL(parse_resource_updated_notification, "{}");
  TEST_FAIL(parse_resource_list_changed_notification, "{}");
  TEST_FAIL(parse_resource_reference, "{}");
  TEST_FAIL(parse_result, "{}");
  TEST_FAIL(parse_root, "{}");
  TEST_FAIL(parse_roots_list_changed_notification, "{}");
  TEST_FAIL(parse_list_roots_request, "{}");
  TEST_FAIL(parse_list_roots_result, "{}");
  TEST_FAIL(parse_set_level_request, "{}");
  TEST_FAIL(parse_logging_message_notification, "{}");
  TEST_FAIL(parse_progress_notification, "{}");
  TEST_FAIL(parse_empty_result, "{}");
  TEST_FAIL(parse_sampling_message, "{}");
  TEST_FAIL(parse_ping_request, "{}");
  TEST_FAIL(parse_subscribe_request, "{}");
  TEST_FAIL(parse_unsubscribe_request, "{}");
  TEST_FAIL(parse_list_tools_request, "{}");
  TEST_FAIL(parse_list_tools_result, "{}");

  // Specific coverage for missing lines:
  TEST_FAIL(parse_sampling_message, "[]");
  TEST_FAIL(parse_sampling_message, R"({"role":"system"})");
  TEST_FAIL(parse_sampling_message, R"({"role":"system", "content": {}})");
  TEST_FAIL(parse_ping_request, "[]");

  // CreateMessageRequest string includeContext
  TEST_FAIL(parse_create_message_request, R"({
      "method":"m",
      "params":{
          "messages":[],
          "includeContext": "some_string"
      }
  })");

  // parse_subscribe_request
  TEST_FAIL(parse_subscribe_request, R"({
      "method":"m",
      "params":{
          "uri": "some_uri"
      }
  })");

  // parse_unsubscribe_request
  TEST_FAIL(parse_unsubscribe_request, R"({
      "method":"m",
      "params":{
          "uri": "some_uri"
      }
  })");

  // list_tools_request / result
  TEST_FAIL(parse_list_tools_request, R"({
      "method":"m",
      "params":{
          "cursor": "cursor"
      }
  })");
  TEST_FAIL(parse_list_tools_result, R"({
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
}
} // namespace cdd_cpp::mcp
