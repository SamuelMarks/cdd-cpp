// GCOV_EXCL_BR_START
#include "../mcp/emit.hpp"
#include "../mcp/parse.hpp"
#include <cassert>
#include <iostream>

using namespace cdd_cpp;

void test_mcp_annotated() {
  std::string json =
      R"({"annotations": {"audience": "users", "priority": 1.5}})";
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json);
  simdjson::ondemand::document doc = parser.iterate(padded);
  simdjson::ondemand::value val = doc.get_value();

  auto result = mcp::parse_annotated(val);
  assert(result.has_value());
  assert(result->annotations.has_value());
  assert(result->annotations->audience == "users");
  assert(result->annotations->priority == 1.5);

  utils::JsonWriter jw;
  mcp::emit_annotated(result.operator*(), jw);
  assert(jw.str() == R"({"annotations":{"audience":"users","priority":1.5}})");

  std::cout << "mcp::test_mcp_annotated passed.\n";
}

void test_mcp_blob_resource_contents() {
  std::string json =
      R"({"blob": "base64data", "mimeType": "image/png", "uri": "file:///test.png"})";
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json);
  simdjson::ondemand::document doc = parser.iterate(padded);
  simdjson::ondemand::value val = doc.get_value();

  auto result = mcp::parse_blob_resource_contents(val);
  assert(result.has_value());
  assert(result->blob == "base64data");
  assert(result->mimeType == "image/png");
  assert(result->uri == "file:///test.png");

  utils::JsonWriter jw;
  mcp::emit_blob_resource_contents(result.operator*(), jw);
  assert(
      jw.str() ==
      R"({"blob":"base64data","mimeType":"image/png","uri":"file:///test.png"})");

  // test failures
  std::string json_no_blob = R"({"uri": "file:///test.png"})";
  simdjson::padded_string padded_no_blob(json_no_blob);
  doc = parser.iterate(padded_no_blob);
  val = doc.get_value();
  auto result_no_blob = mcp::parse_blob_resource_contents(val);
  assert(!result_no_blob.has_value());
  assert(result_no_blob.error() == "Missing required field: blob");

  std::string json_no_uri = R"({"blob": "base64data"})";
  simdjson::padded_string padded_no_uri(json_no_uri);
  doc = parser.iterate(padded_no_uri);
  val = doc.get_value();
  auto result_no_uri = mcp::parse_blob_resource_contents(val);
  assert(!result_no_uri.has_value());
  assert(result_no_uri.error() == "Missing required field: uri");

  std::string json_not_obj = R"(["string"])";
  simdjson::padded_string padded_not_obj(json_not_obj);
  doc = parser.iterate(padded_not_obj);
  val = doc.get_value();
  auto result_not_obj = mcp::parse_blob_resource_contents(val);
  assert(!result_not_obj.has_value());
  assert(result_not_obj.error() == "Expected object for BlobResourceContents");

  std::cout << "mcp::test_mcp_blob_resource_contents passed.\n";
}

void test_mcp_call_tool_request() {
  std::string json =
      R"({"method": "tools/call", "params": {"name": "my_tool", "arguments": {"arg1": "val1"}}})";
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json);
  simdjson::ondemand::document doc = parser.iterate(padded);
  simdjson::ondemand::value val = doc.get_value();

  auto result = mcp::parse_call_tool_request(val);
  assert(result.has_value());
  assert(result->method == "tools/call");
  assert(result->params.name == "my_tool");
  assert(result->params.arguments.has_value());
  // simdjson raw_json might include whitespace
  assert(result->params.arguments.operator*().find(R"({"arg1": "val1"})") !=
         std::string::npos);

  utils::JsonWriter jw;
  mcp::emit_call_tool_request(result.operator*(), jw);
  assert(
      jw.str() ==
      R"({"method":"tools/call","params":{"name":"my_tool","arguments":{"arg1": "val1"}}})");

  // test failures
  std::string json_no_method = R"({"params": {"name": "my_tool"}})";
  simdjson::padded_string padded_no_method(json_no_method);
  doc = parser.iterate(padded_no_method);
  val = doc.get_value();
  auto result_no_method = mcp::parse_call_tool_request(val);
  assert(!result_no_method.has_value());
  assert(result_no_method.error() == "Missing required field: method");

  std::string json_no_params = R"({"method": "tools/call"})";
  simdjson::padded_string padded_no_params(json_no_params);
  doc = parser.iterate(padded_no_params);
  val = doc.get_value();
  auto result_no_params = mcp::parse_call_tool_request(val);
  assert(!result_no_params.has_value());
  assert(result_no_params.error() == "Missing required field: params");

  std::string json_no_name = R"({"method": "tools/call", "params": {}})";
  simdjson::padded_string padded_no_name(json_no_name);
  doc = parser.iterate(padded_no_name);
  val = doc.get_value();
  auto result_no_name = mcp::parse_call_tool_request(val);
  assert(!result_no_name.has_value());
  assert(result_no_name.error() == "Missing required field: params.name");

  std::string json_not_obj = R"(["string"])";
  simdjson::padded_string padded_not_obj(json_not_obj);
  doc = parser.iterate(padded_not_obj);
  val = doc.get_value();
  auto result_not_obj = mcp::parse_call_tool_request(val);
  assert(!result_not_obj.has_value());
  assert(result_not_obj.error() == "Expected object for CallToolRequest");

  std::cout << "mcp::test_mcp_call_tool_request passed.\n";
}

