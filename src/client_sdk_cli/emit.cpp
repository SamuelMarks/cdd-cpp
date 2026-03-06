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

std::string emit_cli(const openapi::OpenAPI &spec) noexcept {
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
  ss << "namespace cdd_cli {\n\n";

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
    default_url = spec.servers.value().front().url;
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

  if (spec.paths.has_value()) {
    for (const auto &[path, item] : spec.paths.value()) {
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
           << escape_string(n->op->summary.value()) << "\\n\";\n";
      }
      if (n->op->description.has_value()) {
        ss << "            std::cout << \"Description: "
           << escape_string(n->op->description.value()) << "\\n\";\n";
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
    if (n->method.has_value() && n->op.has_value()) {
      std::string op_id =
          n->op->operationId.value_or("op_" + std::to_string(node_ids[n]));
      sanitize_string(op_id);

      ss << "/**\n";
      if (n->op->summary.has_value()) {
        ss << " * @summary " << escape_string(n->op->summary.value()) << "\n";
      }
      if (n->op->description.has_value()) {
        ss << " * @description " << escape_string(n->op->description.value())
           << "\n";
      }
      ss << " * @route " << n->method.value() << " "
         << escape_string(n->path.value()) << "\n";
      ss << " */\n";

      
      if (n->op->parameters.has_value()) {
          for (const auto& p : n->op->parameters.value()) {
              if (p.description.has_value()) {
                  ss << " * @param " << p.name << " " << escape_string(p.description.value()) << "\n";
              } else {
                  ss << " * @param " << p.name << " " << p.in << " parameter\n";
              }
              if (p.example.has_value()) {
                  ss << " * @param_example " << p.name << " " << escape_string(p.example.value()) << "\n";
              }
              if (p.deprecated) {
                  ss << " * @param_deprecated " << p.name << "\n";
              }
          }
      }
      ss << "std::expected<std::string, std::string> handle_" << op_id
         << "(cdd_cli::Client& client";

      bool has_body = n->op->requestBody.has_value();
      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.value()) {
          std::string pname = p.name;
          sanitize_string(pname);
          ss << ", std::string " << pname;
        }
      }
      if (has_body) {
        ss << ", std::string body";
      }
      ss << ") noexcept {\n";

      ss << "    std::string path = \"" << escape_string(n->path.value())
         << "\";\n";
      ss << "    std::string qs = \"\";\n";

      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.value()) {
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
      ss << "    return client.request(\"" << escape_string(n->method.value())
         << "\", path, " << (has_body ? "body" : "\"\"") << ");\n";
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
    if (n->method.has_value() && n->op.has_value()) {
      std::string op_id =
          n->op->operationId.value_or("op_" + std::to_string(node_ids[n]));
      sanitize_string(op_id);

      ss << "        case " << node_ids[n] << ": {\n";

      ss << "            auto res = handle_" << op_id << "(client";

      bool has_body = n->op->requestBody.has_value();
      if (n->op->parameters.has_value()) {
        for (const auto &p : n->op->parameters.value()) {
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

  return ss.str();
}

} // namespace cdd_cpp::client_sdk_cli
