#include "../classes/emit_cli.hpp"
#include "../classes/emit_client.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../openapi/upgraders/upgrader.hpp"
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../google_discovery/parse.hpp"
#include "../utils/cpp_parser.hpp"
#include "../utils/json_writer.hpp"

using namespace cdd_cpp;

void print_help(std::ostream &out) noexcept {
  out << "CDD CLI (Code-Driven Development)\n"
      << "Usage:\n"
      << "  cdd-cpp --help\n"
      << "  cdd-cpp --version\n"
      << "  cdd-cpp to_openapi -f <path/to/code> [-o <spec.json>]\n"
      << "  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] -i <spec.json> "
         "[-o <docs.json>]\n"
      << "  cdd-cpp from_openapi to_sdk_cli -i <spec.json> -o "
         "<target_directory>\n"
      << "  cdd-cpp from_openapi to_sdk_cli --input-dir <specs_dir> -o "
         "<target_directory>\n"
      << "  cdd-cpp from_openapi to_sdk -i <spec.json> -o <target_directory>\n"
      << "  cdd-cpp from_openapi to_sdk --input-dir <specs_dir> -o "
         "<target_directory>\n"
      << "  cdd-cpp from_openapi to_server -i <spec.json> -o "
         "<target_directory>\n"
      << "  cdd-cpp from_openapi to_server --input-dir <specs_dir> -o "
         "<target_directory>\n"
      << "  cdd-cpp serve_json_rpc --port <port> --listen <host>\n"
      << "\n"
      << "Commands:\n"
      << "  sync         : Bi-directional sync of code directory and OpenAPI "
         "spec.\n"
      << "  from_openapi : Parses an OpenAPI spec and emits C++ code.\n"
      << "  to_openapi   : Parses C++ code and emits an OpenAPI spec.\n"
      << "  to_docs_json : Generates JSON documentation for API calls.\n"
      << "  serve_json_rpc: Starts JSON-RPC server.\n";
}

void print_version(std::ostream &out) noexcept { out << "cdd-cpp version 0.0.1\n"; }

std::expected<std::string, std::string>
read_file(const std::string &path) noexcept {
  std::ifstream fs(path);
  if (!fs)
    return std::unexpected("Could not open file: " + path);
  std::string content((std::istreambuf_iterator<char>(fs)),
                      std::istreambuf_iterator<char>());
  if (fs.bad())
    return std::unexpected("Error reading file: " + path);
  return content;
}

std::string get_arg_or_env(const std::string &val, const std::string &env_var,
                           const std::string &def = "") noexcept {
  if (!val.empty())
    return val;
  if (const char *env_p = std::getenv(env_var.c_str()))
    return std::string(env_p);
  return def;
}

bool get_bool_arg_or_env(bool val, const std::string &env_var) noexcept {
  if (val)
    return true;
  if (const char *env_p = std::getenv(env_var.c_str())) {
    std::string s(env_p);
    return s == "1" || s == "true" || s == "TRUE";
  }
  return false;
}

namespace cdd_cpp::cli {
void sync_command(const std::string &code_dir, const std::string &spec_file);
}

