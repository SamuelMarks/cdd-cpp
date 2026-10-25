/// @file mcp_meta_tests.cpp
/// @brief Unit tests for MCP `_meta` parsing and optional-field emission
/// symmetry.

#include "../mcp/emit.hpp"
#include "../mcp/parse.hpp"
#include "../utils/json_writer.hpp"
#include <cassert>
#include <simdjson.h>
#include <string_view>

namespace cdd_cpp::mcp {
void test_mcp_meta_coverage() {
  simdjson::ondemand::parser parser;
  [[maybe_unused]] auto expect_ok = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    [[maybe_unused]] auto err = parser.iterate(padded).get(doc);
    assert(!err);
    simdjson::ondemand::value val;
    [[maybe_unused]] auto val_err = doc.get_value().get(val);
    assert(!val_err);
    auto res = func(val);
    assert(res.has_value());
    return *res;
  };
  auto expect_err = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    [[maybe_unused]] auto err = parser.iterate(padded).get(doc);
    assert(!err);
    simdjson::ondemand::value val;
    [[maybe_unused]] auto val_err = doc.get_value().get(val);
    assert(!val_err);
    auto res = func(val);
    assert(!res.has_value());
    assert(!res.error().empty());
  };

  assert(!expect_ok(parse_annotated, R"({"method":"m","params":{"_meta":1}})")
              .annotations.has_value());
  assert(!expect_ok(parse_annotated, R"({"_meta":1})").annotations.has_value());
  expect_err(parse_blob_resource_contents,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_blob_resource_contents, R"({"_meta":1})");
  expect_err(parse_call_tool_request, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_call_tool_request, R"({"_meta":1})");
  expect_err(parse_text_content, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_text_content, R"({"_meta":1})");
  expect_err(parse_image_content, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_image_content, R"({"_meta":1})");
  expect_err(parse_text_resource_contents,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_text_resource_contents, R"({"_meta":1})");
  expect_err(parse_embedded_resource, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_embedded_resource, R"({"_meta":1})");
  expect_err(parse_call_tool_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_call_tool_result, R"({"_meta":1})");
  expect_err(parse_cancelled_notification,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_cancelled_notification, R"({"_meta":1})");
  assert(!expect_ok(parse_client_capabilities,
                    R"({"method":"m","params":{"_meta":1}})")
              .roots.has_value());
  assert(!expect_ok(parse_client_capabilities, R"({"_meta":1})")
              .roots.has_value());
  expect_err(parse_complete_request, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_complete_request, R"({"_meta":1})");
  expect_err(parse_complete_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_complete_result, R"({"_meta":1})");
  assert(!expect_ok(parse_model_hint, R"({"method":"m","params":{"_meta":1}})")
              .name.has_value());
  assert(!expect_ok(parse_model_hint, R"({"_meta":1})").name.has_value());
  assert(!expect_ok(parse_model_preferences,
                    R"({"method":"m","params":{"_meta":1}})")
              .hints.has_value());
  assert(
      !expect_ok(parse_model_preferences, R"({"_meta":1})").hints.has_value());
  expect_err(parse_create_message_request,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_create_message_request, R"({"_meta":1})");
  expect_err(parse_create_message_result,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_create_message_result, R"({"_meta":1})");
  expect_err(parse_tool, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_tool, R"({"_meta":1})");
  assert(expect_ok(parse_tool_list_changed_notification,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_tool_list_changed_notification, R"({"_meta":1})");
  assert(expect_ok(parse_paginated_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .method == "m");
  expect_err(parse_paginated_request, R"({"_meta":1})");
  assert(!expect_ok(parse_paginated_result,
                    R"({"method":"m","params":{"_meta":1}})")
              ._meta.has_value());
  assert(expect_ok(parse_paginated_result, R"({"_meta":1})")._meta == "1");
  expect_err(parse_implementation, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_implementation, R"({"_meta":1})");
  expect_err(parse_initialize_request,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_initialize_request, R"({"_meta":1})");
  assert(!expect_ok(parse_server_capabilities,
                    R"({"method":"m","params":{"_meta":1}})")
              .tools.has_value());
  assert(!expect_ok(parse_server_capabilities, R"({"_meta":1})")
              .tools.has_value());
  expect_err(parse_initialize_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_initialize_result, R"({"_meta":1})");
  assert(expect_ok(parse_initialized_notification,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_initialized_notification, R"({"_meta":1})");
  expect_err(parse_jsonrpc_error, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_jsonrpc_error, R"({"_meta":1})");
  expect_err(parse_jsonrpc_notification,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_jsonrpc_notification, R"({"_meta":1})");
  expect_err(parse_jsonrpc_request, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_jsonrpc_request, R"({"_meta":1})");
  expect_err(parse_jsonrpc_response, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_jsonrpc_response, R"({"_meta":1})");
  assert(expect_ok(parse_list_prompts_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .method == "m");
  expect_err(parse_list_prompts_request, R"({"_meta":1})");
  expect_err(parse_prompt_argument, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_prompt_argument, R"({"_meta":1})");
  expect_err(parse_prompt, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_prompt, R"({"_meta":1})");
  expect_err(parse_list_prompts_result,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_list_prompts_result, R"({"_meta":1})");
  expect_err(parse_get_prompt_request,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_get_prompt_request, R"({"_meta":1})");
  expect_err(parse_get_prompt_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_get_prompt_result, R"({"_meta":1})");
  assert(expect_ok(parse_prompt_list_changed_notification,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_prompt_list_changed_notification, R"({"_meta":1})");
  expect_err(parse_prompt_reference, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_prompt_reference, R"({"_meta":1})");
  assert(expect_ok(parse_list_resources_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .method == "m");
  expect_err(parse_list_resources_request, R"({"_meta":1})");
  expect_err(parse_resource, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_resource, R"({"_meta":1})");
  expect_err(parse_list_resources_result,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_list_resources_result, R"({"_meta":1})");
  assert(expect_ok(parse_list_resource_templates_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .method == "m");
  expect_err(parse_list_resource_templates_request, R"({"_meta":1})");
  expect_err(parse_resource_template, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_resource_template, R"({"_meta":1})");
  expect_err(parse_list_resource_templates_result,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_list_resource_templates_result, R"({"_meta":1})");
  expect_err(parse_read_resource_request,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_read_resource_request, R"({"_meta":1})");
  expect_err(parse_read_resource_result,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_read_resource_result, R"({"_meta":1})");
  expect_err(parse_resource_updated_notification,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_resource_updated_notification, R"({"_meta":1})");
  assert(expect_ok(parse_resource_list_changed_notification,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_resource_list_changed_notification, R"({"_meta":1})");
  expect_err(parse_resource_reference,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_resource_reference, R"({"_meta":1})");
  assert(!expect_ok(parse_result, R"({"method":"m","params":{"_meta":1}})")
              ._meta.has_value());
  assert(expect_ok(parse_result, R"({"_meta":1})")._meta == "1");
  expect_err(parse_root, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_root, R"({"_meta":1})");
  assert(expect_ok(parse_roots_list_changed_notification,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_roots_list_changed_notification, R"({"_meta":1})");
  assert(expect_ok(parse_list_roots_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_list_roots_request, R"({"_meta":1})");
  expect_err(parse_list_roots_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_list_roots_result, R"({"_meta":1})");
  expect_err(parse_set_level_request, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_set_level_request, R"({"_meta":1})");
  expect_err(parse_logging_message_notification,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_logging_message_notification, R"({"_meta":1})");
  expect_err(parse_progress_notification,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_progress_notification, R"({"_meta":1})");
  assert(
      !expect_ok(parse_empty_result, R"({"method":"m","params":{"_meta":1}})")
           ._meta.has_value());
  assert(expect_ok(parse_empty_result, R"({"_meta":1})")._meta == "1");
  expect_err(parse_sampling_message, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_sampling_message, R"({"_meta":1})");
  assert(expect_ok(parse_ping_request, R"({"method":"m","params":{"_meta":1}})")
             .params->_meta == "1");
  expect_err(parse_ping_request, R"({"_meta":1})");
  expect_err(parse_subscribe_request, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_subscribe_request, R"({"_meta":1})");
  expect_err(parse_unsubscribe_request,
             R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_unsubscribe_request, R"({"_meta":1})");
  assert(expect_ok(parse_list_tools_request,
                   R"({"method":"m","params":{"_meta":1}})")
             .method == "m");
  expect_err(parse_list_tools_result, R"({"method":"m","params":{"_meta":1}})");
  expect_err(parse_list_tools_result, R"({"_meta":1})");

  // Cover emit_initialize_result with _meta
  {
    InitializeResult res;
    res.protocolVersion = "1.0";
    res._meta = R"({"custom": true})";
    cdd_cpp::utils::JsonWriter jw;
    emit_initialize_result(res, jw);
    assert(jw.str().find(R"("_meta":{"custom": true})") != std::string::npos);
  }

  auto check_emit = [](auto emit_fn, const auto &model,
                       [[maybe_unused]] std::string_view expected_substr) {
    cdd_cpp::utils::JsonWriter jw;
    emit_fn(model, jw);
    [[maybe_unused]] std::string out = jw.str();
    assert(!out.empty() && out.front() == '{' && out.back() == '}');
    assert(out.find(expected_substr) != std::string::npos);
  };

  // Cover all optional-absent branches in mcp::emit_* with isolated assertions
  {
    Annotated ann_empty;
    check_emit(emit_annotated, ann_empty, "{}");
    Annotated ann_inner_empty;
    ann_inner_empty.annotations = Annotations{};
    check_emit(emit_annotated, ann_inner_empty, R"("annotations":{})");

    BlobResourceContents blob;
    blob.blob = "b";
    blob.uri = "u";
    check_emit(emit_blob_resource_contents, blob, R"("blob":"b")");

    CallToolRequest ctr_req;
    ctr_req.params.name = "t";
    check_emit(emit_call_tool_request, ctr_req, R"("name":"t")");

    TextContent tc;
    tc.text = "t";
    tc.annotations = Annotations{};
    check_emit(emit_text_content, tc, R"("text":"t")");

    ImageContent ic;
    ic.data = "d";
    ic.mimeType = "m";
    Annotations ann_with_fields;
    ann_with_fields.audience = std::string(to_string(RoleEnum::User));
    ann_with_fields.priority = 1.0;
    ic.annotations = ann_with_fields;
    check_emit(emit_image_content, ic, R"("audience":"user")");

    TextResourceContents trc;
    trc.uri = "u";
    trc.text = "t";
    check_emit(emit_text_resource_contents, trc, R"("text":"t")");

    EmbeddedResource er;
    er.resource_json = "{}";
    check_emit(emit_embedded_resource, er, R"("resource":{})");

    CallToolResult ctr_res;
    ctr_res.content_json = "[]";
    check_emit(emit_call_tool_result, ctr_res, R"("content":[])");

    CancelledNotification cn_empty_id;
    check_emit(emit_cancelled_notification, cn_empty_id, R"("requestId":"")");

    InitializedNotification in_empty;
    check_emit(emit_initialized_notification, in_empty, R"("method":)");

    ClientCapabilities cc_empty;
    check_emit(emit_client_capabilities, cc_empty, "{}");
    ClientCapabilities cc_roots_empty;
    cc_roots_empty.roots = ClientCapabilitiesRoots{};
    check_emit(emit_client_capabilities, cc_roots_empty, R"("roots":{})");

    CompleteResult comp_res;
    check_emit(emit_complete_result, comp_res, R"("values":[])");

    ModelHint mh_empty;
    check_emit(emit_model_hint, mh_empty, "{}");

    ModelPreferences mp_empty;
    check_emit(emit_model_preferences, mp_empty, "{}");
    ModelPreferences mp_empty_hints;
    mp_empty_hints.hints = std::vector<ModelHint>{};
    check_emit(emit_model_preferences, mp_empty_hints, R"("hints":[])");

    CreateMessageRequest cm_req;
    cm_req.params.maxTokens = 10;
    cm_req.params.messages_json = "[]";
    check_emit(emit_create_message_request, cm_req, R"("maxTokens":10)");
    CreateMessageRequest cm_req_empty_stops;
    cm_req_empty_stops.params.maxTokens = 10;
    cm_req_empty_stops.params.messages_json = "[]";
    cm_req_empty_stops.params.stopSequences = std::vector<std::string>{};
    check_emit(emit_create_message_request, cm_req_empty_stops,
               R"("stopSequences":[])");

    CreateMessageResult cm_res;
    cm_res.content_json = "{}";
    cm_res.model = "m";
    check_emit(emit_create_message_result, cm_res, R"("model":"m")");

    Tool tool_min;
    tool_min.name = "t";
    check_emit(emit_tool, tool_min, R"("name":"t")");
    Tool tool_empty_req;
    tool_empty_req.name = "t";
    tool_empty_req.inputSchema.required = std::vector<std::string>{};
    check_emit(emit_tool, tool_empty_req, R"("required":[])");

    ToolListChangedNotification tlcn;
    tlcn.params = ToolListChangedNotificationParams{};
    check_emit(emit_tool_list_changed_notification, tlcn, R"("params":{})");

    InitializeRequest init_req;
    init_req.params.protocolVersion = "1.0";
    check_emit(emit_initialize_request, init_req, R"("protocolVersion":"1.0")");

    ServerCapabilities sc_sub_empty;
    sc_sub_empty.prompts = ServerCapabilitiesPrompts{};
    sc_sub_empty.resources = ServerCapabilitiesResources{};
    sc_sub_empty.tools = ServerCapabilitiesTools{};
    check_emit(emit_server_capabilities, sc_sub_empty, R"("tools":{})");

    InitializedNotification init_notif;
    init_notif.params = InitializedNotificationParams{};
    check_emit(emit_initialized_notification, init_notif, R"("params":{})");

    JSONRPCError jerr;
    jerr.id = "1";
    jerr.error.code = -1;
    jerr.error.message = "m";
    check_emit(emit_jsonrpc_error, jerr, R"("message":"m")");

    JSONRPCNotification jnotif_no_params;
    check_emit(emit_jsonrpc_notification, jnotif_no_params, R"("jsonrpc":)");
    JSONRPCNotification jnotif_empty_params;
    jnotif_empty_params.params = JSONRPCNotificationParams{};
    check_emit(emit_jsonrpc_notification, jnotif_empty_params,
               R"("params":{})");

    JSONRPCRequest jreq_no_params;
    jreq_no_params.id = "1";
    check_emit(emit_jsonrpc_request, jreq_no_params, R"("id":1)");
    JSONRPCRequest jreq_empty_params;
    jreq_empty_params.id = "1";
    jreq_empty_params.params = JSONRPCRequestParams{};
    check_emit(emit_jsonrpc_request, jreq_empty_params, R"("params":{})");

    ListPromptsRequest lpr_empty_params;
    lpr_empty_params.params = ListPromptsRequestParams{};
    check_emit(emit_list_prompts_request, lpr_empty_params, R"("params":{})");

    PromptArgument pa_min;
    pa_min.name = "a";
    check_emit(emit_prompt_argument, pa_min, R"("name":"a")");

    Prompt p_min;
    p_min.name = "p";
    check_emit(emit_prompt, p_min, R"("name":"p")");
    Prompt p_empty_args;
    p_empty_args.name = "p";
    p_empty_args.arguments = std::vector<PromptArgument>{};
    check_emit(emit_prompt, p_empty_args, R"("arguments":[])");

    ListPromptsResult lpres_min;
    check_emit(emit_list_prompts_result, lpres_min, R"("prompts":[])");

    GetPromptRequest gpr_min;
    gpr_min.params.name = "p";
    check_emit(emit_get_prompt_request, gpr_min, R"("name":"p")");

    GetPromptResult gpres_min;
    check_emit(emit_get_prompt_result, gpres_min, R"("messages":[])");

    PromptListChangedNotification plcn_empty_params;
    plcn_empty_params.params = PromptListChangedNotificationParams{};
    check_emit(emit_prompt_list_changed_notification, plcn_empty_params,
               R"("params":{})");

    ListResourcesRequest lrr_empty_params;
    lrr_empty_params.params = ListResourcesRequestParams{};
    check_emit(emit_list_resources_request, lrr_empty_params, R"("params":{})");

    Resource res_min;
    res_min.uri = "u";
    res_min.name = "n";
    check_emit(emit_resource, res_min, R"("uri":"u")");

    ListResourcesResult lrres_min;
    check_emit(emit_list_resources_result, lrres_min, R"("resources":[])");

    ListResourceTemplatesRequest lrtr_empty_params;
    lrtr_empty_params.params = ListResourceTemplatesRequestParams{};
    check_emit(emit_list_resource_templates_request, lrtr_empty_params,
               R"("params":{})");

    ResourceTemplate rt_min;
    rt_min.uriTemplate = "ut";
    rt_min.name = "n";
    check_emit(emit_resource_template, rt_min, R"("uriTemplate":"ut")");

    ListResourceTemplatesResult lrtres_min;
    check_emit(emit_list_resource_templates_result, lrtres_min,
               R"("resourceTemplates":[])");

    ReadResourceResult rrres_min;
    check_emit(emit_read_resource_result, rrres_min, R"("contents":)");

    ResourceListChangedNotification rlcn_empty_params;
    rlcn_empty_params.params = ResourceListChangedNotificationParams{};
    check_emit(emit_resource_list_changed_notification, rlcn_empty_params,
               R"("params":{})");

    RootsListChangedNotification rootlcn_empty_params;
    rootlcn_empty_params.params = RootsListChangedNotificationParams{};
    check_emit(emit_roots_list_changed_notification, rootlcn_empty_params,
               R"("params":{})");

    ListRootsRequest lrootreq_empty_params;
    lrootreq_empty_params.params = ListRootsRequestParams{};
    check_emit(emit_list_roots_request, lrootreq_empty_params,
               R"("params":{})");

    ListRootsResult lrootres_min;
    check_emit(emit_list_roots_result, lrootres_min, R"("roots":[])");

    LoggingMessageNotification lmn_min;
    lmn_min.params.data = "{}";
    check_emit(emit_logging_message_notification, lmn_min, R"("data":{})");

    EmptyResult emp_min;
    check_emit(emit_empty_result, emp_min, "{}");

    PingRequest ping_no_params;
    check_emit(emit_ping_request, ping_no_params, R"("method":)");
    PingRequest ping_empty_params;
    ping_empty_params.params = PingRequestParams{};
    check_emit(emit_ping_request, ping_empty_params, R"("params":{})");

    ListToolsRequest ltreq_no_params;
    check_emit(emit_list_tools_request, ltreq_no_params, R"("method":)");
    ListToolsRequest ltreq_empty_params;
    ltreq_empty_params.params = ListToolsRequestParams{};
    check_emit(emit_list_tools_request, ltreq_empty_params, R"("params":{})");

    ListToolsResult ltres_min;
    check_emit(emit_list_tools_result, ltres_min, R"("tools":[])");
  }

  assert(expect_ok(parse_complete_result, R"({"completion":{"values":["a"]}})")
             .completion.values.size() == 1);
  assert(
      expect_ok(parse_jsonrpc_notification, R"({"jsonrpc":"2.0","method":"m"})")
          .method == "m");
  assert(expect_ok(parse_jsonrpc_request,
                   R"({"jsonrpc":"2.0","id":1,"method":"m"})")
             .id == "1");
  assert(expect_ok(parse_prompt, R"({"name":"p"})").name == "p");
  assert(expect_ok(parse_ping_request, R"({"method":"ping"})").method ==
         "ping");
  assert(expect_ok(parse_list_tools_request, R"({"method":"tools/list"})")
             .method == "tools/list");
}
} // namespace cdd_cpp::mcp