void test_mcp_content_and_results() {
  simdjson::ondemand::parser parser;

  // TextContent
  {
    std::string json =
        R"({"type":"text","text":"hello","annotations":{"audience":"user"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_text_content(val);
    assert(res.has_value() && res->text == "hello");
    utils::JsonWriter jw;
    mcp::emit_text_content(res.operator*(), jw);
    assert(
        jw.str() ==
        R"({"type":"text","text":"hello","annotations":{"audience":"user"}})");
  }

  // ImageContent
  {
    std::string json =
        R"({"type":"image","data":"b64","mimeType":"image/png"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_image_content(val);
    assert(res.has_value() && res->data == "b64");
    utils::JsonWriter jw;
    mcp::emit_image_content(res.operator*(), jw);
    assert(jw.str() ==
           R"({"type":"image","data":"b64","mimeType":"image/png"})");
  }

  // TextResourceContents
  {
    std::string json =
        R"({"uri":"file:///a.txt","mimeType":"text/plain","text":"file content"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_text_resource_contents(val);
    assert(res.has_value() && res->text == "file content");
    utils::JsonWriter jw;
    mcp::emit_text_resource_contents(res.operator*(), jw);
    assert(
        jw.str() ==
        R"({"uri":"file:///a.txt","mimeType":"text/plain","text":"file content"})");
  }

  // EmbeddedResource
  {
    std::string json =
        R"({"type":"resource","resource":{"uri":"file:///a.txt","text":"hi"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_embedded_resource(val);
    assert(res.has_value() && res->type == "resource");
    utils::JsonWriter jw;
    mcp::emit_embedded_resource(res.operator*(), jw);
    assert(jw.str().find("file:///a.txt") != std::string::npos);
  }

  // CallToolResult
  {
    std::string json =
        R"({"_meta":{"progressToken":"123"},"content":[{"type":"text","text":"hi"}],"isError":true})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_call_tool_result(val);
    assert(res.has_value() && res->isError.operator*() == true);
    utils::JsonWriter jw;
    mcp::emit_call_tool_result(res.operator*(), jw);
    assert(jw.str().find("progressToken") != std::string::npos);
  }

  // CallToolResult Missing Errors
  {
    std::string json_no_content = R"({"isError":true})";
    simdjson::padded_string padded(json_no_content);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_call_tool_result(val);
    assert(!res.has_value());
  }

  // Test parsing error paths
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };
  test_err(mcp::parse_text_content, R"([])");
  test_err(mcp::parse_text_content, R"({"text":"hi"})");
  test_err(mcp::parse_text_content, R"({"type":"text"})");

  test_err(mcp::parse_image_content, R"([])");
  test_err(mcp::parse_image_content, R"({"data":"b64","mimeType":"x"})");
  test_err(mcp::parse_image_content, R"({"type":"image","mimeType":"x"})");
  test_err(mcp::parse_image_content, R"({"type":"image","data":"b64"})");

  test_err(mcp::parse_text_resource_contents, R"([])");
  test_err(mcp::parse_text_resource_contents, R"({"text":"hi"})");
  test_err(mcp::parse_text_resource_contents, R"({"uri":"f"})");

  test_err(mcp::parse_embedded_resource, R"([])");
  test_err(mcp::parse_embedded_resource, R"({"resource":{}})");
  test_err(mcp::parse_embedded_resource, R"({"type":"resource"})");

  test_err(mcp::parse_call_tool_result, R"([])");

  // CancelledNotification
  {
    std::string json =
        R"({"method":"notifications/cancelled","params":{"requestId":123,"reason":"timeout"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_cancelled_notification(val);
    assert(res.has_value() && res->params.requestId == "123");
    utils::JsonWriter jw;
    mcp::emit_cancelled_notification(res.operator*(), jw);
    assert(jw.str().find("123") != std::string::npos);
  }

  {
    std::string json =
        R"({"method":"notifications/cancelled","params":{"requestId":"abc"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_cancelled_notification(val);
    assert(res.has_value() && res->params.requestId == "abc");
    utils::JsonWriter jw;
    mcp::emit_cancelled_notification(res.operator*(), jw);
    assert(jw.str().find("\"abc\"") != std::string::npos);
  }

  test_err(mcp::parse_cancelled_notification, R"([])");
  test_err(mcp::parse_cancelled_notification, R"({"params":{}})");
  test_err(mcp::parse_cancelled_notification, R"({"method":"m"})");
  test_err(mcp::parse_cancelled_notification, R"({"method":"m","params":{}})");

  // ClientCapabilities
  {
    std::string json =
        R"({"experimental":{"feature":true},"roots":{"listChanged":true},"sampling":{}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_client_capabilities(val);
    assert(res.has_value());
    assert(res->experimental.has_value() &&
           res->experimental.operator*().find("feature") != std::string::npos);
    assert(res->roots.has_value() &&
           res->roots->listChanged.operator*() == true);
    assert(res->sampling.has_value() &&
           res->sampling.operator*().find("{}") != std::string::npos);

    utils::JsonWriter jw;
    mcp::emit_client_capabilities(res.operator*(), jw);
    assert(jw.str().find("listChanged") != std::string::npos);
  }
  test_err(mcp::parse_client_capabilities, R"([])");

  std::cout << "mcp::test_mcp_content_and_results passed.\n";
}

void test_mcp_completion() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // CompleteRequest
  {
    std::string json =
        R"({"method":"completion/complete","params":{"argument":{"name":"arg1","value":"val1"},"ref":{"type":"ref"}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_complete_request(val);
    assert(res.has_value());
    assert(res->method == "completion/complete");
    assert(res->params.argument.name == "arg1");
    assert(res->params.argument.value == "val1");
    assert(res->params.ref_json.find("ref") != std::string::npos);

    utils::JsonWriter jw;
    mcp::emit_complete_request(res.operator*(), jw);
    assert(jw.str().find("arg1") != std::string::npos);
  }

  test_err(mcp::parse_complete_request, R"([])");
  test_err(mcp::parse_complete_request, R"({"params":{}})");
  test_err(mcp::parse_complete_request, R"({"method":"m"})");
  test_err(mcp::parse_complete_request, R"({"method":"m","params":{}})");
  test_err(mcp::parse_complete_request,
           R"({"method":"m","params":{"argument":{}}})");
  test_err(mcp::parse_complete_request,
           R"({"method":"m","params":{"argument":{"name":"n"}}})");
  test_err(mcp::parse_complete_request,
           R"({"method":"m","params":{"argument":{"name":"n","value":"v"}}})");

  // CompleteResult
  {
    std::string json =
        R"({"_meta":{"progressToken":"1"},"completion":{"values":["a","b"],"total":2,"hasMore":false}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_complete_result(val);
    assert(res.has_value());
    assert(res->completion.values.size() == 2);
    assert(res->completion.total.operator*() == 2);
    assert(res->completion.hasMore.operator*() == false);

    utils::JsonWriter jw;
    mcp::emit_complete_result(res.operator*(), jw);
    assert(jw.str().find("hasMore") != std::string::npos);
  }
  test_err(mcp::parse_complete_result, R"([])");
  test_err(mcp::parse_complete_result, R"({"_meta":{}})");
  test_err(mcp::parse_complete_result, R"({"completion":{}})");

  std::cout << "mcp::test_mcp_completion passed.\n";
}

