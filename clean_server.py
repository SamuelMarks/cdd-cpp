import sys

with open("src/server/emit.cpp", "r") as f:
    content = f.read()

bad = """  ss << "        // MCP SSE Gateway\\n";
  ss << "        std::string handle_mcp_message(const std::string& request_body) {\\n";
  ss << "            return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":null,\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32601,\\\\\\"message\\\\\\":\\\\\\"Method not found\\\\\\"}}<END>\\\\n\\";\\n";
  ss << "        }\\n\\n";
  ss << "        // MCP Gateway Adapter\\n";
  ss << "        std::string handle_mcp_message(const std::string& request_body) {\\n";
  ss << "            return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":null,\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32601,\\\\\\"message\\\\\\":\\\\\\"Method not found\\\\\\"}}<END>\\\\n\\";\\n";
  ss << "        }\\n\\n";"""

good = """
  ss << "        // MCP Gateway Adapter\\n";
  ss << "        std::string handle_mcp_message(const std::string& request_body) {\\n";
  ss << "            simdjson::ondemand::parser parser;\\n";
  ss << "            simdjson::padded_string padded(request_body);\\n";
  ss << "            simdjson::ondemand::document doc;\\n";
  ss << "            auto error = parser.iterate(padded).get(doc);\\n";
  ss << "            if (error) return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32700,\\\\\\"message\\\\\\":\\\\\\"Parse error\\\\\\"}}<END>\\\\n\\";\\n";
  ss << "            std::string id_str = \\"null\\";\\n";
  ss << "            simdjson::ondemand::value id_val;\\n";
  ss << "            if (!doc[\\"id\\"].get(id_val)) {\\n";
  ss << "                simdjson::ondemand::json_type t;\\n";
  ss << "                if (!id_val.type().get(t)) {\\n";
  ss << "                    if (t == simdjson::ondemand::json_type::number) {\\n";
  ss << "                        int64_t v = 0;\\n";
  ss << "                        if (!id_val.get_int64().get(v)) id_str = std::to_string(v);\\n";
  ss << "                    } else if (t == simdjson::ondemand::json_type::string) {\\n";
  ss << "                        std::string_view sv;\\n";
  ss << "                        if (!id_val.get_string().get(sv)) id_str = \\"\\\\\\"\\" + std::string(sv) + \\"\\\\\\"\\";\\n";
  ss << "                    }\\n";
  ss << "                }\\n";
  ss << "            }\\n";
  ss << "            std::string method = \\"\\";\\n";
  ss << "            std::string_view method_v;\\n";
  ss << "            if (!doc[\\"method\\"].get(method_v)) method = method_v;\\n";
  ss << "            if (method == \\"initialize\\") {\\n";
  ss << "                return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" + id_str + \\",\\\\\\"result\\\\\\":{\\\\\\"protocolVersion\\\\\\":\\\\\\"2024-11-05\\\\\\",\\\\\\"capabilities\\\\\\":{\\\\\\"resources\\\\\\":{}},\\\\\\"serverInfo\\\\\\":{\\\\\\"name\\\\\\":\\\\\\"cdd_server\\\\\\",\\\\\\"version\\\\\\":\\\\\\"1.0.0\\\\\\"}}}<END>\\\\n\\";\\n";
  ss << "            } else if (method == \\"resources/list\\") {\\n";
  ss << "                return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" + id_str + \\",\\\\\\"result\\\\\\":{\\\\\\"resources\\\\\\":[{\\\\\\"uri\\\\\\":\\\\\\"cdd://openapi\\\\\\",\\\\\\"name\\\\\\":\\\\\\"OpenAPI Specification\\\\\\",\\\\\\"mimeType\\\\\\":\\\\\\"application/json\\\\\\"}]}}<END>\\\\n\\";\\n";
  ss << "            } else if (method == \\"resources/read\\") {\\n";
  ss << "                return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" + id_str + \\",\\\\\\"result\\\\\\":{\\\\\\"contents\\\\\\":[{\\\\\\"uri\\\\\\":\\\\\\"cdd://openapi\\\\\\",\\\\\\"mimeType\\\\\\":\\\\\\"application/json\\\\\\",\\\\\\"text\\\\\\":\\\\\\"{\\\\\\\\\\\\\\"openapi\\\\\\\\\\\\\\":\\\\\\\\\\\\\\"embedded\\\\\\\\\\\\\\"}\\\\\\"}]}}<END>\\\\n\\";\\n";
  ss << "            }\\n";
  ss << "            return \\"{\\\\\\"jsonrpc\\\\\\":\\\\\\"2.0\\\\\\",\\\\\\"id\\\\\\":\\" + id_str + \\",\\\\\\"error\\\\\\":{\\\\\\"code\\\\\\":-32601,\\\\\\"message\\\\\\":\\\\\\"Method not found\\\\\\"}}<END>\\\\n\\";\\n";
  ss << "        }\\n\\n";"""

content = content.replace(bad, good)

header_injection = """  ss << "#pragma once\\n";                        // GCOV_EXCL_BR_LINE
  ss << "#include <string>\\n";                   // GCOV_EXCL_BR_LINE
  ss << "#include <functional>\\n";               // GCOV_EXCL_BR_LINE
  ss << "#include <map>\\n";                    // GCOV_EXCL_BR_LINE
  ss << "#include <simdjson.h>\\n\\n";                    // GCOV_EXCL_BR_LINE"""

content = content.replace("""  ss << "#pragma once\\n";                        // GCOV_EXCL_BR_LINE
  ss << "#include <string>\\n";                   // GCOV_EXCL_BR_LINE
  ss << "#include <functional>\\n";               // GCOV_EXCL_BR_LINE
  ss << "#include <map>\\n\\n";                    // GCOV_EXCL_BR_LINE""", header_injection)


with open("src/server/emit.cpp", "w") as f:
    f.write(content)

print("Server resources adapter fixed.")
