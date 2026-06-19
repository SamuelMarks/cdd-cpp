#include "../mcp/emit.hpp"
#include "../mcp/parse.hpp"
#include "../utils/json_writer.hpp"
#include <simdjson.h>

namespace cdd_cpp::mcp {
void test_mcp_meta_coverage() {
  simdjson::ondemand::parser parser;
  auto test_meta = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    parser.iterate(padded).get(doc);
    simdjson::ondemand::value val;
    doc.get_value().get(val);
    func(val);
  };

  test_meta(parse_annotated, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_annotated, R"({"_meta":1})");
  test_meta(parse_blob_resource_contents,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_blob_resource_contents, R"({"_meta":1})");
  test_meta(parse_call_tool_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_call_tool_request, R"({"_meta":1})");
  test_meta(parse_text_content, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_text_content, R"({"_meta":1})");
  test_meta(parse_image_content, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_image_content, R"({"_meta":1})");
  test_meta(parse_text_resource_contents,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_text_resource_contents, R"({"_meta":1})");
  test_meta(parse_embedded_resource, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_embedded_resource, R"({"_meta":1})");
  test_meta(parse_call_tool_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_call_tool_result, R"({"_meta":1})");
  test_meta(parse_cancelled_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_cancelled_notification, R"({"_meta":1})");
  test_meta(parse_client_capabilities,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_client_capabilities, R"({"_meta":1})");
  test_meta(parse_complete_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_complete_request, R"({"_meta":1})");
  test_meta(parse_complete_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_complete_result, R"({"_meta":1})");
  test_meta(parse_model_hint, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_model_hint, R"({"_meta":1})");
  test_meta(parse_model_preferences, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_model_preferences, R"({"_meta":1})");
  test_meta(parse_create_message_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_create_message_request, R"({"_meta":1})");
  test_meta(parse_create_message_result,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_create_message_result, R"({"_meta":1})");
  test_meta(parse_tool, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_tool, R"({"_meta":1})");
  test_meta(parse_tool_list_changed_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_tool_list_changed_notification, R"({"_meta":1})");
  test_meta(parse_paginated_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_paginated_request, R"({"_meta":1})");
  test_meta(parse_paginated_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_paginated_result, R"({"_meta":1})");
  test_meta(parse_implementation, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_implementation, R"({"_meta":1})");
  test_meta(parse_initialize_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_initialize_request, R"({"_meta":1})");
  test_meta(parse_server_capabilities,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_server_capabilities, R"({"_meta":1})");
  test_meta(parse_initialize_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_initialize_result, R"({"_meta":1})");
  test_meta(parse_initialized_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_initialized_notification, R"({"_meta":1})");
  test_meta(parse_jsonrpc_error, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_jsonrpc_error, R"({"_meta":1})");
  test_meta(parse_jsonrpc_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_jsonrpc_notification, R"({"_meta":1})");
  test_meta(parse_jsonrpc_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_jsonrpc_request, R"({"_meta":1})");
  test_meta(parse_jsonrpc_response, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_jsonrpc_response, R"({"_meta":1})");
  test_meta(parse_list_prompts_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_prompts_request, R"({"_meta":1})");
  test_meta(parse_prompt_argument, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_prompt_argument, R"({"_meta":1})");
  test_meta(parse_prompt, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_prompt, R"({"_meta":1})");
  test_meta(parse_list_prompts_result,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_prompts_result, R"({"_meta":1})");
  test_meta(parse_get_prompt_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_get_prompt_request, R"({"_meta":1})");
  test_meta(parse_get_prompt_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_get_prompt_result, R"({"_meta":1})");
  test_meta(parse_prompt_list_changed_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_prompt_list_changed_notification, R"({"_meta":1})");
  test_meta(parse_prompt_reference, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_prompt_reference, R"({"_meta":1})");
  test_meta(parse_list_resources_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_resources_request, R"({"_meta":1})");
  test_meta(parse_resource, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_resource, R"({"_meta":1})");
  test_meta(parse_list_resources_result,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_resources_result, R"({"_meta":1})");
  test_meta(parse_list_resource_templates_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_resource_templates_request, R"({"_meta":1})");
  test_meta(parse_resource_template, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_resource_template, R"({"_meta":1})");
  test_meta(parse_list_resource_templates_result,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_resource_templates_result, R"({"_meta":1})");
  test_meta(parse_read_resource_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_read_resource_request, R"({"_meta":1})");
  test_meta(parse_read_resource_result,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_read_resource_result, R"({"_meta":1})");
  test_meta(parse_resource_updated_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_resource_updated_notification, R"({"_meta":1})");
  test_meta(parse_resource_list_changed_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_resource_list_changed_notification, R"({"_meta":1})");
  test_meta(parse_resource_reference, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_resource_reference, R"({"_meta":1})");
  test_meta(parse_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_result, R"({"_meta":1})");
  test_meta(parse_root, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_root, R"({"_meta":1})");
  test_meta(parse_roots_list_changed_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_roots_list_changed_notification, R"({"_meta":1})");
  test_meta(parse_list_roots_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_roots_request, R"({"_meta":1})");
  test_meta(parse_list_roots_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_roots_result, R"({"_meta":1})");
  test_meta(parse_set_level_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_set_level_request, R"({"_meta":1})");
  test_meta(parse_logging_message_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_logging_message_notification, R"({"_meta":1})");
  test_meta(parse_progress_notification,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_progress_notification, R"({"_meta":1})");
  test_meta(parse_empty_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_empty_result, R"({"_meta":1})");
  test_meta(parse_sampling_message, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_sampling_message, R"({"_meta":1})");
  test_meta(parse_ping_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_ping_request, R"({"_meta":1})");
  test_meta(parse_subscribe_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_subscribe_request, R"({"_meta":1})");
  test_meta(parse_unsubscribe_request,
            R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_unsubscribe_request, R"({"_meta":1})");
  test_meta(parse_list_tools_request, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_tools_result, R"({"method":"m","params":{"_meta":1}})");
  test_meta(parse_list_tools_result, R"({"_meta":1})");

  // Cover emit_initialize_result with _meta
  {
    InitializeResult res;
    res.protocolVersion = "1.0";
    res._meta = R"({"custom": true})";
    cdd_cpp::utils::JsonWriter jw;
    emit_initialize_result(res, jw);
  }
}
} // namespace cdd_cpp::mcp