void test_mcp_messaging() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // CreateMessageRequest
  {
    std::string json =
        R"({"method":"sampling/createMessage","params":{"maxTokens":100,"messages":[{"role":"user","content":{"type":"text","text":"hi"}}],"includeContext":"none","metadata":{},"modelPreferences":{"costPriority":0.5,"hints":[{"name":"claude"}]},"stopSequences":["stop"],"systemPrompt":"sys","temperature":0.8}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_create_message_request(val);
    assert(res.has_value());
    assert(res->params.maxTokens == 100);
    assert(res->params.messages_json.find("role") != std::string::npos);
    assert(res->params.modelPreferences.has_value());
    assert(res->params.modelPreferences->hints->size() == 1);
    assert(res->params.stopSequences->size() == 1);

    utils::JsonWriter jw;
    mcp::emit_create_message_request(res.operator*(), jw);
    assert(jw.str().find("sampling/createMessage") != std::string::npos);
    assert(jw.str().find("claude") != std::string::npos);
  }
  test_err(mcp::parse_create_message_request, R"([])");
  test_err(mcp::parse_create_message_request, R"({"params":{}})");
  test_err(mcp::parse_create_message_request, R"({"method":"m"})");
  test_err(mcp::parse_create_message_request, R"({"method":"m","params":{}})");
  test_err(mcp::parse_create_message_request,
           R"({"method":"m","params":{"maxTokens":1}})");

  // CreateMessageResult
  {
    std::string json =
        R"({"_meta":{"progressToken":"1"},"content":{"type":"text","text":"hi"},"model":"claude","role":"assistant","stopReason":"end"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_create_message_result(val);
    assert(res.has_value());
    assert(res->model == "claude");
    assert(res->role == "assistant");

    utils::JsonWriter jw;
    mcp::emit_create_message_result(res.operator*(), jw);
    assert(jw.str().find("claude") != std::string::npos);
  }
  test_err(mcp::parse_create_message_result, R"([])");
  test_err(mcp::parse_create_message_result, R"({"content":{}})");
  test_err(mcp::parse_create_message_result, R"({"content":{},"model":"m"})");

  std::cout << "mcp::test_mcp_messaging passed.\n";
}

void test_mcp_tools() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // Tool
  {
    std::string json =
        R"({"name":"test_tool","description":"desc","inputSchema":{"type":"object","properties":{"a":{}},"required":["a"]}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_tool(val);
    assert(res.has_value());
    assert(res->name == "test_tool");
    assert(res->inputSchema.type == "object");
    assert(res->inputSchema.properties_json.has_value());
    assert(res->inputSchema.required.has_value() &&
           res->inputSchema.required->size() == 1);

    utils::JsonWriter jw;
    mcp::emit_tool(res.operator*(), jw);
    assert(jw.str().find("test_tool") != std::string::npos);
    assert(jw.str().find("properties") != std::string::npos);
  }
  test_err(mcp::parse_tool, R"([])");
  test_err(mcp::parse_tool, R"({"description":"d"})");
  test_err(mcp::parse_tool, R"({"name":"n"})");
  test_err(mcp::parse_tool, R"({"name":"n","inputSchema":{}})");

  // ToolListChangedNotification
  {
    std::string json =
        R"({"method":"notifications/tools/list_changed","params":{"_meta":{"key":"val"}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_tool_list_changed_notification(val);
    assert(res.has_value());
    assert(res->method == "notifications/tools/list_changed");
    assert(res->params.has_value() && res->params->_meta.has_value());

    utils::JsonWriter jw;
    mcp::emit_tool_list_changed_notification(res.operator*(), jw);
    assert(jw.str().find("list_changed") != std::string::npos);
  }
  {
    std::string json = R"({"method":"notifications/tools/list_changed"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_tool_list_changed_notification(val);
    assert(res.has_value());
    assert(!res->params.has_value());

    utils::JsonWriter jw;
    mcp::emit_tool_list_changed_notification(res.operator*(), jw);
    assert(jw.str() == R"({"method":"notifications/tools/list_changed"})");
  }
  test_err(mcp::parse_tool_list_changed_notification, R"([])");
  test_err(mcp::parse_tool_list_changed_notification, R"({"params":{}})");

  std::cout << "mcp::test_mcp_tools passed.\n";
}

