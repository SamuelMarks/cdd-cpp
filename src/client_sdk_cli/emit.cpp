// GCOV_EXCL_BR_START

#include "../docstrings/emit.hpp"
#include "../mocks/emit.hpp"
#include "emit.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>

namespace cdd_cpp::client_sdk_cli {

struct CliNode {
  std::string name;
  std::map<std::string, std::shared_ptr<CliNode>> children;
  std::optional<std::string> method;
  std::optional<std::string> path;
  std::optional<openapi::Operation> op;
  bool is_mcp = false;

  std::shared_ptr<CliNode>
  get_or_create_child(const std::string &child_name) noexcept {
    if (!children.count(child_name)) {
      children[child_name] = std::make_shared<CliNode>();
      children[child_name]->name = child_name;
    }
    return children[child_name];
  }
};

static void sanitize_string(std::string &str) noexcept {
  std::replace(str.begin(), str.end(), '-', '_');
  std::replace(str.begin(), str.end(), ' ', '_');
}

static std::string escape_string(const std::string &s) noexcept {
  std::string out;
  for (char c : s) {
    if (c == '"')
      out += "\\\"";
    else if (c == '\\')
      out += "\\\\";
    else if (c == '\n')
      out += "\\n";
    else if (c == '\r')
      out += "\\r";
    else
      out += c;
  }
  return out;
}

std::map<std::string, std::string> emit_cli(const openapi::OpenAPI &spec,
                                            bool no_github_actions,
                                            bool no_installable_package,
                                            bool tests) noexcept {
  std::stringstream ss;

  ss << "#include <expected>\n";
  ss << "#include <iostream>\n";
  ss << "#include <string>\n";
  ss << "#include <vector>\n";
  ss << "#include <map>\n";
  ss << "#include <optional>\n";
  ss << "#include <curl/curl.h>\n";
  ss << "#include <simdjson.h>\n\n";

  ss << "using namespace simdjson;\n\n";
  ss << "static std::string escape_string(const std::string& s) {\n";
  ss << "    std::string out;\n";
  ss << "    for (char c : s) {\n";
  ss << "        if (c == '\\\"') out += \"\\\\\\\"\";\n";
  ss << "        else if (c == '\\n') out += \"\\\\n\";\n";
  ss << "        else if (c == '\\r') out += \"\\\\r\";\n";
  ss << "        else if (c == '\\\\') out += \"\\\\\\\\\";\n";
  ss << "        else out += c;\n";
  ss << "    }\n";
  ss << "    return out;\n";
  ss << "}\n\n";
  ss << "namespace cdd_cli {\n\n";
  ss << docstrings::emit_api_docstrings(spec);
  ss << "    class Client {\n";
  ss << "        std::string base_url;\n";
  ss << "        CURL* curl;\n\n";
  ss << "        static size_t WriteCallback(void* contents, size_t size, "
        "size_t nmemb, void* userp) {\n";
  ss << "            ((std::string*)userp)->append((char*)contents, size * "
        "nmemb);\n";
  ss << "            return size * nmemb;\n";
  ss << "        }\n\n";

  ss << "    public:\n";
  std::string default_url = "";
  if (spec.servers.has_value() && !spec.servers->empty()) {
    default_url = spec.servers.operator*().front().url;
  }

  ss << "        Client(const std::string& url = \""
     << escape_string(default_url) << "\") : base_url(url) {\n";
  ss << "            curl_global_init(CURL_GLOBAL_DEFAULT);\n";
  ss << "            curl = curl_easy_init();\n";
  ss << "        }\n\n";

  ss << "        ~Client() {\n";
  ss << "            if(curl) curl_easy_cleanup(curl);\n";
  ss << "            curl_global_cleanup();\n";
  ss << "        }\n\n";

  ss << "        std::expected<std::string, std::string> request(const "
        "std::string& method, const "
        "std::string& path, const std::string& body = \"\") noexcept {\n";
  ss << "            if (!curl) return std::unexpected(\"Curl not "
        "initialized\");\n";
  ss << "            std::string readBuffer;\n";
  ss << "            std::string full_url = base_url + path;\n";
  ss << "            curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());\n";
  ss << "            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "
        "method.c_str());\n";
  ss << "            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "
        "WriteCallback);\n";
  ss << "            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);\n";
  ss << "            struct curl_slist *headers = NULL;\n";
  ss << "            if (!body.empty()) {\n";
  ss << "                headers = curl_slist_append(headers, \"Content-Type: "
        "application/json\");\n";
  ss << "                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "
        "headers);\n";
  ss << "                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "
        "body.c_str());\n";
  ss << "            }\n";
  ss << "            CURLcode res = curl_easy_perform(curl);\n";
  ss << "            if(headers) curl_slist_free_all(headers);\n";
  ss << "            if(res != CURLE_OK) {\n";
  ss << "                return std::unexpected(curl_easy_strerror(res));\n";
  ss << "            }\n";
  ss << "            return readBuffer;\n";
  ss << "        }\n";
  ss << "    };\n";
  ss << "}\n\n";

  // Build the tree
  auto root = std::make_shared<CliNode>();
  root->name = "root";

  auto mcp_node = root->get_or_create_child("mcp");
  mcp_node->is_mcp = true;

  if (spec.paths.has_value()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      std::vector<std::string> segments;
      std::stringstream pss(path);
      std::string token;
      while (std::getline(pss, token, '/')) {
        if (!token.empty())
          segments.push_back(token);
      }

      auto add_op = [&](const std::string &method,
                        const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        auto current = root;
        for (const auto &seg : segments) {
          current = current->get_or_create_child(seg);
        }
        std::string m = method;
        for (auto &c : m)
          c = std::tolower(c);
        auto leaf = current->get_or_create_child(m);
        leaf->method = method;
        leaf->path = path;
        leaf->op = op;
      };

      add_op("GET", item.get);
      add_op("POST", item.post);
      add_op("PUT", item.put);
      add_op("DELETE", item.delete_op);
      add_op("PATCH", item.patch);
    }
  }

