/// @file emit.cpp
/// @brief Implementation of C++ HTTP server and JSON-RPC emitters from
/// OpenAPI 3.2.0 specs.

#include "../docstrings/emit.hpp"
#include "../cli/api.hpp"
#include "../mcp/emit.hpp"
#include "../mcp/parse.hpp"
#include "emit.hpp"
#include <simdjson.h>
#include <sstream>

namespace cdd_cpp::server {

/// @brief Map an OpenAPI schema type to its C++ primitive/standard library
/// type.
/// @param type The OpenAPI primitive type string.
/// @return C++ type string ("std::string", "int", "double", "bool").
std::string map_type_to_cpp(const std::string &type) noexcept {
  return openapi::openapi_type_to_cpp(type);
}

/// @brief Emit C++ HTTP server or JSON-RPC handler for `emit`.
std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n\n";
  ss << "namespace cdd_server {\n\n";
  ss << docstrings::emit_api_docstrings(spec);

  ss << "    class Router {\n";
  ss << "    public:\n";
  ss << "        using Handler = std::function<std::string(const "
        "std::string&)>;\n\n";

  if (spec.paths.has_value()) {
    for (const auto &[path, item] : *spec.paths) {
      auto emit_method = [&](const std::string &method,
                             const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("handler");

        ss << docstrings::emit_path_docstrings(item);

        std::vector<openapi::Parameter> all_params;
        if (item.parameters) {
          for (const auto &p : *item.parameters)
            all_params.push_back(p);
        }
        if (op->parameters) {
          for (const auto &p : *op->parameters)
            all_params.push_back(p);
        }

        openapi::Operation new_op = op.operator*();
        new_op.parameters = all_params;
        ss << docstrings::emit_operation_docstrings(new_op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = (p.schema && p.schema->type)
                                 ? map_type_to_cpp(*p.schema->type)
                                 : "std::string";
          if (i > 0)
            param_list += ", ";
          param_list += type + " " + p.name;
        }
        if (op->requestBody) {
          if (!param_list.empty())
            param_list += ", ";
          param_list += "std::string requestBody";
        }
        if (param_list.empty()) {
          param_list = "const std::string&";
        }

        ss << "        using " << func_name
           << "Handler = std::function<std::string(" << param_list << ")>;\n";
        ss << "        void on_" << method << "_" << func_name
           << "(std::function<std::string(" << param_list
           << ")> handler) noexcept {\n";
        ss << "            (void)handler;\n";
        ss << "            routes[\"" << method << " " << path
           << "\"] = [](const std::string& req) { return req; };\n";
        ss << "        }\n\n";
      };

      emit_method("GET", item.get);
      emit_method("POST", item.post);
      emit_method("PUT", item.put);
      emit_method("DELETE", item.delete_op);
      emit_method("PATCH", item.patch);
      emit_method("OPTIONS", item.options);
      emit_method("HEAD", item.head);
      emit_method("TRACE", item.trace);
      emit_method("QUERY", item.query);
    }
  }

  ss << "        std::string handle_mcp_sse() const noexcept {\n";
  ss << "            return \"HTTP/1.1 200 OK\\r\\nContent-Type: "
        "text/event-stream\\r\\n\\r\\nevent: endpoint\\ndata: "
        "/mcp/message\\n\\n\";\n";
  ss << "        }\n\n";

  ss << "        std::string handle_mcp_message(const std::string& "
        "request_json, const std::map<std::string, std::string>& auth_headers "
        "= {}) const noexcept {\n";
  ss << "            // Parse json and forward to appropriate handler\n";
  ss << "            return "
        "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":null,\\\"error\\\":{"
        "\\\"code\\\":-32601,\\\"message\\\":\\\"Method not found\\\"}}\";\n";
  ss << "        }\n\n";
  ss << "    private:\n";

  ss << "        std::map<std::string, Handler> routes;\n";
  ss << "    };\n";
  ss << "}\n";

  return ss.str();
}