void test_mcp_pagination() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // PaginatedRequest
  {
    std::string json =
        R"({"method":"test/paginated","params":{"cursor":"abc"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_paginated_request(val);
    assert(res.has_value() && res->method == "test/paginated");
    assert(res->params.cursor.has_value() &&
           res->params.cursor.operator*() == "abc");

    utils::JsonWriter jw;
    mcp::emit_paginated_request(res.operator*(), jw);
    assert(jw.str().find("abc") != std::string::npos);
  }
  {
    std::string json = R"({"method":"test/paginated"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_paginated_request(val);
    assert(res.has_value() && res->method == "test/paginated");
    assert(!res->params.cursor.has_value());

    utils::JsonWriter jw;
    mcp::emit_paginated_request(res.operator*(), jw);
    assert(jw.str() == R"({"method":"test/paginated"})");
  }
  test_err(mcp::parse_paginated_request, R"([])");
  test_err(mcp::parse_paginated_request, R"({})");

  // PaginatedResult
  {
    std::string json = R"({"_meta":{"key":"val"},"nextCursor":"c1"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_paginated_result(val);
    assert(res.has_value() && res->nextCursor.has_value() &&
           res->nextCursor.operator*() == "c1");
    assert(res->_meta.has_value() &&
           res->_meta.operator*().find("val") != std::string::npos);

    utils::JsonWriter jw;
    mcp::emit_paginated_result(res.operator*(), jw);
    assert(jw.str().find("c1") != std::string::npos);
  }
  {
    std::string json = R"({})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_paginated_result(val);
    assert(res.has_value() && !res->nextCursor.has_value() &&
           !res->_meta.has_value());

    utils::JsonWriter jw;
    mcp::emit_paginated_result(res.operator*(), jw);
    assert(jw.str() == R"({})");
  }
  test_err(mcp::parse_paginated_result, R"([])");

  std::cout << "mcp::test_mcp_pagination passed.\n";
}

void test_mcp_init() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // InitializeRequest
  {
    std::string json =
        R"({"method":"initialize","params":{"protocolVersion":"1","capabilities":{"roots":{}},"clientInfo":{"name":"c","version":"1.0"}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_initialize_request(val);
    assert(res.has_value());
    assert(res->params.protocolVersion == "1");
    assert(res->params.clientInfo.name == "c");
    assert(res->params.capabilities.has_value());

    utils::JsonWriter jw;
    mcp::emit_initialize_request(res.operator*(), jw);
    assert(jw.str().find("protocolVersion") != std::string::npos);
  }
  test_err(mcp::parse_initialize_request, R"([])");
  test_err(mcp::parse_initialize_request, R"({"params":{}})");
  test_err(mcp::parse_initialize_request, R"({"method":"m"})");
  test_err(mcp::parse_initialize_request, R"({"method":"m","params":{}})");
  test_err(mcp::parse_initialize_request,
           R"({"method":"m","params":{"protocolVersion":"1"}})");
  test_err(
      mcp::parse_initialize_request,
      R"({"method":"m","params":{"protocolVersion":"1","clientInfo":[]}})");

  // ServerCapabilities / InitializeResult
  {
    std::string json =
        R"({"protocolVersion":"1","capabilities":{"experimental":{},"logging":{},"prompts":{"listChanged":true},"resources":{"subscribe":false,"listChanged":true},"tools":{"listChanged":false}},"serverInfo":{"name":"s","version":"2"},"instructions":"hi"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_initialize_result(val);
    assert(res.has_value());
    assert(res->capabilities.prompts->listChanged == true);
    assert(res->capabilities.resources->subscribe == false);
    assert(res->instructions == "hi");

    utils::JsonWriter jw;
    mcp::emit_initialize_result(res.operator*(), jw);
    assert(jw.str().find("logging") != std::string::npos);
  }
  test_err(mcp::parse_initialize_result, R"([])");
  test_err(mcp::parse_initialize_result, R"({"protocolVersion":"1"})");
  test_err(mcp::parse_initialize_result,
           R"({"protocolVersion":"1","capabilities":[]})");
  test_err(mcp::parse_initialize_result,
           R"({"protocolVersion":"1","capabilities":{}})");
  test_err(mcp::parse_initialize_result,
           R"({"protocolVersion":"1","capabilities":{},"serverInfo":[]})");

  // InitializedNotification
  {
    std::string json =
        R"({"method":"notifications/initialized","params":{"_meta":{"x":1}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_initialized_notification(val);
    assert(res.has_value());

    utils::JsonWriter jw;
    mcp::emit_initialized_notification(res.operator*(), jw);
    assert(jw.str().find("notifications/initialized") != std::string::npos);
  }
  {
    std::string json = R"({"method":"notifications/initialized"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_initialized_notification(val);
    assert(res.has_value());
  }
  test_err(mcp::parse_initialized_notification, R"([])");
  test_err(mcp::parse_initialized_notification, R"({"params":{}})");

  std::cout << "mcp::test_mcp_init passed.\n";
}