int main_impl(int argc, char **argv, std::ostream &out, std::ostream &err) noexcept {
  if (argc < 2) {
    print_help(out);
    return 1;
  }

  std::string command = argv[1];

  if (command == "--help" || command == "-h") {
    print_help(out);
    return 0;
  }
  if (command == "--version" || command == "-v") {
    print_version(out);
    return 0;
  }

  if (command == "sync") {
    std::string folder, spec;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-d" || arg == "--dir") && i + 1 < argc)
        folder = argv[++i];
      if ((arg == "-s" || arg == "--spec") && i + 1 < argc)
        spec = argv[++i];
    }
    folder = get_arg_or_env(folder, "CDD_CPP_DIR");
    spec = get_arg_or_env(spec, "CDD_CPP_SPEC");
    if (folder.empty() || spec.empty()) {
      err << "Usage: cdd_cpp sync -d <dir> -s <spec.json>\n";
      return 1;
    }
    cdd_cpp::cli::sync_command(folder, spec);
    return 0;
  }

  else if (command == "from_openapi") {
    if (argc < 3) {
      err << "Missing subcommand for from_openapi\n";
      return 1;
    }
    std::string subcommand = argv[2];
    std::string input;
    std::string input_dir;
    std::string output;
    bool no_github_actions = false;
    bool no_installable_package = false;

    for (int i = 3; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      } else if (arg == "--input-dir" && i + 1 < argc) {
        input_dir = argv[++i];
      } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
        output = argv[++i];
      } else if (arg == "--no-github-actions") {
        no_github_actions = true;
      } else if (arg == "--no-installable-package") {
        no_installable_package = true;
      }
    }

    input = get_arg_or_env(input, "CDD_CPP_INPUT");
    input_dir = get_arg_or_env(input_dir, "CDD_CPP_INPUT_DIR");
    output = get_arg_or_env(output, "CDD_CPP_OUTPUT", ".");
    no_github_actions =
        get_bool_arg_or_env(no_github_actions, "CDD_CPP_NO_GITHUB_ACTIONS");
    no_installable_package = get_bool_arg_or_env(
        no_installable_package, "CDD_CPP_NO_INSTALLABLE_PACKAGE");

    if (input.empty() && input_dir.empty()) {
      err << "Missing -i <spec.json> or --input-dir <specs_dir>\n";
      return 1;
    }

    std::string content = "";
    if (!input.empty()) {
      auto content_res = read_file(input);
      if (!content_res) {
        err << "Error: " << content_res.error() << "\n";
        return 1;
      }
      content = *content_res;
    } else {
      content = "{\"openapi\": \"3.2.0\"}";
    }

    auto upgraded_spec_res = openapi::upgraders::upgrade_to_latest(content);
    if (!upgraded_spec_res) {
      err << "Error: " << upgraded_spec_res.error() << "\n";
      return 1;
    }
    auto spec_res = openapi::parse(*upgraded_spec_res);
    if (!spec_res) {
      err << "Error: " << spec_res.error() << "\n";
      return 1;
    }
    auto spec = *spec_res;

    std::string code = "";
    std::string filename = "generated.cpp";
    if (subcommand == "to_sdk_cli") {
      code = classes::emit_cli(spec);
      filename = "generated_cli.cpp";
    } else if (subcommand == "to_sdk") {
      code = classes::emit_client(spec);
      filename = "generated_client.hpp";
    } else if (subcommand == "to_server") {
      code = "// Server implementation placeholder\n";
      filename = "generated_server.cpp";
    } else {
      err << "Unknown subcommand: " << subcommand << "\n";
      return 1;
    }

    std::string out_path = output + "/" + filename;
    std::ofstream out_file(out_path);
    if (!out_file) {
      err << "Could not open output file: " << out_path << "\n";
      return 1;
    }
    out_file << code;
    out << "Successfully generated " << out_path << "\n";

    if (!no_installable_package) {
      std::string cmake_path = output + "/CMakeLists.txt";
      std::ofstream cmake_file(cmake_path);
      cmake_file << "cmake_minimum_required(VERSION 3.15)\n"
                 << "project(generated_project LANGUAGES CXX)\n"
                 << "set(CMAKE_CXX_STANDARD 20)\n"
                 << "add_executable(generated_bin " << filename << ")\n";
    }

    if (!no_github_actions) {
      std::string wf_dir = output + "/.github/workflows";
      std::error_code ec;
      std::filesystem::create_directories(wf_dir, ec);
      std::string ci_path = wf_dir + "/ci.yml";
      std::ofstream ci_file(ci_path);
      ci_file << "name: CI\n"
              << "on: [push]\n"
              << "jobs:\n"
              << "  build:\n"
              << "    runs-on: ubuntu-latest\n"
              << "    steps:\n"
              << "      - uses: actions/checkout@v3\n"
              << "      - run: cmake . && make\n";
    }

  } else if (command == "to_openapi") {
    std::string folder;
    std::string output;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-f" || arg == "--folder") && i + 1 < argc) {
        folder = argv[++i];
      } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
        output = argv[++i];
      }
    }
    folder = get_arg_or_env(folder, "CDD_CPP_FOLDER");
    output = get_arg_or_env(output, "CDD_CPP_OUTPUT");

    if (folder.empty()) {
      err << "Missing -f <path/to/code>\n";
      return 1;
    }
    auto spec = utils::parse_cpp_project(folder);
    std::string spec_str = openapi::emit(spec);
    if (output.empty()) {
      out << spec_str << "\n";
    } else {
      std::ofstream fs(output);
      fs << spec_str << "\n";
    }
  } else if (command == "serve_json_rpc") {
    std::string port = "8080";
    std::string listen = "127.0.0.1";
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--port" && i + 1 < argc)
        port = argv[++i];
      else if (arg == "--listen" && i + 1 < argc)
        listen = argv[++i];
    }
    port = get_arg_or_env(port, "CDD_CPP_PORT", "8080");
    listen = get_arg_or_env(listen, "CDD_CPP_LISTEN", "127.0.0.1");
    out << "Starting JSON-RPC server on " << listen << ":" << port << " ...\n";
    // placeholder for json rpc server logic
  } else if (command == "to_docs_json") {
    bool no_imports = false;
    bool no_wrapping = false;
    std::string input_file;
    std::string output_file;

    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--no-imports")
        no_imports = true;
      else if (arg == "--no-wrapping")
        no_wrapping = true;
      else if ((arg == "-i" || arg == "--input") && i + 1 < argc)
        input_file = argv[++i];
      else if ((arg == "-o" || arg == "--output") && i + 1 < argc)
        output_file = argv[++i];
    }

    no_imports = get_bool_arg_or_env(no_imports, "CDD_CPP_NO_IMPORTS");
    no_wrapping = get_bool_arg_or_env(no_wrapping, "CDD_CPP_NO_WRAPPING");
    input_file = get_arg_or_env(input_file, "CDD_CPP_INPUT");
    output_file = get_arg_or_env(output_file, "CDD_CPP_OUTPUT");

    if (input_file.empty()) {
      err << "Missing -i <spec.json>\n";
      return 1;
    }

    auto content_res = read_file(input_file);
    if (!content_res) {
      err << "Error: " << content_res.error() << "\n";
      return 1;
    }
    std::string content = *content_res;
    auto spec_res = openapi::parse(content);
    if (!spec_res) {
      err << "Error: " << spec_res.error() << "\n";
      return 1;
    }
    auto spec = *spec_res;

    utils::JsonWriter jw;
    jw.start_array();
    jw.start_object();
    jw.key_value("language", "cpp");
    jw.key("operations");
    jw.start_array();

    if (spec.paths.has_value() && !spec.paths->empty()) {
      for (const auto &[path, pi] : spec.paths.value()) {
        auto add_op = [&](const std::string &method,
                          const std::optional<openapi::Operation> &op) {
          if (!op.has_value())
            return;
          jw.start_object();
          jw.key_value("method", method);
          jw.key_value("path", path);
          if (op->operationId.has_value()) {
            jw.key_value("operationId", op->operationId.value());
          }
          jw.key("code");
          jw.start_object();

          if (!no_imports) {
            jw.key_value("imports",
                         "#include <iostream>\n#include <cdd_client.hpp>");
          }
          if (!no_wrapping) {
            jw.key_value("wrapper_start",
                         "int main() {\n    cdd_cpp::Client client;\n");
          }

          std::string op_name = op->operationId.value_or("request");
          jw.key_value("snippet", "    auto res = client." + op_name +
                                      "();\n    out << res << \"\\n\";");

          if (!no_wrapping) {
            jw.key_value("wrapper_end", "    return 0;\n}");
          }
          jw.end_object();
          jw.end_object();
        };

        add_op("GET", pi.get);
        add_op("POST", pi.post);
        add_op("PUT", pi.put);
        add_op("DELETE", pi.delete_op);
        add_op("PATCH", pi.patch);
      }
    }

    jw.end_array();
    jw.end_object();
    jw.end_array();

    if (output_file.empty()) {
      out << jw.str() << "\n";
    } else {
      std::ofstream fs(output_file);
      fs << jw.str() << "\n";
    }
  } else {
    err << "Unknown command: " << command << "\n";
    print_help(out);
    return 1;
  }

  return 0;
}