/// @brief Emit C++ HTTP server or JSON-RPC handler for `serve_json_rpc`.
std::string serve_json_rpc(const std::string &request) noexcept {
  auto make_err = [](const std::string &id, int code, const std::string &msg) {
    mcp::JSONRPCError err_obj;
    err_obj.jsonrpc = "2.0";
    err_obj.id = id;
    err_obj.error.code = code;
    err_obj.error.message = msg;
    utils::JsonWriter jw;
    mcp::emit_jsonrpc_error(err_obj, jw);
    return jw.str();
  };

  auto make_ok = [](const std::string &id, const std::string &result_json) {
    mcp::JSONRPCResponse resp_obj;
    resp_obj.jsonrpc = "2.0";
    resp_obj.id = id;
    resp_obj.result_json = result_json;
    utils::JsonWriter jw;
    mcp::emit_jsonrpc_response(resp_obj, jw);
    return jw.str();
  };

  if (request.empty()) {
    return make_err("null", -32601, "Method not found");
  }

  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(request);
  simdjson::ondemand::document doc;
  if (parser.iterate(padded).get(doc)) {
    return make_err("null", -32700, "Parse error");
  }

  std::string id_str = "null";
  simdjson::ondemand::value id_val;
  if (!doc["id"].get(id_val)) {
    int64_t num_id = 0;
    std::string_view sv_id;
    if (!id_val.get_int64().get(num_id)) {
      id_str = std::to_string(num_id);
    } else if (!id_val.get_string().get(sv_id)) {
      id_str = "\"" + utils::escape_json_string(sv_id) + "\"";
    }
  }

  std::string_view jsonrpc_v;
  if (doc["jsonrpc"].get(jsonrpc_v) || jsonrpc_v != "2.0") {
    return make_err(id_str, -32600, "Invalid Request");
  }

  std::string method = "";
  std::string_view method_v;
  if (!doc["method"].get(method_v)) {
    method = method_v;
  }

  if (method == "initialize") {
    mcp::InitializeResult init_res;
    init_res.protocolVersion = "2024-11-05";
    init_res.capabilities.tools = mcp::ServerCapabilitiesTools{};
    init_res.serverInfo.name = "cdd-cli-mcp";
    init_res.serverInfo.version = "0.0.4";
    utils::JsonWriter jw;
    mcp::emit_initialize_result(init_res, jw);
    return make_ok(id_str, jw.str());
  } else if (method == "tools/list") {
    mcp::ListToolsResult ltr;
    mcp::Tool t1;
    t1.name = "cdd_generate";
    t1.description = "Generate SDKs or Servers";
    t1.inputSchema.type = "object";
    t1.inputSchema.properties_json =
        "{\"command\":{\"type\":\"string\"},\"input\":{\"type\":\"string\"},"
        "\"output\":{\"type\":\"string\"}}";
    t1.inputSchema.required =
        std::vector<std::string>{"command", "input", "output"};
    mcp::Tool t2;
    t2.name = "cdd_inspect";
    t2.description = "Inspect Schemas";
    t2.inputSchema.type = "object";
    t2.inputSchema.properties_json = "{\"input\":{\"type\":\"string\"}}";
    t2.inputSchema.required = std::vector<std::string>{"input"};
    mcp::Tool t3;
    t3.name = "cdd_sync";
    t3.description = "Bidirectional sync";
    t3.inputSchema.type = "object";
    t3.inputSchema.properties_json =
        "{\"input\":{\"type\":\"string\"},\"output\":{\"type\":\"string\"}}";
    t3.inputSchema.required = std::vector<std::string>{"input", "output"};
    ltr.tools = {t1, t2, t3};
    utils::JsonWriter jw;
    mcp::emit_list_tools_result(ltr, jw);
    return make_ok(id_str, jw.str());
  } else if (method == "tools/call") {
    std::string tool_name = "";
    std::string_view name_sv;
    if (!doc["params"]["name"].get(name_sv)) {
      tool_name = std::string(name_sv);
    }

    if (tool_name == "cdd_generate") {
      std::string cmd_arg = "";
      std::string in_arg = "";
      std::string out_arg = "";
      simdjson::ondemand::object args_obj;
      if (!doc["params"]["arguments"].get(args_obj)) {
        std::string_view sv;
        if (!args_obj["command"].get(sv))
          cmd_arg = std::string(sv);
        if (!args_obj["input"].get(sv))
          in_arg = std::string(sv);
        if (!args_obj["output"].get(sv))
          out_arg = std::string(sv);
      }
      if (!in_arg.empty()) {
        std::ostringstream out_ss, err_ss;
        if (cmd_arg == "to_openapi") {
          cli::ToOpenApiConfig cfg;
          cfg.input = in_arg;
          cfg.output = out_arg;
          cli::to_openapi(cfg, out_ss, err_ss);
        } else if (cmd_arg == "to_arazzo") {
          cli::ToArazzoConfig cfg;
          cfg.input = in_arg;
          cfg.output = out_arg;
          cli::to_arazzo(cfg, out_ss, err_ss);
        } else if (cmd_arg == "from_arazzo") {
          cli::FromArazzoConfig cfg;
          cfg.input = in_arg;
          cfg.output = out_arg;
          cli::from_arazzo(cfg, out_ss, err_ss);
        } else {
          cli::FromOpenApiConfig cfg;
          cfg.input = in_arg;
          cfg.output = out_arg.empty() ? "." : out_arg;
          cfg.subcommand = cmd_arg.empty() ? "to_sdk" : cmd_arg;
          if (cli::from_openapi(cfg, out_ss, err_ss) != 0) {
            return make_err(id_str, -32603, err_ss.str());
          }
        }
      }
      mcp::CallToolResult ctr;
      ctr.content_json = "[{\"type\":\"text\",\"text\":\"OK\"}]";
      utils::JsonWriter jw;
      mcp::emit_call_tool_result(ctr, jw);
      return make_ok(id_str, jw.str());
    } else if (tool_name == "cdd_inspect" || tool_name == "cdd_sync") {
      mcp::CallToolResult ctr;
      ctr.content_json = "[{\"type\":\"text\",\"text\":\"OK\"}]";
      utils::JsonWriter jw;
      mcp::emit_call_tool_result(ctr, jw);
      return make_ok(id_str, jw.str());
    }
    return make_err(id_str, -32601, "Tool not found");
  }

  return make_err(id_str, -32601, "Method not found");
}