void test_mcp_jsonrpc() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // Error
  {
    std::string json =
        R"({"jsonrpc":"2.0","id":1,"error":{"code":-32600,"message":"Invalid","data":{"x":1}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_error(val);
    assert(res.has_value());
    assert(res->id == "1");
    assert(res->error.code == -32600);

    utils::JsonWriter jw;
    mcp::emit_jsonrpc_error(res.operator*(), jw);
    assert(jw.str().find("-32600") != std::string::npos);
  }
  test_err(mcp::parse_jsonrpc_error, R"([])");
  test_err(mcp::parse_jsonrpc_error, R"({"id":1})");
  test_err(mcp::parse_jsonrpc_error, R"({"jsonrpc":"2.0"})");
  test_err(mcp::parse_jsonrpc_error, R"({"jsonrpc":"2.0","id":1})");
  test_err(mcp::parse_jsonrpc_error, R"({"jsonrpc":"2.0","id":1,"error":{}})");
  test_err(mcp::parse_jsonrpc_error,
           R"({"jsonrpc":"2.0","id":1,"error":{"code":1}})");

  // Notification
  {
    std::string json =
        R"({"jsonrpc":"2.0","method":"n","params":{"_meta":{"a":1},"b":2}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_notification(val);
    assert(res.has_value());
    assert(res->method == "n");
    assert(res->params.has_value() && res->params->_raw.has_value());

    utils::JsonWriter jw;
    mcp::emit_jsonrpc_notification(res.operator*(), jw);
    assert(jw.str().find("b") != std::string::npos);
  }
  {
    std::string json =
        R"({"jsonrpc":"2.0","method":"n","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_notification(val);
    res->params->_raw = std::nullopt; // Force emit branch
    utils::JsonWriter jw;
    mcp::emit_jsonrpc_notification(res.operator*(), jw);
    assert(jw.str().find("_meta") != std::string::npos);
  }
  test_err(mcp::parse_jsonrpc_notification, R"([])");
  test_err(mcp::parse_jsonrpc_notification, R"({"method":"m"})");
  test_err(mcp::parse_jsonrpc_notification, R"({"jsonrpc":"2.0"})");

  // Request
  {
    std::string json =
        R"({"jsonrpc":"2.0","id":"abc","method":"m","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_request(val);
    assert(res.has_value());
    assert(res->id == "\"abc\"");

    utils::JsonWriter jw;
    mcp::emit_jsonrpc_request(res.operator*(), jw);
    assert(jw.str().find("\"abc\"") != std::string::npos);
  }
  {
    std::string json =
        R"({"jsonrpc":"2.0","id":1,"method":"m","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_request(val);
    res->params->_raw = std::nullopt; // Force emit branch
    utils::JsonWriter jw;
    mcp::emit_jsonrpc_request(res.operator*(), jw);
    assert(jw.str().find("_meta") != std::string::npos);
  }
  test_err(mcp::parse_jsonrpc_request, R"([])");
  test_err(mcp::parse_jsonrpc_request, R"({"id":1})");
  test_err(mcp::parse_jsonrpc_request, R"({"jsonrpc":"2.0"})");
  test_err(mcp::parse_jsonrpc_request, R"({"jsonrpc":"2.0","id":1})");

  // Response
  {
    std::string json = R"({"jsonrpc":"2.0","id":null,"result":{"a":1}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_jsonrpc_response(val);
    assert(res.has_value());
    assert(res->id == "null");

    utils::JsonWriter jw;
    mcp::emit_jsonrpc_response(res.operator*(), jw);
    assert(jw.str().find("null") != std::string::npos);
  }
  test_err(mcp::parse_jsonrpc_response, R"([])");
  test_err(mcp::parse_jsonrpc_response, R"({"id":1})");
  test_err(mcp::parse_jsonrpc_response, R"({"jsonrpc":"2.0"})");
  test_err(mcp::parse_jsonrpc_response, R"({"jsonrpc":"2.0","id":1})");

  std::cout << "mcp::test_mcp_jsonrpc passed.\n";
}

void test_mcp_prompts() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // ListPromptsRequest
  {
    std::string json = R"({"method":"prompts/list","params":{"cursor":"a"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_prompts_request(val);
    assert(res.has_value());
    assert(res->params.has_value() && res->params->cursor == "a");

    utils::JsonWriter jw;
    mcp::emit_list_prompts_request(res.operator*(), jw);
    assert(jw.str().find("prompts/list") != std::string::npos);
  }
  {
    std::string json = R"({"method":"prompts/list"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_prompts_request(val);
    assert(res.has_value());
    assert(!res->params.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_prompts_request(res.operator*(), jw);
  }
  test_err(mcp::parse_list_prompts_request, R"([])");
  test_err(mcp::parse_list_prompts_request, R"({})");

  // ListPromptsResult
  {
    std::string json =
        R"({"_meta":{"k":"v"},"nextCursor":"n","prompts":[{"name":"p1","description":"d","arguments":[{"name":"arg1","description":"ad","required":true}]}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_prompts_result(val);
    assert(res.has_value());
    assert(res->prompts.size() == 1);
    assert(res->prompts[0].name == "p1");
    assert(res->prompts[0].arguments->size() == 1);

    utils::JsonWriter jw;
    mcp::emit_list_prompts_result(res.operator*(), jw);
    assert(jw.str().find("arg1") != std::string::npos);
  }
  test_err(mcp::parse_list_prompts_result, R"([])");
  test_err(mcp::parse_list_prompts_result, R"({})");

  // GetPromptRequest
  {
    std::string json =
        R"({"method":"prompts/get","params":{"name":"p1","arguments":{"arg1":"val"}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_get_prompt_request(val);
    assert(res.has_value());
    assert(res->params.name == "p1");

    utils::JsonWriter jw;
    mcp::emit_get_prompt_request(res.operator*(), jw);
    assert(jw.str().find("val") != std::string::npos);
  }
  test_err(mcp::parse_get_prompt_request, R"([])");
  test_err(mcp::parse_get_prompt_request, R"({"params":{}})");
  test_err(mcp::parse_get_prompt_request, R"({"method":"m"})");
  test_err(mcp::parse_get_prompt_request, R"({"method":"m","params":{}})");

  // GetPromptResult
  {
    std::string json =
        R"({"_meta":{},"description":"d","messages":[{"role":"user","content":{"type":"text","text":"hi"}}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_get_prompt_result(val);
    assert(res.has_value());
    assert(res->messages.size() == 1);
    assert(res->messages[0].role == "user");

    utils::JsonWriter jw;
    mcp::emit_get_prompt_result(res.operator*(), jw);
    assert(jw.str().find("user") != std::string::npos);
  }
  test_err(mcp::parse_get_prompt_result, R"([])");
  test_err(mcp::parse_get_prompt_result, R"({})");

  // error branches inside prompt
  test_err(mcp::parse_prompt, R"([])");
  test_err(mcp::parse_prompt, R"({})");
  test_err(mcp::parse_prompt_argument, R"([])");
  test_err(mcp::parse_prompt_argument, R"({})");

  // PromptListChangedNotification
  {
    std::string json =
        R"({"method":"notifications/prompts/list_changed","params":{"_meta":{"k":1}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_prompt_list_changed_notification(val);
    assert(res.has_value());
    assert(res->method == "notifications/prompts/list_changed");
    assert(res->params.has_value() && res->params->_meta.has_value());

    utils::JsonWriter jw;
    mcp::emit_prompt_list_changed_notification(res.operator*(), jw);
    assert(jw.str().find("list_changed") != std::string::npos);
  }
  {
    std::string json = R"({"method":"notifications/prompts/list_changed"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_prompt_list_changed_notification(val);
    assert(res.has_value());
    assert(!res->params.has_value());

    utils::JsonWriter jw;
    mcp::emit_prompt_list_changed_notification(res.operator*(), jw);
    assert(jw.str() == R"({"method":"notifications/prompts/list_changed"})");
  }
  test_err(mcp::parse_prompt_list_changed_notification, R"([])");
  test_err(mcp::parse_prompt_list_changed_notification, R"({"params":{}})");

  // PromptReference
  {
    std::string json = R"({"type":"ref/prompt","name":"p1"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_prompt_reference(val);
    assert(res.has_value());
    assert(res->type == "ref/prompt" && res->name == "p1");

    utils::JsonWriter jw;
    mcp::emit_prompt_reference(res.operator*(), jw);
    assert(jw.str().find("ref/prompt") != std::string::npos);
  }
  test_err(mcp::parse_prompt_reference, R"([])");
  test_err(mcp::parse_prompt_reference, R"({"type":"ref/prompt"})");
  test_err(mcp::parse_prompt_reference, R"({"name":"p1"})");

  std::cout << "mcp::test_mcp_prompts passed.\n";
}

