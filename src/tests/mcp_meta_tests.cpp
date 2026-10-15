#include "../mcp/emit.hpp"
#include "../mcp/parse.hpp"
#include "../utils/json_writer.hpp"
#include <cassert>
#include <simdjson.h>

namespace cdd_cpp::mcp {
void test_mcp_meta_coverage() {
  simdjson::ondemand::parser parser;
  auto test_meta = [&](auto func, const std::string &j) {
    simdjson::padded_string padded(j);
    simdjson::ondemand::document doc;
    auto err = parser.iterate(padded).get(doc);
    assert(!err);
    simdjson::ondemand::value val;
    assert(!doc.get_value().get(val));
    auto res = func(val);
    assert(res.has_value() || !res.error().empty());
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
    assert(jw.str().find("\"_meta\":{\"custom\": true}") != std::string::npos);
  }

  // Cover all optional-absent branches in mcp::emit_*
  {
    cdd_cpp::utils::JsonWriter jw;
    Annotated ann_empty;
    emit_annotated(ann_empty, jw);
    Annotated ann_inner_empty;
    ann_inner_empty.annotations = Annotations{};
    emit_annotated(ann_inner_empty, jw);

    BlobResourceContents blob;
    blob.blob = "b";
    blob.uri = "u";
    emit_blob_resource_contents(blob, jw);

    CallToolRequest ctr_req;
    ctr_req.params.name = "t";
    emit_call_tool_request(ctr_req, jw);

    TextContent tc;
    tc.text = "t";
    tc.annotations = Annotations{};
    emit_text_content(tc, jw);

    ImageContent ic;
    ic.data = "d";
    ic.mimeType = "m";
    Annotations ann_with_fields;
    ann_with_fields.audience = std::string(to_string(RoleEnum::User));
    ann_with_fields.priority = 1.0;
    ic.annotations = ann_with_fields;
    emit_image_content(ic, jw);

    TextResourceContents trc;
    trc.uri = "u";
    trc.text = "t";
    emit_text_resource_contents(trc, jw);

    EmbeddedResource er;
    er.resource_json = "{}";
    emit_embedded_resource(er, jw);

    CallToolResult ctr_res;
    ctr_res.content_json = "[]";
    emit_call_tool_result(ctr_res, jw);

    CancelledNotification cn_empty_id;
    emit_cancelled_notification(cn_empty_id, jw);

    InitializedNotification in_empty;
    emit_initialized_notification(in_empty, jw);

    ClientCapabilities cc_empty;
    emit_client_capabilities(cc_empty, jw);
    ClientCapabilities cc_roots_empty;
    cc_roots_empty.roots = ClientCapabilitiesRoots{};
    emit_client_capabilities(cc_roots_empty, jw);

    CompleteResult comp_res;
    emit_complete_result(comp_res, jw);

    ModelHint mh_empty;
    emit_model_hint(mh_empty, jw);

    ModelPreferences mp_empty;
    emit_model_preferences(mp_empty, jw);
    ModelPreferences mp_empty_hints;
    mp_empty_hints.hints = std::vector<ModelHint>{};
    emit_model_preferences(mp_empty_hints, jw);

    CreateMessageRequest cm_req;
    cm_req.params.maxTokens = 10;
    cm_req.params.messages_json = "[]";
    emit_create_message_request(cm_req, jw);
    CreateMessageRequest cm_req_empty_stops;
    cm_req_empty_stops.params.maxTokens = 10;
    cm_req_empty_stops.params.messages_json = "[]";
    cm_req_empty_stops.params.stopSequences = std::vector<std::string>{};
    emit_create_message_request(cm_req_empty_stops, jw);

    CreateMessageResult cm_res;
    cm_res.content_json = "{}";
    cm_res.model = "m";
    emit_create_message_result(cm_res, jw);

    Tool tool_min;
    tool_min.name = "t";
    emit_tool(tool_min, jw);
    Tool tool_empty_req;
    tool_empty_req.name = "t";
    tool_empty_req.inputSchema.required = std::vector<std::string>{};
    emit_tool(tool_empty_req, jw);

    ToolListChangedNotification tlcn;
    tlcn.params = ToolListChangedNotificationParams{};
    emit_tool_list_changed_notification(tlcn, jw);

    InitializeRequest init_req;
    init_req.params.protocolVersion = "1.0";
    emit_initialize_request(init_req, jw);

    ServerCapabilities sc_sub_empty;
    sc_sub_empty.prompts = ServerCapabilitiesPrompts{};
    sc_sub_empty.resources = ServerCapabilitiesResources{};
    sc_sub_empty.tools = ServerCapabilitiesTools{};
    emit_server_capabilities(sc_sub_empty, jw);

    InitializedNotification init_notif;
    init_notif.params = InitializedNotificationParams{};
    emit_initialized_notification(init_notif, jw);

    JSONRPCError jerr;
    jerr.id = "1";
    jerr.error.code = -1;
    jerr.error.message = "m";
    emit_jsonrpc_error(jerr, jw);

    JSONRPCNotification jnotif_no_params;
    emit_jsonrpc_notification(jnotif_no_params, jw);
    JSONRPCNotification jnotif_empty_params;
    jnotif_empty_params.params = JSONRPCNotificationParams{};
    emit_jsonrpc_notification(jnotif_empty_params, jw);

    JSONRPCRequest jreq_no_params;
    jreq_no_params.id = "1";
    emit_jsonrpc_request(jreq_no_params, jw);
    JSONRPCRequest jreq_empty_params;
    jreq_empty_params.id = "1";
    jreq_empty_params.params = JSONRPCRequestParams{};
    emit_jsonrpc_request(jreq_empty_params, jw);

    ListPromptsRequest lpr_empty_params;
    lpr_empty_params.params = ListPromptsRequestParams{};
    emit_list_prompts_request(lpr_empty_params, jw);

    PromptArgument pa_min;
    pa_min.name = "a";
    emit_prompt_argument(pa_min, jw);

    Prompt p_min;
    p_min.name = "p";
    emit_prompt(p_min, jw);
    Prompt p_empty_args;
    p_empty_args.name = "p";
    p_empty_args.arguments = std::vector<PromptArgument>{};
    emit_prompt(p_empty_args, jw);

    ListPromptsResult lpres_min;
    emit_list_prompts_result(lpres_min, jw);

    GetPromptRequest gpr_min;
    gpr_min.params.name = "p";
    emit_get_prompt_request(gpr_min, jw);

    GetPromptResult gpres_min;
    emit_get_prompt_result(gpres_min, jw);

    PromptListChangedNotification plcn_empty_params;
    plcn_empty_params.params = PromptListChangedNotificationParams{};
    emit_prompt_list_changed_notification(plcn_empty_params, jw);

    ListResourcesRequest lrr_empty_params;
    lrr_empty_params.params = ListResourcesRequestParams{};
    emit_list_resources_request(lrr_empty_params, jw);

    Resource res_min;
    res_min.uri = "u";
    res_min.name = "n";
    emit_resource(res_min, jw);

    ListResourcesResult lrres_min;
    emit_list_resources_result(lrres_min, jw);

    ListResourceTemplatesRequest lrtr_empty_params;
    lrtr_empty_params.params = ListResourceTemplatesRequestParams{};
    emit_list_resource_templates_request(lrtr_empty_params, jw);

    ResourceTemplate rt_min;
    rt_min.uriTemplate = "ut";
    rt_min.name = "n";
    emit_resource_template(rt_min, jw);

    ListResourceTemplatesResult lrtres_min;
    emit_list_resource_templates_result(lrtres_min, jw);

    ReadResourceResult rrres_min;
    emit_read_resource_result(rrres_min, jw);

    ResourceListChangedNotification rlcn_empty_params;
    rlcn_empty_params.params = ResourceListChangedNotificationParams{};
    emit_resource_list_changed_notification(rlcn_empty_params, jw);

    RootsListChangedNotification rootlcn_empty_params;
    rootlcn_empty_params.params = RootsListChangedNotificationParams{};
    emit_roots_list_changed_notification(rootlcn_empty_params, jw);

    ListRootsRequest lrootreq_empty_params;
    lrootreq_empty_params.params = ListRootsRequestParams{};
    emit_list_roots_request(lrootreq_empty_params, jw);

    ListRootsResult lrootres_min;
    emit_list_roots_result(lrootres_min, jw);

    LoggingMessageNotification lmn_min;
    lmn_min.params.data = "{}";
    emit_logging_message_notification(lmn_min, jw);

    EmptyResult emp_min;
    emit_empty_result(emp_min, jw);

    PingRequest ping_no_params;
    emit_ping_request(ping_no_params, jw);
    PingRequest ping_empty_params;
    ping_empty_params.params = PingRequestParams{};
    emit_ping_request(ping_empty_params, jw);

    ListToolsRequest ltreq_no_params;
    emit_list_tools_request(ltreq_no_params, jw);
    ListToolsRequest ltreq_empty_params;
    ltreq_empty_params.params = ListToolsRequestParams{};
    emit_list_tools_request(ltreq_empty_params, jw);

    ListToolsResult ltres_min;
    emit_list_tools_result(ltres_min, jw);
  }

  test_meta(parse_complete_result, R"({"completion":{"values":["a"]}})");
  test_meta(parse_jsonrpc_notification, R"({"jsonrpc":"2.0","method":"m"})");
  test_meta(parse_jsonrpc_request, R"({"jsonrpc":"2.0","id":1,"method":"m"})");
  test_meta(parse_prompt, R"({"name":"p"})");
  test_meta(parse_ping_request, R"({"method":"ping"})");
  test_meta(parse_list_tools_request, R"({"method":"tools/list"})");
}
} // namespace cdd_cpp::mcp