#include "../utils/httplib.h"
#include <mutex>

std::mutex g_cout_mutex;

int main(int argc, char **argv) noexcept {
  if (argc >= 2 && std::string(argv[1]) == "serve_json_rpc") {
    std::string port = "8080";
    std::string listen_host = "127.0.0.1";
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--port" && i + 1 < argc)
        port = argv[++i];
      else if (arg == "--listen" && i + 1 < argc)
        listen_host = argv[++i];
    }
    port = get_arg_or_env(port, "CDD_CPP_PORT", "8080");
    listen_host = get_arg_or_env(listen_host, "CDD_CPP_LISTEN", "127.0.0.1");

    httplib::Server svr;
    svr.Post("/rpc", [](const httplib::Request &req, httplib::Response &res) {
      simdjson::dom::parser parser;
      simdjson::dom::element doc;
      auto error = parser.parse(req.body).get(doc);
      if (error) {
        res.status = 400;
        res.set_content("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32700, "
                        "\"message\": \"Parse error\"}, \"id\": null}",
                        "application/json");
        return;
      }

      std::string_view method_sv;
      if (doc["method"].get(method_sv)) {
        res.status = 400;
        return;
      }

      std::vector<std::string> args = {"cdd-cpp", std::string(method_sv)};
      simdjson::dom::array params;
      if (!doc["params"].get(params)) {
        for (auto elem : params) {
          std::string_view p;
          if (!elem.get(p)) {
            args.push_back(std::string(p));
          }
        }
      }

      std::vector<char *> argv_vec;
      for (auto &s : args)
        argv_vec.push_back(const_cast<char *>(s.c_str()));

      std::ostringstream out, err;
      int ret;
      {
        std::lock_guard<std::mutex> lock(g_cout_mutex);
        ret = main_impl(argv_vec.size(), argv_vec.data(), out, err);
      }

      utils::JsonWriter jw;
      jw.start_object();
      jw.key_value("jsonrpc", "2.0");

      std::string id_str = "null";
      if (doc["id"].type() == simdjson::dom::element_type::INT64) {
        int64_t id_val;
        if (doc["id"].get(id_val) == simdjson::SUCCESS) {
          jw.key_value("id", std::to_string(id_val));
        }
      } else if (doc["id"].type() == simdjson::dom::element_type::STRING) {
        std::string_view id_val;
        if (doc["id"].get(id_val) == simdjson::SUCCESS) {
          jw.key_value("id", std::string(id_val));
        }
      } else {
        jw.key("id");
        jw.null_value(); // simplified
      }

      if (ret == 0) {
        jw.key_value("result", out.str());
      } else {
        jw.key("error");
        jw.start_object();
        jw.key_value("code", -32603);
        jw.key_value("message", err.str());
        jw.end_object();
      }
      jw.end_object();
      res.set_content(jw.str(), "application/json");
    });

    std::cout << "Starting JSON-RPC server on " << listen_host << ":" << port
              << " ...\n";
    svr.listen(listen_host.c_str(), std::stoi(port));
    return 0;
  }

  return main_impl(argc, argv, std::cout, std::cerr);
}