void test_mcp_resources() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // ListResourcesRequest
  {
    std::string json =
        R"({"method":"resources/list","params":{"cursor":"abc"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resources_request(val);
    assert(res.has_value() && res->params->cursor == "abc");
    utils::JsonWriter jw;
    mcp::emit_list_resources_request(res.operator*(), jw);
    assert(jw.str().find("abc") != std::string::npos);
  }
  {
    std::string json = R"({"method":"resources/list"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resources_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_resources_request(res.operator*(), jw);
  }
  test_err(mcp::parse_list_resources_request, R"([])");
  test_err(mcp::parse_list_resources_request, R"({})");

  // Resource & ListResourcesResult
  {
    std::string json =
        R"({"_meta":{"k":1},"nextCursor":"n","resources":[{"uri":"file:///a.txt","name":"a","description":"d","mimeType":"text/plain"}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resources_result(val);
    assert(res.has_value());
    assert(res->resources.size() == 1);
    utils::JsonWriter jw;
    mcp::emit_list_resources_result(res.operator*(), jw);
    assert(jw.str().find("file:///a.txt") != std::string::npos);
  }
  test_err(mcp::parse_list_resources_result, R"([])");
  test_err(mcp::parse_list_resources_result, R"({})");
  test_err(mcp::parse_list_resources_result, R"({"resources":[{}]})");
  test_err(mcp::parse_resource, R"([])");
  test_err(mcp::parse_resource, R"({})");
  test_err(mcp::parse_resource, R"({"uri":"u"})");

  // ListResourceTemplatesRequest
  {
    std::string json =
        R"({"method":"resources/templates/list","params":{"cursor":"c"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resource_templates_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_resource_templates_request(res.operator*(), jw);
    assert(jw.str().find("cursor") != std::string::npos);
  }
  {
    std::string json = R"({"method":"resources/templates/list"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resource_templates_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_resource_templates_request(res.operator*(), jw);
  }
  test_err(mcp::parse_list_resource_templates_request, R"([])");
  test_err(mcp::parse_list_resource_templates_request, R"({})");

  // ResourceTemplate & ListResourceTemplatesResult
  {
    std::string json =
        R"({"_meta":{},"nextCursor":"c","resourceTemplates":[{"uriTemplate":"file:///{file}","name":"n","description":"d","mimeType":"m"}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_resource_templates_result(val);
    assert(res.has_value());
    assert(res->resourceTemplates.size() == 1);
    utils::JsonWriter jw;
    mcp::emit_list_resource_templates_result(res.operator*(), jw);
    assert(jw.str().find("file:///{file}") != std::string::npos);
  }
  test_err(mcp::parse_list_resource_templates_result, R"([])");
  test_err(mcp::parse_list_resource_templates_result, R"({})");
  test_err(mcp::parse_list_resource_templates_result,
           R"({"resourceTemplates":[{}]})");
  test_err(mcp::parse_resource_template, R"([])");
  test_err(mcp::parse_resource_template, R"({})");
  test_err(mcp::parse_resource_template, R"({"uriTemplate":"u"})");

  // ReadResourceRequest
  {
    std::string json =
        R"({"method":"resources/read","params":{"uri":"file:///a.txt"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_read_resource_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_read_resource_request(res.operator*(), jw);
    assert(jw.str().find("file:///a.txt") != std::string::npos);
  }
  test_err(mcp::parse_read_resource_request, R"([])");
  test_err(mcp::parse_read_resource_request, R"({})");
  test_err(mcp::parse_read_resource_request, R"({"method":"m"})");
  test_err(mcp::parse_read_resource_request, R"({"method":"m","params":{}})");

  // ReadResourceResult
  {
    std::string json =
        R"({"_meta":{},"contents":[{"uri":"f","mimeType":"t","text":"hello"}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_read_resource_result(val);
    assert(res.has_value());
    assert(res->contents_json.size() == 1);
    utils::JsonWriter jw;
    mcp::emit_read_resource_result(res.operator*(), jw);
    assert(jw.str().find("hello") != std::string::npos);
  }
  test_err(mcp::parse_read_resource_result, R"([])");
  test_err(mcp::parse_read_resource_result, R"({})");

  // ResourceUpdatedNotification
  {
    std::string json =
        R"({"method":"notifications/resources/updated","params":{"uri":"file:///a.txt"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_resource_updated_notification(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_resource_updated_notification(res.operator*(), jw);
    assert(jw.str().find("file:///a.txt") != std::string::npos);
  }
  test_err(mcp::parse_resource_updated_notification, R"([])");
  test_err(mcp::parse_resource_updated_notification, R"({})");
  test_err(mcp::parse_resource_updated_notification, R"({"method":"m"})");
  test_err(mcp::parse_resource_updated_notification,
           R"({"method":"m","params":{}})");

  // ResourceListChangedNotification
  {
    std::string json =
        R"({"method":"notifications/resources/list_changed","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_resource_list_changed_notification(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_resource_list_changed_notification(res.operator*(), jw);
    assert(jw.str().find("_meta") != std::string::npos);
  }
  {
    std::string json = R"({"method":"notifications/resources/list_changed"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_resource_list_changed_notification(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_resource_list_changed_notification(res.operator*(), jw);
  }
  test_err(mcp::parse_resource_list_changed_notification, R"([])");
  test_err(mcp::parse_resource_list_changed_notification, R"({})");

  // ResourceReference
  {
    std::string json = R"({"type":"ref/resource","uri":"file:///a.txt"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_resource_reference(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_resource_reference(res.operator*(), jw);
    assert(jw.str().find("file:///a.txt") != std::string::npos);
  }
  test_err(mcp::parse_resource_reference, R"([])");
  test_err(mcp::parse_resource_reference, R"({})");
  test_err(mcp::parse_resource_reference, R"({"type":"t"})");

  std::cout << "mcp::test_mcp_resources passed.\n";
}