std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool no_installable_package,
                                                bool tests, bool with_postgres,
                                                bool with_faker) noexcept {
  std::map<std::string, std::string> files;
  std::vector<std::string> names;

  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.operator*()) {
      if (schema.type.value_or("") != "object" || !schema.properties)
        continue;
      names.push_back(name);
      std::string table_name = name;
      for (auto &c : table_name)
        c = std::tolower(c);

      // Mocks
      if (with_faker) {
        std::stringstream mk_ss;
        mk_ss << "#pragma once\n#include \"../models/" << name
              << ".hpp\"\n#include <faker-cxx/String.h>\n\n";
        mk_ss << "namespace cdd_mocks {\n";
        mk_ss << "    /// Fake Data generator for the " << name
              << " model.\n    class " << name << "Mock {\n    public:\n";
        mk_ss << "        static cdd_models::" << name
              << " generate() noexcept {\n";
        mk_ss << "            cdd_models::" << name << " item;\n";
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          std::string c_type =
              map_type_to_cpp(prop_schema.type.value_or("string"));
          if (c_type == "std::string") {
            mk_ss << "            item." << prop_name
                  << " = faker::String::alphanumeric(10);\n";
          } else if (c_type == "int") {
            mk_ss << "            item." << prop_name << " = 42;\n";
          } else if (c_type == "double") {
            mk_ss << "            item." << prop_name << " = 3.14;\n";
          } else {
            mk_ss << "            item." << prop_name << " = true;\n";
          }
        }
        mk_ss << "            return item;\n";
        mk_ss << "        }\n    };\n}\n";
        files["src/mocks/" + name + "Mock.hpp"] = mk_ss.str();
      }

      // Routes
      std::stringstream r_ss;
      r_ss << "#pragma once\n#include <httplib.h>\n#include \"../orm/" << name
           << "Dao.hpp\"\n\n";
      r_ss << "namespace cdd_routes {\n";
      r_ss << "    inline void register_" << table_name
           << "_routes(httplib::Server& svr, cdd_orm::I" << name
           << "Dao* dao) noexcept {\n";
      r_ss << "        svr.Get(\"/api/" << table_name
           << "\", [dao](const httplib::Request&, httplib::Response& res) {\n";
      r_ss << "            if (!dao) { res.status = 501; "
              "res.set_content(\"{\\\"error\\\":\\\"Not Implemented\\\"}\", "
              "\"application/json\"); return; }\n";
      r_ss << "            res.status = 200; res.set_content(\"[]\", "
              "\"application/json\");\n";
      r_ss << "        });\n    }\n}\n";
      files["src/routes/" + name + "Routes.hpp"] = r_ss.str();

      if (tests) {
        std::stringstream t_ss;
        t_ss << "#include <gtest/gtest.h>\n#include \"../src/orm/" << name
             << "Dao.hpp\"\n\n";
        t_ss << "TEST(" << name << "Test, StubReturnsNotImplemented) {\n";
        t_ss << "    cdd_orm::Stub" << name
             << "Dao dao;\n    auto res = dao.create_table();\n";
        t_ss << "    EXPECT_FALSE(res.has_value());\n    "
                "EXPECT_EQ(res.error(), cdd_orm::Error::NotImplemented);\n}\n";
        files["tests/" + name + "Test.cpp"] = t_ss.str();
      }
    }
  }

  // Server Entrypoint
  std::stringstream s_ss;
  s_ss << "#include <iostream>\n#include <httplib.h>\n#include "
          "\"orm/error.hpp\"\n";
  if (with_postgres)
    s_ss << "#include \"orm/database.hpp\"\n";
  s_ss << "#include \"orm/RepositoryFactory.hpp\"\n";
  for (const auto &name : names) {
    s_ss << "#include \"routes/" << name << "Routes.hpp\"\n";
    if (with_faker)
      s_ss << "#include \"mocks/" << name << "Mock.hpp\"\n";
  }
  s_ss << "\nint main(int argc, char** argv) noexcept {\n";
  if (with_postgres) {
    s_ss << "    auto config = cdd_orm::DatabaseConfig::from_env();\n";
    s_ss << "    for (int i = 1; i < argc; ++i) { if (std::string(argv[i]) == "
            "\"--ephemeral\") config.is_ephemeral = true; }\n";
    s_ss << "    auto conn_res = "
            "cdd_orm::ConnectionFactory::create_connection(config);\n";
    s_ss << "    PGconn* conn = nullptr;\n    if (conn_res) conn = "
            "*conn_res;\n    else std::cerr << \"Stub mode.\\n\";\n";
    s_ss << "    cdd_orm::RepositoryFactory factory(conn, conn == nullptr);\n";
  } else {
    s_ss << "    (void)argc;\n    (void)argv;\n";
    s_ss << "    cdd_orm::RepositoryFactory factory;\n";
  }
  s_ss << "    httplib::Server svr;\n";
  for (const auto &name : names) {
    std::string table_name = name;
    for (auto &c : table_name)
      c = std::tolower(c);
    s_ss << "    auto dao_" << name << " = factory.create_" << name
         << "_dao();\n";
    s_ss << "    cdd_routes::register_" << table_name << "_routes(svr, dao_"
         << name << ".get());\n";
  }
  s_ss << "    svr.listen(\"0.0.0.0\", 8080);\n    return 0;\n}\n";
  files["src/generated_server.cpp"] = s_ss.str();

  if (!no_installable_package) {
    std::stringstream c_ss;
    c_ss << "cmake_minimum_required(VERSION 3.15)\nproject(mock_server "
            "LANGUAGES CXX)\n";
    c_ss << "set(CMAKE_CXX_STANDARD 26)\n"
            "add_definitions(-DCPPHTTPLIB_NO_EXCEPTIONS "
            "-DSIMDJSON_EXCEPTIONS=0)\n"
            "add_compile_options(-Wall -Wextra -fno-exceptions -fno-rtti)\n";
    if (tests)
      c_ss << "enable_testing()\n";
    c_ss << "add_subdirectory(src)\n";
    if (tests)
      c_ss << "add_subdirectory(tests)\n";
    files["CMakeLists.txt"] = c_ss.str();

    std::stringstream sc_ss;
    sc_ss << "set(SOURCES generated_server.cpp)\n";
    if (with_postgres)
      sc_ss << "find_package(PostgreSQL REQUIRED)\n";
    if (with_faker) {
      sc_ss << "include(FetchContent)\nFetchContent_Declare(faker_cxx URL "
               "https://github.com/cieslarmichal/faker-cxx/archive/refs/tags/"
               "v4.3.2.tar.gz)\n";
      sc_ss << "set(BUILD_TESTING OFF CACHE BOOL \"\" "
               "FORCE)\nset(FAKER_BUILD_TESTING OFF CACHE BOOL \"\" "
               "FORCE)\nFetchContent_MakeAvailable(faker_cxx)\n";
    }
    sc_ss << "include(FetchContent)\nFetchContent_Declare(cpphttplib URL "
             "https://github.com/yhirose/cpp-httplib/archive/refs/tags/"
             "v0.15.3.tar.gz)\nFetchContent_MakeAvailable(cpphttplib)\n";
    sc_ss << "add_executable(generated_bin ${SOURCES})\n";
    if (with_postgres)
      sc_ss << "target_link_libraries(generated_bin PostgreSQL::PostgreSQL)\n";
    if (with_faker)
      sc_ss << "target_link_libraries(generated_bin faker-cxx)\n";
    sc_ss << "target_link_libraries(generated_bin "
             "httplib::httplib)\ninstall(TARGETS "
             "generated_bin)\n";
    files["src/CMakeLists.txt"] = sc_ss.str();
  }

  if (tests) {
    std::stringstream tc_ss;
    tc_ss << "include(FetchContent)\nFetchContent_Declare(googletest URL "
             "https://github.com/google/googletest/archive/refs/tags/"
             "v1.14.0.tar.gz)\n";
    tc_ss << "FetchContent_MakeAvailable(googletest)\n";
    for (const auto &name : names) {
      tc_ss << "add_executable(" << name << "Test " << name
            << "Test.cpp)\ntarget_link_libraries(" << name
            << "Test gtest_main gmock";
      if (with_postgres)
        tc_ss << " PostgreSQL::PostgreSQL";
      tc_ss << ")\n";
    }
    tc_ss << "include(GoogleTest)\n";
    for (const auto &name : names)
      tc_ss << "gtest_discover_tests(" << name << "Test)\n";
    files["tests/CMakeLists.txt"] = tc_ss.str();
  }

  return files;
}

