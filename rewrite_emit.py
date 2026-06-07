import re

with open("src/client_sdk_cli/emit.cpp", "r") as f:
    content = f.read()

# We want to replace the mcp generate_exec logic
mcp_exec_start = content.find('if (n->is_mcp) {')
mcp_exec_end = content.find('} else if (n->method.has_value() && n->op.has_value()) {')

if mcp_exec_start != -1 and mcp_exec_end != -1:
    old_mcp_exec = content[mcp_exec_start:mcp_exec_end]

    new_mcp_exec = """if (n->is_mcp) {
      ss << "        case " << node_ids[n] << ": {\\n";
      ss << "            std::string line;\\n";
      ss << "            bool initialized = false;\\n";
      ss << "            while (std::getline(std::cin, line)) {\\n";
      ss << "                if (line.empty()) continue;\\n";
      ss << "                simdjson::ondemand::parser parser;\\n";
      ss << "                simdjson::padded_string padded(line);\\n";
      ss << "                simdjson::ondemand::document doc;\\n";
      ss << "                auto error = parser.iterate(padded).get(doc);\\n";
      ss << "                if (error) {\\n";
      ss << "                    std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32700,\\\\\\"message\\\\\\":\\\\\\"Parse error\\\\\\"}}<END>\\\\n\\";\\n";
      ss << "                    continue;\\n";
      ss << "                }\\n";
      ss << "                std::string id_str = \\"null\\";\\n";
      ss << "                simdjson::ondemand::value id_val;\\n";
      ss << "                if (!doc[\\"id\\"].get(id_val)) {\\n";
      ss << "                    if (id_val.type() == simdjson::ondemand::json_type::number) id_str = std::to_string(id_val.get_int64().value_or(0));\\n";
      ss << "                    else if (id_val.type() == simdjson::ondemand::json_type::string) id_str = \\"\\\\\\"\\" + std::string(id_val.get_string().value_or(\\"\\")) + \\"\\\\\\"\\";\\n";
      ss << "                }\\n";
      ss << "                std::string method = \\"\\";\\n";
      ss << "                std::string_view method_v;\\n";
      ss << "                if (!doc[\\"method\\"].get(method_v)) method = method_v;\\n";
      ss << "                if (method == \\"initialize\\") {\\n";
      ss << "                    std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"result\\\\\\":{\\\\\\"protocolVersion\\\\\\":\\\\\\"2024-11-05\\\\\\",\\\\\\"capabilities\\\\\\":{\\\\\\"tools\\\\\\":{}},\\\\\\"serverInfo\\\\\\":{\\\\\\"name\\\\\\":\\\\\\"cdd_cli\\\\\\",\\\\\\"version\\\\\\":\\\\\\"1.0.0\\\\\\"}}}<END>\\\\n\\";\\n";
      ss << "                    initialized = true;\\n";
      ss << "                } else if (method == \\"notifications/initialized\\") {\\n";
      ss << "                    // Do nothing\\n";
      ss << "                } else if (method == \\"tools/list\\") {\\n";
      ss << "                    std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"result\\\\\\":{\\\\\\"tools\\\\\\":[\\";\\n";
      
      // Here we inject tools json
      bool first_tool = true;
      auto generate_tools_json = [&](auto& self, CliNode* cn) -> void {
          if (cn->method.has_value() && cn->op.has_value()) {
              std::string op_id = cn->op->operationId.value_or("op_" + std::to_string(node_ids[cn]));
              sanitize_string(op_id);
              
              if (!first_tool) ss << "                        std::cout << \\",\\";\\n";
              first_tool = false;
              
              std::string desc = cn->op->description.value_or("");
              if (desc.empty()) desc = cn->op->summary.value_or("");
              if (desc.empty()) desc = op_id;
              
              ss << "                    std::cout << \\"{\\\\\\"name\\\\\\":\\\\\\"\\" << \\"" << escape_string(op_id) << "\\" << \\"\\\\\\",\\\\\\"description\\\\\\":\\\\\\"\\" << \\"" << escape_string(desc) << "\\" << \\"\\\\\\",\\\\\\"inputSchema\\\\\\":{\\\\\\"type\\\\\\":\\\\\\"object\\\\\\",\\\\\\"properties\\\\\\":{\\";\\n";
              
              bool first_param = true;
              if (cn->op->parameters.has_value()) {
                  for (const auto& p : cn->op->parameters.value()) {
                      if (!first_param) ss << "                        std::cout << \\",\\";\\n";
                      first_param = false;
                      ss << "                        std::cout << \\"\\\\\\"\\" << \\"" << escape_string(p.name) << "\\" << \\"\\\\\\":{\\\\\\"type\\\\\\":\\\\\\"string\\\\\\",\\\\\\"description\\\\\\":\\\\\\"\\" << \\"" << escape_string(p.description.value_or("")) << "\\" << \\"\\\\\\"}\\";\\n";
                  }
              }
              if (cn->op->requestBody.has_value()) {
                  if (!first_param) ss << "                        std::cout << \\",\\";\\n";
                  first_param = false;
                  ss << "                        std::cout << \\"\\\\\\"body\\\\\\":{\\\\\\"type\\\\\\":\\\\\\"string\\\\\\",\\\\\\"description\\\\\\":\\\\\\"JSON body\\\\\\"}\\";\\n";
              }
              
              ss << "                    std::cout << \\"},\\\\\\"required\\\\\\":[\\";\\n";
              
              bool first_req = true;
              if (cn->op->parameters.has_value()) {
                  for (const auto& p : cn->op->parameters.value()) {
                      if (p.required) {
                          if (!first_req) ss << "                        std::cout << \\",\\";\\n";
                          first_req = false;
                          ss << "                        std::cout << \\"\\\\\\"\\" << \\"" << escape_string(p.name) << "\\" << \\"\\\\\\"\\";\\n";
                      }
                  }
              }
              if (cn->op->requestBody.has_value()) {
                  if (!first_req) ss << "                        std::cout << \\",\\";\\n";
                  first_req = false;
                  ss << "                        std::cout << \\"\\\\\\"body\\\\\\"\\";\\n";
              }
              
              ss << "                    std::cout << \\"]}}\\";\\n";
          }
          for (auto &[name, child] : cn->children) {
              self(self, child.get());
          }
      };
      generate_tools_json(generate_tools_json, root.get());
      
      ss << "                    std::cout << \\"]}}<END>\\\\n\\";\\n";
      ss << "                } else if (method == \\"tools/call\\") {\\n";
      ss << "                    std::string tool_name = \\"\\";\\n";
      ss << "                    simdjson::ondemand::value name_val;\\n";
      ss << "                    if (!doc[\\"params\\"][\\"name\\"].get(name_val)) {\\n";
      ss << "                        std::string_view sv;\\n";
      ss << "                        if (!name_val.get(sv)) tool_name = sv;\\n";
      ss << "                    }\\n";
      
      ss << "                    simdjson::ondemand::value args_val;\\n";
      ss << "                    bool has_args = !doc[\\"params\\"][\\"arguments\\"].get(args_val);\\n";
      
      auto generate_tools_call = [&](auto& self, CliNode* cn) -> void {
          if (cn->method.has_value() && cn->op.has_value()) {
              std::string op_id = cn->op->operationId.value_or("op_" + std::to_string(node_ids[cn]));
              sanitize_string(op_id);
              
              ss << "                    if (tool_name == \\"" << escape_string(op_id) << "\\") {\\n";
              
              if (cn->op->parameters.has_value()) {
                  for (const auto& p : cn->op->parameters.value()) {
                      std::string pname = p.name;
                      sanitize_string(pname);
                      ss << "                        std::string arg_" << pname << " = \\"\\";\\n";
                      ss << "                        if (has_args) {\\n";
                      ss << "                            simdjson::ondemand::value pval;\\n";
                      ss << "                            if (!args_val[\\"" << escape_string(p.name) << "\\"].get(pval)) {\\n";
                      ss << "                                std::string_view sv;\\n";
                      ss << "                                if (!pval.get(sv)) arg_" << pname << " = sv;\\n";
                      ss << "                            }\\n";
                      ss << "                        }\\n";
                  }
              }
              if (cn->op->requestBody.has_value()) {
                  ss << "                        std::string arg_body = \\"\\";\\n";
                  ss << "                        if (has_args) {\\n";
                  ss << "                            simdjson::ondemand::value pval;\\n";
                  ss << "                            if (!args_val[\\"body\\"].get(pval)) {\\n";
                  ss << "                                std::string_view sv;\\n";
                  ss << "                                if (!pval.get(sv)) arg_body = sv;\\n";
                  ss << "                            }\\n";
                  ss << "                        }\\n";
              }
              
              ss << "                        auto res = handle_" << op_id << "(client";
              if (cn->op->parameters.has_value()) {
                  for (const auto& p : cn->op->parameters.value()) {
                      std::string pname = p.name;
                      sanitize_string(pname);
                      ss << ", arg_" << pname;
                  }
              }
              if (cn->op->requestBody.has_value()) {
                  ss << ", arg_body";
              }
              ss << ");\\n";
              
              ss << "                        if (!res) {\\n";
              ss << "                            std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"result\\\\\\":{\\\\\\"isError\\\\\\":true,\\\\\\"content\\\\\\":[{\\\\\\"type\\\\\\":\\\\\\"text\\\\\\",\\\\\\"text\\\\\\":\\\\\\"\\" << escape_string(res.error()) << \\"\\\\\\"}]}}<END>\\\\n\\";\\n";
              ss << "                        } else {\\n";
              ss << "                            std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"result\\\\\\":{\\\\\\"content\\\\\\":[{\\\\\\"type\\\\\\":\\\\\\"text\\\\\\",\\\\\\"text\\\\\\":\\\\\\"\\" << escape_string(*res) << \\"\\\\\\"}]}}<END>\\\\n\\";\\n";
              ss << "                        }\\n";
              ss << "                        continue;\\n";
              ss << "                    }\\n";
          }
          for (auto &[name, child] : cn->children) {
              self(self, child.get());
          }
      };
      generate_tools_call(generate_tools_call, root.get());
      
      ss << "                    std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32601,\\\\\\"message\\\\\\":\\\\\\"Tool not found\\\\\\"}}<END>\\\\n\\";\\n";
      ss << "                } else {\\n";
      ss << "                    std::cout << \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" << id_str << \\",\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32601,\\\\\\"message\\\\\\":\\\\\\"Method not found\\\\\\"}}<END>\\\\n\\";\\n";
      ss << "                }\\n";
      ss << "            }\\n";
      ss << "            break;\\n";
      ss << "        }\\n    " """

    content = content.replace(old_mcp_exec, new_mcp_exec)
    with open("src/client_sdk_cli/emit.cpp", "w") as f:
        f.write(content)
    print("Patched successfully.")
else:
    print("Failed to find boundaries.", mcp_exec_start, mcp_exec_end)