void test_mcp_remaining() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // Result
  {
    std::string json = R"({"_meta":{"k":1}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_result(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_result(res.operator*(), jw);
    assert(jw.str().find("_meta") != std::string::npos);
  }
  {
    std::string json = R"({})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_result(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_result(res.operator*(), jw);
    assert(jw.str() == R"({})");
  }
  test_err(mcp::parse_result, R"([])");

  // Root
  {
    std::string json = R"({"uri":"file:///","name":"root"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_root(val);
    assert(res.has_value() && res->name == "root");
    utils::JsonWriter jw;
    mcp::emit_root(res.operator*(), jw);
    assert(jw.str().find("root") != std::string::npos);
  }
  test_err(mcp::parse_root, R"([])");
  test_err(mcp::parse_root, R"({})");

  // RootsListChangedNotification
  {
    std::string json =
        R"({"method":"notifications/roots/list_changed","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_roots_list_changed_notification(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_roots_list_changed_notification(res.operator*(), jw);
    assert(jw.str().find("list_changed") != std::string::npos);
  }
  {
    std::string json = R"({"method":"notifications/roots/list_changed"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_roots_list_changed_notification(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_roots_list_changed_notification(res.operator*(), jw);
  }
  test_err(mcp::parse_roots_list_changed_notification, R"([])");
  test_err(mcp::parse_roots_list_changed_notification, R"({})");

  // ListRootsRequest
  {
    std::string json = R"({"method":"roots/list","params":{"_meta":{}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_roots_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_roots_request(res.operator*(), jw);
    assert(jw.str().find("_meta") != std::string::npos);
  }
  {
    std::string json = R"({"method":"roots/list"})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_roots_request(val);
    assert(res.has_value());
    utils::JsonWriter jw;
    mcp::emit_list_roots_request(res.operator*(), jw);
  }
  test_err(mcp::parse_list_roots_request, R"([])");
  test_err(mcp::parse_list_roots_request, R"({})");

  // ListRootsResult
  {
    std::string json = R"({"_meta":{},"roots":[{"uri":"file:///"}]})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_list_roots_result(val);
    assert(res.has_value());
    assert(res->roots.size() == 1);
    utils::JsonWriter jw;
    mcp::emit_list_roots_result(res.operator*(), jw);
    assert(jw.str().find("file:///") != std::string::npos);
  }
  test_err(mcp::parse_list_roots_result, R"([])");
  test_err(mcp::parse_list_roots_result, R"({})");
  test_err(mcp::parse_list_roots_result, R"({"roots":[{}]})");

  // cover models
  {
    cdd_cpp::mcp::EmptyResult er;
    cdd_cpp::mcp::EmptyResult er2 = std::move(er);

    cdd_cpp::mcp::SamplingMessage sm;
    cdd_cpp::mcp::SamplingMessage sm2 = std::move(sm);

    cdd_cpp::mcp::PingRequest pr;
    pr.params = cdd_cpp::mcp::PingRequestParams();
    cdd_cpp::mcp::PingRequest pr2 = std::move(pr);

    cdd_cpp::mcp::SubscribeRequest sr;
    sr.params = cdd_cpp::mcp::SubscribeRequestParams();
    cdd_cpp::mcp::SubscribeRequest sr2 = std::move(sr);

    cdd_cpp::mcp::UnsubscribeRequest ur;
    ur.params = cdd_cpp::mcp::UnsubscribeRequestParams();
    cdd_cpp::mcp::UnsubscribeRequest ur2 = std::move(ur);

    cdd_cpp::mcp::ListToolsRequest ltr;
    ltr.params = cdd_cpp::mcp::ListToolsRequestParams();
    cdd_cpp::mcp::ListToolsRequest ltr2 = std::move(ltr);

    cdd_cpp::mcp::ListToolsResult ltres;
    cdd_cpp::mcp::ListToolsResult ltres2 = std::move(ltres);

    cdd_cpp::utils::JsonWriter jw;
    cdd_cpp::mcp::emit_empty_result(er2, jw);
    cdd_cpp::mcp::emit_sampling_message(sm2, jw);
    cdd_cpp::mcp::emit_ping_request(pr2, jw);
    cdd_cpp::mcp::emit_subscribe_request(sr2, jw);
    cdd_cpp::mcp::emit_unsubscribe_request(ur2, jw);
    cdd_cpp::mcp::emit_list_tools_request(ltr2, jw);
    cdd_cpp::mcp::emit_list_tools_result(ltres2, jw);

    std::string json = "{}";
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);
    auto val = doc.get_value();
    cdd_cpp::mcp::parse_empty_result(val.value_unsafe());

    auto doc2 = parser.iterate(json);
    auto val2 = doc2.get_value();
    cdd_cpp::mcp::parse_sampling_message(val2.value_unsafe());

    auto doc3 = parser.iterate(json);
    auto val3 = doc3.get_value();
    cdd_cpp::mcp::parse_ping_request(val3.value_unsafe());

    auto doc4 = parser.iterate(json);
    auto val4 = doc4.get_value();
    cdd_cpp::mcp::parse_subscribe_request(val4.value_unsafe());

    auto doc5 = parser.iterate(json);
    auto val5 = doc5.get_value();
    cdd_cpp::mcp::parse_unsubscribe_request(val5.value_unsafe());

    auto doc6 = parser.iterate(json);
    auto val6 = doc6.get_value();
    cdd_cpp::mcp::parse_list_tools_request(val6.value_unsafe());

    auto doc7 = parser.iterate(json);
    auto val7 = doc7.get_value();
    cdd_cpp::mcp::parse_list_tools_result(val7.value_unsafe());

    cdd_cpp::mcp::Annotations a1, a2;
    a1 = a2;
    a1 = std::move(a2);
    cdd_cpp::mcp::ClientCapabilities cc1, cc2;
    cc1 = cc2;
    cdd_cpp::mcp::ModelHint mh1, mh2;
    mh1 = mh2;
    cdd_cpp::mcp::ModelPreferences mp1, mp2;
    mp1 = mp2;
    cdd_cpp::mcp::ToolInputSchema tis1, tis2;
    tis1 = tis2;
    cdd_cpp::mcp::ToolInputSchema tis3(tis1);
    cdd_cpp::mcp::Tool t1, t2;
    t1 = t2;
    cdd_cpp::mcp::Tool t3(t1);
    cdd_cpp::mcp::ToolListChangedNotificationParams tcp1, tcp2;
    tcp1 = tcp2;
    cdd_cpp::mcp::InitializedNotificationParams inp1, inp2;
    inp1 = inp2;
    cdd_cpp::mcp::JSONRPCNotificationParams jnp1, jnp2;
    jnp1 = jnp2;
    cdd_cpp::mcp::JSONRPCRequestParams jrp1, jrp2;
    jrp1 = jrp2;
    cdd_cpp::mcp::ListPromptsRequestParams lpr1, lpr2;
    lpr1 = lpr2;
    cdd_cpp::mcp::PromptArgument pa1, pa2;
    pa1 = pa2;
    cdd_cpp::mcp::PromptMessage pm1, pm2;
    pm1 = std::move(pm2);
    cdd_cpp::mcp::PromptMessage pm3(std::move(pm1));
    cdd_cpp::mcp::PromptListChangedNotificationParams plp1, plp2;
    plp1 = plp2;
    cdd_cpp::mcp::ListResourcesRequestParams lrr1, lrr2;
    lrr1 = lrr2;
    cdd_cpp::mcp::ListResourceTemplatesRequestParams lrt1, lrt2;
    lrt1 = lrt2;
    cdd_cpp::mcp::ResourceListChangedNotificationParams rlc1, rlc2;
    rlc1 = rlc2;
    cdd_cpp::mcp::RootsListChangedNotificationParams roc1, roc2;
    roc1 = roc2;
    cdd_cpp::mcp::ListRootsRequestParams lrrp1, lrrp2;
    lrrp1 = lrrp2;
  }

  std::cout << "mcp::test_mcp_remaining passed.\n";
}