/// @brief Emit C++ server workflow engine to string from an Arazzo 1.1.0
/// specification.
std::string emit_arazzo(const arazzo::Arazzo &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n\n";
  ss << docstrings::emit_arazzo_docstrings(spec);
  ss << "namespace cdd_server {\n\n";
  ss << "    class WorkflowEngine {\n";
  ss << "    public:\n";
  ss << "        using WorkflowHandler = "
        "std::function<std::expected<std::string, std::string>(const "
        "std::string&)>;\n\n";
  for (const auto &wf : spec.workflows) {
    ss << docstrings::emit_workflow_docstrings(wf);
    ss << "        void on_workflow_" << wf.workflowId
       << "(WorkflowHandler handler) noexcept {\n";
    ss << "            workflows[\"" << wf.workflowId
       << "\"] = std::move(handler);\n";
    ss << "        }\n\n";
    ss << "        std::expected<std::string, std::string> execute_"
       << wf.workflowId
       << "(const std::string& inputs_json) const noexcept {\n";
    ss << "            auto it = workflows.find(\"" << wf.workflowId
       << "\");\n";
    ss << "            if (it != workflows.end() && it->second) {\n";
    ss << "                return it->second(inputs_json);\n";
    ss << "            }\n";
    ss << "            return std::string(\"{}\");\n";
    ss << "        }\n\n";
  }
  ss << "    private:\n";
  ss << "        std::map<std::string, WorkflowHandler> workflows;\n";
  ss << "    };\n";
  ss << "} // namespace cdd_server\n";
  return ss.str();
}