  // Generator logic
  int node_id_counter = 0;
  std::map<CliNode *, int> node_ids;

  auto assign_ids = [&](auto &self, CliNode *n) -> void {
    node_ids[n] = node_id_counter++;
    for (auto &[name, child] : n->children) {
      self(self, child.get());
    }
  };
  assign_ids(assign_ids, root.get());

  ss << "void print_help(int node_id) {\n";
  ss << "    switch (node_id) {\n";
  auto generate_help = [&](auto &self, CliNode *n) -> void {
    ss << "        case " << node_ids[n] << ": {\n";
    ss << "            std::cout << \"Usage: cli [options] "
          "<command>\\n\\n\";\n";
    if (n->op.has_value()) {
      if (n->op->summary.has_value()) {
        ss << "            std::cout << \"Summary: "
           << escape_string(n->op->summary.operator*()) << "\\n\";\n";
      }
      if (n->op->description.has_value()) {
        ss << "            std::cout << \"Description: "
           << escape_string(n->op->description.operator*()) << "\\n\";\n";
      }
      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.operator*()) {
          ss << "            std::cout << \"  --" << escape_string(p.name)
             << "\\n\";\n";
        }
      }
      if (n->op->requestBody.has_value()) {
        ss << "            std::cout << \"  --body\\n\";\n";
      }
    }
    if (!n->children.empty()) {
      ss << "            std::cout << \"Commands:\\n\";\n";
      for (auto &[name, child] : n->children) {
        ss << "            std::cout << \"  " << escape_string(name)
           << "\\n\";\n";
      }
    }
    ss << "            break;\n";
    ss << "        }\n";
    for (auto &[name, child] : n->children) {
      self(self, child.get());
    }
  };
  generate_help(generate_help, root.get());
  ss << "    }\n";
  ss << "}\n\n";

  // Generate explicit handler functions for each leaf node
  auto generate_handlers = [&](auto &self, CliNode *n) -> void {
    if (n->is_mcp) {
      // MCP node doesn't need a global handler function here.
    } else if (n->method.has_value() && n->op.has_value()) {
      std::string op_id =
          n->op->operationId.value_or("op_" + std::to_string(node_ids[n]));
      sanitize_string(op_id);

      ss << docstrings::emit_operation_docstrings(n->op.operator*());
      ss << "/// @route " << n->method.operator*() << " "
         << escape_string(n->path.operator*()) << "\n";

      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.operator*()) {
          if (p.description.has_value()) {
            ss << "/// @param " << p.name << " "
               << escape_string(p.description.operator*()) << "\n";
          } else {
            ss << "/// @param " << p.name << " " << p.in << " parameter\n";
          }
          if (p.example.has_value()) {
            ss << "/// @param_example " << p.name << " "
               << escape_string(p.example.operator*()) << "\n";
          }
          if (p.deprecated) {
            ss << "/// @param_deprecated " << p.name << "\n";
          }
        }
      }
      ss << "std::expected<std::string, std::string> handle_" << op_id
         << "(cdd_cli::Client& client";

      bool has_body = n->op->requestBody.has_value();
      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.operator*()) {
          std::string pname = p.name;
          sanitize_string(pname);
          ss << ", std::string " << pname;
        }
      }
      if (has_body) {
        ss << ", std::string body";
      }
      ss << ") noexcept {\n";

      ss << "    std::string path = \"" << escape_string(n->path.operator*())
         << "\";\n";
      ss << "    std::string qs = \"\";\n";

      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.operator*()) {
          std::string pname = p.name;
          sanitize_string(pname);
          if (p.in == "path") {
            ss << "    {\n";
            ss << "        size_t pos = path.find(\"{" << escape_string(p.name)
               << "}\");\n";
            ss << "        if (pos != std::string::npos) {\n";
            ss << "            path.replace(pos, " << p.name.size() + 2 << ", "
               << pname << ");\n";
            ss << "        }\n";
            ss << "    }\n";
          } else if (p.in == "query") {
            ss << "    if (!" << pname << ".empty()) {\n";
            ss << "        qs += (qs.empty() ? \"?\" : \"&\") + std::string(\""
               << escape_string(p.name) << "=\") + " << pname << ";\n";
            ss << "    }\n";
          }
        }
      }

      // To support generic unlisted query parameters that the user passes:
      // Since this is statically typed, we'll just ignore unlisted params for
      // now in the explicitly typed handler, or we could add a
      // `std::map<std::string, std::string> extra_params` but that complicates
      // `to_openapi`. Better to stick to explicitly typed params so that
      // `to_openapi` works smoothly.

      ss << "    path += qs;\n";
      ss << "    return client.request(\""
         << escape_string(n->method.operator*()) << "\", path, "
         << (has_body ? "body" : "\"\"") << ");\n";
      ss << "}\n\n";
    }
    for (auto &[name, child] : n->children) {
      self(self, child.get());
    }
  };
  generate_handlers(generate_handlers, root.get());

  ss << "int main(int argc, char** argv) {\n";

  ss << "    std::vector<std::string> args;\n";
  ss << "    for (int i = 1; i < argc; ++i) args.push_back(argv[i]);\n";
  ss << "    std::map<std::string, std::string> path_params;\n";
  ss << "    std::map<std::string, std::string> query_params;\n";
  ss << "    std::string body;\n";
  ss << "    cdd_cli::Client client;\n";
  ss << "    int idx = 0;\n";

  ss << "    int current_node = 0;\n";
  ss << "    while (idx < args.size()) {\n";
  ss << "        std::string arg = args[idx];\n";
  ss << "        if (arg == \"--help\" || arg == \"-h\") { "
        "print_help(current_node); return 0; }\n";

  ss << "        if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {\n";
  ss << "            size_t eq = arg.find('=');\n";
  ss << "            if (eq != std::string::npos) {\n";
  ss << "                query_params[arg.substr(2, eq - 2)] = arg.substr(eq + "
        "1);\n";
  ss << "            } else if (idx + 1 < args.size()) {\n";
  ss << "                query_params[arg.substr(2)] = args[++idx];\n";
  ss << "            }\n";
  ss << "            idx++;\n";
  ss << "            continue;\n";
  ss << "        }\n";

  ss << "        switch (current_node) {\n";

  auto generate_node = [&](auto &self, CliNode *n) -> void {
    int nid = node_ids[n];
    ss << "            case " << nid << ": {\n";

    bool has_param_child = false;
    std::string param_child_name = "";
    int param_child_id = -1;

    ss << "                ";
    bool first = true;
    for (auto &[name, child] : n->children) {
      if (!name.empty() && name[0] == '{' && name.back() == '}') {
        has_param_child = true;
        param_child_name = name.substr(1, name.size() - 2);
        param_child_id = node_ids[child.get()];
        continue;
      }
      if (!first)
        ss << " else ";
      ss << "if (arg == \"" << escape_string(name) << "\") {\n";
      ss << "                    current_node = " << node_ids[child.get()]
         << ";\n";
      ss << "                }";
      first = false;
    }

    if (has_param_child) {
      if (!first)
        ss << " else {\n";
      else
        ss << "{\n";
      ss << "                    path_params[\""
         << escape_string(param_child_name) << "\"] = arg;\n";
      ss << "                    current_node = " << param_child_id << ";\n";
      ss << "                }\n";
    } else {
      if (!first)
        ss << " else {\n";
      else
        ss << "{\n";
      ss << "                    std::cerr << \"Unknown command: \" << arg << "
            "\"\\n\";\n";
      ss << "                    return 1;\n";
      ss << "                }\n";
    }

    ss << "                break;\n";
    ss << "            }\n";

    for (auto &[name, child] : n->children) {
      self(self, child.get());
    }
  };

  generate_node(generate_node, root.get());

  ss << "        }\n"; // end switch
  ss << "        idx++;\n";
  ss << "    }\n"; // end while

  // Execute the final leaf node
  ss << "    switch (current_node) {\n";
  auto generate_exec = [&](auto &self, CliNode *n) -> void {
    if (n->is_mcp) {
      ss << "        case " << node_ids[n] << ": {\n";
      ss << "            std::string line;\n";
      ss << "            bool initialized = false;\n";
      ss << "            while (std::getline(std::cin, line)) {\n";
      ss << "                if (line.empty()) continue;\n";
      ss << "                simdjson::ondemand::parser parser;\n";
      ss << "                simdjson::padded_string padded(line);\n";
      ss << "                simdjson::ondemand::document doc;\n";
      ss << "                auto error = parser.iterate(padded).get(doc);\n";
      ss << "                if (error) {\n";
      ss << "                    std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"error\\\":{\\\"code\\\":-32700,"
            "\\\"message\\\":\\\"Parse error\\\"}}<END>\\n\";\n";
      ss << "                    continue;\n";
      ss << "                }\n";
      ss << "                std::string id_str = \"null\";\n";
      ss << "                simdjson::ondemand::value id_val;\n";
      ss << "                if (!doc[\"id\"].get(id_val)) {\n";
      ss << "                    if (id_val.type() == "
            "simdjson::ondemand::json_type::number) { int64_t v=0; "
            "id_val.get(v); id_str = std::to_string(v); }\n";
      ss << "                    else if (id_val.type() == "
            "simdjson::ondemand::json_type::string) { std::string_view v; "
            "id_val.get(v); id_str = \"\\\"\" + std::string(v) + \"\\\"\"; }\n";
      ss << "                }\n";
      ss << "                std::string_view jsonrpc_v;\n";
      ss << "                if (doc[\"jsonrpc\"].get(jsonrpc_v) || jsonrpc_v "
            "!= \"2.0\") {\n";
      ss << "                    std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32600,\\\"message\\\":\\\"Invalid "
            "Request\\\"}}<END>\\n\";\n";
      ss << "                    continue;\n";
      ss << "                }\n";
      ss << "                bool is_notification = (id_str == \"null\" && "
            "doc[\"id\"].error() != simdjson::SUCCESS);\n";
      ss << "                std::string method = \"\";\n";
      ss << "                std::string_view method_v;\n";
      ss << "                if (!doc[\"method\"].get(method_v)) method = "
            "method_v;\n";
      ss << "                if (method == \"initialize\") {\n";
      ss << "                    std::string_view protocol_version;\n";
      ss << "                    if "
            "(doc[\"params\"][\"protocolVersion\"].get(protocol_version)) {\n";
      ss << "                        if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32602,\\\"message\\\":\\\"Missing "
            "protocolVersion\\\"}}<END>\\n\";\n";
      ss << "                        continue;\n";
      ss << "                    }\n";
      ss << "                    std::string_view client_name;\n";
      ss << "                    if "
            "(doc[\"params\"][\"clientInfo\"][\"name\"].get(client_name)) {\n";
      ss << "                        if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32602,\\\"message\\\":\\\"Missing "
            "clientInfo.name\\\"}}<END>\\n\";\n";
      ss << "                        continue;\n";
      ss << "                    }\n";
      ss << "                    std::string_view client_version;\n";
      ss << "                    if "
            "(doc[\"params\"][\"clientInfo\"][\"version\"].get(client_version))"
            " {\n";
      ss << "                        if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32602,\\\"message\\\":\\\"Missing "
            "clientInfo.version\\\"}}<END>\\n\";\n";
      ss << "                        continue;\n";
      ss << "                    }\n";
      ss << "                    if (doc[\"params\"][\"capabilities\"].type() "
            "!= simdjson::ondemand::json_type::object) {\n";
      ss << "                        if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32602,\\\"message\\\":\\\"Missing "
            "or invalid capabilities\\\"}}<END>\\n\";\n";
      ss << "                        continue;\n";
      ss << "                    }\n";
      ss << "                    if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"result\\\":{\\\"protocolVersion\\\":\\\"2024-11-05\\\","
            "\\\"capabilities\\\":{\\\"tools\\\":{}},\\\"serverInfo\\\":{"
            "\\\"name\\\":\\\"cdd_cli\\\",\\\"version\\\":\\\"1.0.0\\\"}}}<END>"
            "\\n\";\n";
      ss << "                    initialized = true;\n";
      ss << "                } else if (method == "
            "\"notifications/initialized\") {\n";
      ss << "                    // Do nothing\n";
      ss << "                } else if (method == \"notifications/cancelled\") "
            "{\n";
      ss << "                    // Do nothing for synchronous CLI\n";
      ss << "                } else if (method == \"ping\") {\n";
      ss << "                    if (!is_notification) std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"result\\\":{}}<END>\\n\";\n";
      ss << "                } else if (method == \"tools/list\") {\n";
      ss << "                    if (is_notification) continue;\n";
      ss << "                    std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"result\\\":{\\\"tools\\\":[\";\n";

      // Here we inject tools json
      bool first_tool = true;
      auto generate_tools_json = [&](auto &self, CliNode *cn) -> void {
        if (cn->method.has_value() && cn->op.has_value()) {
          std::string op_id = cn->op->operationId.value_or(
              "op_" + std::to_string(node_ids[cn]));
          sanitize_string(op_id);

          if (!first_tool)
            ss << "                        std::cout << \",\";\n";
          first_tool = false;

          std::string desc = cn->op->description.value_or("");
          if (desc.empty())
            desc = cn->op->summary.value_or("");
          if (desc.empty())
            desc = op_id;

          ss << "                    std::cout << \"{\\\"name\\\":\\\"\" << \""
             << escape_string(op_id)
             << "\" << \"\\\",\\\"description\\\":\\\"\" << \""
             << escape_string(desc)
             << "\" << "
                "\"\\\",\\\"inputSchema\\\":{\\\"type\\\":\\\"object\\\","
                "\\\"properties\\\":{\";\n";

          bool first_param = true;
          if (cn->op->parameters.has_value()) {
            for (const auto &p : cn->op->parameters.operator*()) {
              if (!first_param)
                ss << "                        std::cout << \",\";\n";
              first_param = false;
              ss << "                        std::cout << \"\\\"\" << \""
                 << escape_string(p.name)
                 << "\" << "
                    "\"\\\":{\\\"type\\\":\\\"string\\\",\\\"description\\\":"
                    "\\\"\" << \""
                 << escape_string(p.description.value_or(""))
                 << "\" << \"\\\"}\";\n";
            }
          }
          if (cn->op->requestBody.has_value()) {
            if (!first_param)
              ss << "                        std::cout << \",\";\n";
            first_param = false;
            ss << "                        std::cout << "
                  "\"\\\"body\\\":{\\\"type\\\":\\\"string\\\","
                  "\\\"description\\\":\\\"JSON body\\\"}\";\n";
          }

          ss << "                    std::cout << \"},\\\"required\\\":[\";\n";

          bool first_req = true;
          if (cn->op->parameters.has_value()) {
            for (const auto &p : cn->op->parameters.operator*()) {
              if (p.required) {
                if (!first_req)
                  ss << "                        std::cout << \",\";\n";
                first_req = false;
                ss << "                        std::cout << \"\\\"\" << \""
                   << escape_string(p.name) << "\" << \"\\\"\";\n";
              }
            }
          }
          if (cn->op->requestBody.has_value()) {
            if (!first_req)
              ss << "                        std::cout << \",\";\n";
            first_req = false;
            ss << "                        std::cout << \"\\\"body\\\"\";\n";
          }

          ss << "                    std::cout << \"]}}\";\n";
        }
        for (auto &[name, child] : cn->children) {
          self(self, child.get());
        }
      };
      generate_tools_json(generate_tools_json, root.get());

      ss << "                    std::cout << \"]}}<END>\\n\";\n";
      ss << "                } else if (method == \"tools/call\") {\n";
      ss << "                    std::string tool_name = \"\";\n";
      ss << "                    simdjson::ondemand::value name_val;\n";
      ss << "                    if (!doc[\"params\"][\"name\"].get(name_val)) "
            "{\n";
      ss << "                        std::string_view sv;\n";
      ss << "                        if (!name_val.get(sv)) tool_name = sv;\n";
      ss << "                    }\n";

      ss << "                    simdjson::ondemand::value args_val;\n";
      ss << "                    bool has_args = "
            "!doc[\"params\"][\"arguments\"].get(args_val);\n";

      auto generate_tools_call = [&](auto &self, CliNode *cn) -> void {
        if (cn->method.has_value() && cn->op.has_value()) {
          std::string op_id = cn->op->operationId.value_or(
              "op_" + std::to_string(node_ids[cn]));
          sanitize_string(op_id);

          ss << "                    if (tool_name == \""
             << escape_string(op_id) << "\") {\n";

          if (cn->op->parameters.has_value()) {
            for (const auto &p : cn->op->parameters.operator*()) {
              std::string pname = p.name;
              sanitize_string(pname);
              ss << "                        std::string arg_" << pname
                 << " = \"\";\n";
              ss << "                        if (has_args) {\n";
              ss << "                            simdjson::ondemand::value "
                    "pval;\n";
              ss << "                            if (!args_val[\""
                 << escape_string(p.name) << "\"].get(pval)) {\n";
              ss << "                                std::string_view sv;\n";
              ss << "                                if (!pval.get(sv)) arg_"
                 << pname << " = sv;\n";
              ss << "                            }\n";
              ss << "                        }\n";
            }
          }
          if (cn->op->requestBody.has_value()) {
            ss << "                        std::string arg_body = \"\";\n";
            ss << "                        if (has_args) {\n";
            ss << "                            simdjson::ondemand::value "
                  "pval;\n";
            ss << "                            if "
                  "(!args_val[\"body\"].get(pval)) {\n";
            ss << "                                std::string_view sv;\n";
            ss << "                                if (!pval.get(sv)) arg_body "
                  "= sv;\n";
            ss << "                            }\n";
            ss << "                        }\n";
          }

          ss << "                        auto res = handle_" << op_id
             << "(client";
          if (cn->op->parameters.has_value()) {
            for (const auto &p : cn->op->parameters.operator*()) {
              std::string pname = p.name;
              sanitize_string(pname);
              ss << ", arg_" << pname;
            }
          }
          if (cn->op->requestBody.has_value()) {
            ss << ", arg_body";
          }
          ss << ");\n";

          ss << "                        if (is_notification) continue;\n";
          ss << "                        if (!res) {\n";
          ss << "                            std::cout << "
                "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
                "\",\\\"result\\\":{\\\"isError\\\":true,\\\"content\\\":[{"
                "\\\"type\\\":\\\"text\\\",\\\"text\\\":\\\"\" << "
                "escape_string(res.error()) << \"\\\"}]}}<END>\\n\";\n";
          ss << "                        } else {\n";
          ss << "                            std::cout << "
                "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
                "\",\\\"result\\\":{\\\"content\\\":[{\\\"type\\\":"
                "\\\"text\\\",\\\"text\\\":\\\"\" << escape_string(*res) << "
                "\"\\\"}]}}<END>\\n\";\n";
          ss << "                        }\n";
          ss << "                        continue;\n";
          ss << "                    }\n";
        }
        for (auto &[name, child] : cn->children) {
          self(self, child.get());
        }
      };
      generate_tools_call(generate_tools_call, root.get());

      ss << "                    if (is_notification) continue;\n";
      ss << "                    std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32601,\\\"message\\\":\\\"Tool "
            "not found\\\"}}<END>\\n\";\n";
      ss << "                } else {\n";
      ss << "                    if (is_notification) continue;\n";
      ss << "                    std::cout << "
            "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":\" << id_str << "
            "\",\\\"error\\\":{\\\"code\\\":-32601,\\\"message\\\":\\\"Method "
            "not found\\\"}}<END>\\n\";\n";
      ss << "                }\n";
      ss << "            }\n";
      ss << "            break;\n";
      ss << "        }\n";
    } else if (n->method.has_value() && n->op.has_value()) {
      std::string op_id =
          n->op->operationId.value_or("op_" + std::to_string(node_ids[n]));
      sanitize_string(op_id);

      ss << "        case " << node_ids[n] << ": {\n";

      ss << "            auto res = handle_" << op_id << "(client";

      bool has_body = n->op->requestBody.has_value();
      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.operator*()) {
          std::string pname = p.name;
          sanitize_string(pname);
          if (p.in == "path") {
            ss << ", path_params[\"" << escape_string(p.name) << "\"]";
          } else if (p.in == "query") {
            ss << ", query_params[\"" << escape_string(p.name) << "\"]";
          } else {
            // Other param types (header, cookie) not currently handled in CLI
            // args, pass empty
            ss << ", \"\"";
          }
        }
      }
      if (has_body) {
        ss << ", body";
      }

      ss << ");\n";
      ss << "            if (!res) {\n";
      ss << "                std::cerr << \"Request failed: \" << res.error() "
            "<< \"\\n\";\n";
      ss << "                return 1;\n";
      ss << "            }\n";
      ss << "            std::cout << *res << \"\\n\";\n";
      ss << "            break;\n";
      ss << "        }\n";
    }
    for (auto &[name, child] : n->children) {
      self(self, child.get());
    }
  };
  generate_exec(generate_exec, root.get());
  ss << "        default:\n";
  ss << "            std::cerr << \"Incomplete command.\\n\";\n";
  ss << "            print_help(current_node);\n";
  ss << "            return 1;\n";
  ss << "    }\n";

  ss << "    return 0;\n";
  ss << "}\n";

  std::map<std::string, std::string> result;
  result["src/generated_cli.cpp"] = ss.str();

  if (!no_installable_package) {
    std::string cmake_content =
        "cmake_minimum_required(VERSION 3.15)\n"
        "project(generated_project LANGUAGES CXX)\n"
        "set(CMAKE_CXX_STANDARD 26)\n"
        "include(FetchContent)\n"
        "FetchContent_Declare(simdjson GIT_REPOSITORY "
        "https://github.com/simdjson/simdjson.git GIT_TAG v3.9.5)\n"
        "FetchContent_MakeAvailable(simdjson)\n"
        "find_package(CURL QUIET)\n"
        "if(NOT CURL_FOUND)\n"
        "  FetchContent_Declare(curl GIT_REPOSITORY "
        "https://github.com/curl/curl.git GIT_TAG curl-8_7_1)\n"
        "  set(BUILD_CURL_EXE OFF CACHE BOOL \"\" FORCE)\n"
        "  set(BUILD_TESTING OFF CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_USE_OPENSSL OFF CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_DISABLE_LDAP ON CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_DISABLE_LDAPS ON CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_USE_LIBPSL OFF CACHE BOOL \"\" FORCE)\n"
        "  set(CURL_USE_LIBSSH2 OFF CACHE BOOL \"\" FORCE)\n"
        "  FetchContent_MakeAvailable(curl)\n"
        "endif()\n"
        "add_subdirectory(src)\n";
    if (tests) {
      cmake_content += "add_subdirectory(tests)\n";
    }
    result["CMakeLists.txt"] = cmake_content;
    result["src/CMakeLists.txt"] =
        "set(HEADERS )\n"
        "set(SOURCES generated_cli.cpp)\n"
        "add_executable(generated_bin ${SOURCES} ${HEADERS})\n"
        "if(TARGET curl)\n"
        "  target_link_libraries(generated_bin PUBLIC simdjson::simdjson "
        "curl)\n"
        "else()\n"
        "  target_link_libraries(generated_bin PUBLIC simdjson::simdjson "
        "CURL::libcurl)\n"
        "endif()\n"
        "install(TARGETS generated_bin)\n";
  }

  if (tests) {
    result["tests/mocks.hpp"] = mocks::emit(spec);
    result["tests/CMakeLists.txt"] =
        "include(FetchContent)\n"
        "FetchContent_Declare(\n"
        "  googletest\n"
        "  GIT_REPOSITORY https://github.com/google/googletest.git\n"
        "  GIT_TAG release-1.12.1\n"
        ")\n"
        "FetchContent_MakeAvailable(googletest)\n"
        "add_executable(cli_test cli_test.cpp)\n"
        "target_link_libraries(cli_test gtest_main gmock)\n"
        "include(GoogleTest)\n"
        "gtest_discover_tests(cli_test)\n";
    result["tests/cli_test.cpp"] = "#include <gtest/gtest.h>\n"
                                   "#include \"mocks.hpp\"\n\n"
                                   "TEST(CliTest, MockTest) {\n"
                                   "    cdd_mock_client::MockClient mock;\n"
                                   "    EXPECT_TRUE(true);\n"
                                   "}\n";
  }

  if (!no_github_actions) {
    std::string ci_content = "name: CI\n"
                             "on: [push]\n"
                             "jobs:\n"
                             "  build:\n"
                             "    runs-on: ubuntu-latest\n"
                             "    steps:\n"
                             "      - uses: actions/checkout@v6\n"
                             "      - run: cmake . && cmake --build .\n";
    if (tests) {
      ci_content += "      - run: cd tests && ./cli_test\n";
    }
    result[".github/workflows/ci.yml"] = ci_content;
  }

  return result;
}

} // namespace cdd_cpp::client_sdk_cli

// GCOV_EXCL_BR_STOP