void test_mcp_logging_progress() {
  simdjson::ondemand::parser parser;
  auto test_err = [&](auto func, const std::string &j) {
    simdjson::padded_string p(j);
    simdjson::ondemand::document d = parser.iterate(p);
    simdjson::ondemand::value v = d.get_value();
    auto res = func(v);
    assert(!res.has_value());
    (void)res;
  };

  // SetLevelRequest
  {
    std::string json =
        R"({"method":"logging/setLevel","params":{"level":"debug"}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_set_level_request(val);
    assert(res.has_value() && res->params.level == "debug");
    utils::JsonWriter jw;
    mcp::emit_set_level_request(res.operator*(), jw);
    assert(jw.str().find("debug") != std::string::npos);
  }
  test_err(mcp::parse_set_level_request, R"([])");
  test_err(mcp::parse_set_level_request, R"({"method":"m"})");
  test_err(mcp::parse_set_level_request, R"({"method":"m","params":{}})");

  // LoggingMessageNotification
  {
    std::string json =
        R"({"method":"notifications/message","params":{"level":"info","logger":"main","data":{"a":1}}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_logging_message_notification(val);
    assert(res.has_value() && res->params.level == "info" &&
           res->params.logger == "main");
    utils::JsonWriter jw;
    mcp::emit_logging_message_notification(res.operator*(), jw);
    assert(jw.str().find("info") != std::string::npos);
  }
  test_err(mcp::parse_logging_message_notification, R"([])");
  test_err(mcp::parse_logging_message_notification, R"({"method":"m"})");
  test_err(mcp::parse_logging_message_notification,
           R"({"method":"m","params":{}})");
  test_err(mcp::parse_logging_message_notification,
           R"({"method":"m","params":{"level":"i"}})");

  // ProgressNotification
  {
    std::string json =
        R"({"method":"notifications/progress","params":{"progressToken":"t1","progress":10,"total":100}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_progress_notification(val);
    assert(res.has_value() && res->params.progressToken == "\"t1\"");
    assert(res->params.progress == 10);
    assert(res->params.total.operator*() == 100);
    utils::JsonWriter jw;
    mcp::emit_progress_notification(res.operator*(), jw);
    assert(jw.str().find("\"t1\"") != std::string::npos);
  }
  {
    std::string json =
        R"({"method":"notifications/progress","params":{"progressToken":2,"progress":10}})";
    simdjson::padded_string padded(json);
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::value val = doc.get_value();
    auto res = mcp::parse_progress_notification(val);
    assert(res.has_value() && res->params.progressToken == "2");
    utils::JsonWriter jw;
    mcp::emit_progress_notification(res.operator*(), jw);
    assert(jw.str().find("2") != std::string::npos);
  }
  test_err(mcp::parse_progress_notification, R"([])");
  test_err(mcp::parse_progress_notification, R"({"method":"m"})");
  test_err(mcp::parse_progress_notification, R"({"method":"m","params":{}})");
  test_err(mcp::parse_progress_notification,
           R"({"method":"m","params":{"progressToken":"t"}})");

  std::cout << "mcp::test_mcp_logging_progress passed.\n";
}

// GCOV_EXCL_BR_STOP