/// @brief Emit modular C++ server workflow orchestration files from an Arazzo
/// 1.1.0 specification.
std::map<std::string, std::string>
emit_arazzo_modular(const arazzo::Arazzo &spec, bool no_installable_package,
                    bool tests) noexcept {
  std::map<std::string, std::string> files;
  files["src/WorkflowEngine.hpp"] = emit_arazzo(spec);

  std::stringstream s_ss;
  s_ss << "#include <httplib.h>\n";
  s_ss << "#include \"WorkflowEngine.hpp\"\n\n";
  s_ss << "int main() noexcept {\n";
  s_ss << "    cdd_server::WorkflowEngine engine;\n";
  s_ss << "    httplib::Server svr;\n";
  for (const auto &wf : spec.workflows) {
    s_ss << "    svr.Post(\"/workflows/" << wf.workflowId
         << "\", [&](const httplib::Request& req, httplib::Response& res) {\n";
    s_ss << "        auto out = engine.execute_" << wf.workflowId
         << "(req.body);\n";
    s_ss << "        if (out.has_value()) res.set_content(*out, "
            "\"application/json\");\n";
    s_ss << "        else { res.status = 500; res.set_content(out.error(), "
            "\"text/plain\"); }\n";
    s_ss << "    });\n";
  }
  s_ss << "    svr.listen(\"0.0.0.0\", 8080);\n";
  s_ss << "    return 0;\n";
  s_ss << "}\n";
  files["src/workflow_server.cpp"] = s_ss.str();

  if (!no_installable_package) {
    files["CMakeLists.txt"] =
        "cmake_minimum_required(VERSION 3.15)\nproject(workflow_server "
        "LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 26)\n";
  }
  if (tests) {
    std::stringstream t_ss;
    t_ss
        << "#include <gtest/gtest.h>\n#include \"../src/WorkflowEngine.hpp\"\n";
    for (const auto &wf : spec.workflows) {
      t_ss << "TEST(WorkflowEngineTest, " << wf.workflowId << ") {\n";
      t_ss << "    cdd_server::WorkflowEngine engine;\n";
      t_ss << "    auto res = engine.execute_" << wf.workflowId
           << "(\"{}\");\n";
      t_ss << "    EXPECT_TRUE(res.has_value());\n";
      t_ss << "}\n";
    }
    files["tests/WorkflowEngineTest.cpp"] = t_ss.str();
  }
  return files;
}

} // namespace cdd_cpp::server
