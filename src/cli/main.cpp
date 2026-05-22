#include "../client_sdk/emit.hpp"
#include "../client_sdk_cli/emit.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../openapi/upgraders/upgrader.hpp"
#include "../server/emit.hpp"
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

void print_version(std::ostream &out) noexcept {
  out << "cdd-cpp version 0.0.1\n";
}

std::expected<std::string, std::string>
read_file(const std::string &path) noexcept {
  std::ifstream fs(path);
  if (!fs.is_open())
    return std::unexpected("Could not open input file: " + path);
  std::string content((std::istreambuf_iterator<char>(fs)),
                      std::istreambuf_iterator<char>());
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
void sync_command(const std::string &code_dir,
                  const std::string &spec_file) noexcept;
}

int main_impl(int argc, char **argv, std::ostream &out,
              std::ostream &err) noexcept {
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
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n  cdd-cpp sync -d <dir> -s <spec.json>\n";
        return 0;
      }
    }

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
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n"
            << "  cdd-cpp from_openapi to_sdk_cli -i <spec.json> -o "
               "<target_directory>\n"
            << "  cdd-cpp from_openapi to_sdk_cli --input-dir <specs_dir> -o "
               "<target_directory>\n"
            << "  cdd-cpp from_openapi to_sdk -i <spec.json> -o "
               "<target_directory>\n"
            << "  cdd-cpp from_openapi to_sdk --input-dir <specs_dir> -o "
               "<target_directory>\n"
            << "  cdd-cpp from_openapi to_server -i <spec.json> -o "
               "<target_directory>\n"
            << "  cdd-cpp from_openapi to_server --input-dir <specs_dir> -o "
               "<target_directory>\n";
        return 0;
      }
    }

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
    bool tests = false;

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
      } else if (arg == "--tests") {
        tests = true;
      }
    }

    input = get_arg_or_env(input, "CDD_CPP_INPUT");
    input_dir = get_arg_or_env(input_dir, "CDD_CPP_INPUT_DIR");
    output = get_arg_or_env(output, "CDD_CPP_OUTPUT", ".");
    no_github_actions =
        get_bool_arg_or_env(no_github_actions, "CDD_CPP_NO_GITHUB_ACTIONS");
    no_installable_package = get_bool_arg_or_env(
        no_installable_package, "CDD_CPP_NO_INSTALLABLE_PACKAGE");
    tests = get_bool_arg_or_env(tests, "CDD_CPP_TESTS");

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
    auto spec = *openapi::parse(*upgraded_spec_res);

    std::map<std::string, std::string> multiple_files;

    if (subcommand == "to_sdk_cli") {
      multiple_files = client_sdk_cli::emit_cli(spec, no_github_actions,
                                                no_installable_package, tests);
    } else if (subcommand == "to_sdk") {
      multiple_files = client_sdk::emit_client(spec, no_github_actions,
                                               no_installable_package, tests);
    } else if (subcommand == "to_server") {
      multiple_files["src/generated_server.cpp"] =
          "// Server implementation placeholder\n";
      if (!no_installable_package) {
        std::string cmake_content =
            "cmake_minimum_required(VERSION 3.15)\nproject(generated_project "
            "LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD "
            "26)\nadd_subdirectory(src)\n";
        if (tests) {
          cmake_content += "add_subdirectory(tests)\n";
        }
        multiple_files["CMakeLists.txt"] = cmake_content;
        multiple_files["src/CMakeLists.txt"] =
            "set(HEADERS )\n"
            "set(SOURCES generated_server.cpp)\n"
            "add_executable(generated_bin ${SOURCES} ${HEADERS})\n"
            "install(TARGETS generated_bin)\n";
      }

      if (tests) {
        multiple_files["tests/CMakeLists.txt"] =
            "include(FetchContent)\n"
            "FetchContent_Declare(\n"
            "  googletest\n"
            "  GIT_REPOSITORY https://github.com/google/googletest.git\n"
            "  GIT_TAG release-1.12.1\n"
            ")\n"
            "FetchContent_MakeAvailable(googletest)\n"
            "add_executable(server_test server_test.cpp)\n"
            "target_link_libraries(server_test gtest_main gmock)\n"
            "include(GoogleTest)\n"
            "gtest_discover_tests(server_test)\n";
        multiple_files["tests/server_test.cpp"] =
            "#include <gtest/gtest.h>\n\n"
            "TEST(ServerTest, BasicTest) {\n"
            "    EXPECT_TRUE(true);\n"
            "}\n";
      }

      if (!no_github_actions) {
        std::string ci_content =
            "name: CI\non: [push]\njobs:\n  build:\n    runs-on: "
            "ubuntu-latest\n    steps:\n      - uses: actions/checkout@v3\n    "
            "  - run: cmake . && cmake --build .\n";
        if (tests) {
          ci_content += "      - run: cd tests && ./server_test\n";
        }
        multiple_files[".github/workflows/ci.yml"] = ci_content;
      }
    } else {
      err << "Unknown subcommand: " << subcommand << "\n";
      return 1;
    }

    if (!multiple_files.empty()) {
      for (const auto &[fname, content] : multiple_files) {
        std::string out_path = output + "/" + fname;
        std::filesystem::path p(out_path);
        if (p.has_parent_path()) {
          std::error_code ec;
          std::filesystem::create_directories(p.parent_path(), ec);
        }
        std::ofstream out_file(out_path);
        if (!out_file) {
          err << "Could not open output file: " << out_path << "\n";
          return 1;
        }
        out_file << content;
        out << "Successfully generated " << out_path << "\n";
      }
    }

  } else if (command == "to_openapi") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n  cdd-cpp to_openapi -f <path/to/code> [-o "
               "<spec.json>]\n";
        return 0;
      }
    }

    std::string input;
    std::string output;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
        output = argv[++i];
      }
    }
    input = get_arg_or_env(input, "CDD_CPP_INPUT");
    output = get_arg_or_env(output, "CDD_CPP_OUTPUT");

    if (input.empty()) {
      err << "Missing -i <path/to/code>\n";
      return 1;
    }
    auto spec = utils::parse_cpp_project(input);
    std::string spec_str = openapi::emit(spec);
    if (output.empty()) {
      out << spec_str << "\n";
    } else {
      std::ofstream fs(output);
      fs << spec_str << "\n";
    }

  } else if (command == "to_docs_json") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] "
               "-i <spec.json> [-o <docs.json>]\n";
        return 0;
      }
    }

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

#if !defined(__wasi__) && !defined(__EMSCRIPTEN__)
#include "../utils/httplib.h"
#include <mutex>
std::mutex g_cout_mutex;
#endif

int main(int argc, char **argv) noexcept {

#if !defined(__wasi__) && !defined(__EMSCRIPTEN__)
  if (argc >= 2 && std::string(argv[1]) == "serve_json_rpc") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        std::cout << "Usage:\n"
                  << "  cdd-cpp serve_json_rpc --port <port> --listen <host>\n";
        return 0;
      }
    }

    std::string port = "8080";
    std::string listen_host = "127.0.0.1";
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--port" && i + 1 < argc) {
        port = argv[++i];
      } else if (arg == "--listen" && i + 1 < argc) {
        listen_host = argv[++i];
      }
    }

    httplib::Server svr;

    svr.Post("/", [](const httplib::Request &req, httplib::Response &res) {
      std::string result = cdd_cpp::server::serve_json_rpc(req.body);
      res.set_content(result, "application/json");
      res.set_header("Access-Control-Allow-Origin", "*");
    });

    svr.Options(
        "/", [](const httplib::Request & /*req*/, httplib::Response &res) {
          res.set_header("Access-Control-Allow-Origin", "*");
          res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
          res.set_header("Access-Control-Allow-Headers", "Content-Type");
          res.set_content("", "text/plain");
        });

    svr.Get("/stop",
            [&](const httplib::Request &, httplib::Response &) { svr.stop(); });

    std::cout << "JSON RPC server listening on " << listen_host << ":" << port
              << std::endl;
    svr.listen(listen_host, std::stoi(port));
    return 0;
  }
#else
  if (argc >= 2 && std::string(argv[1]) == "serve_json_rpc") {
    std::cerr << "serve_json_rpc is not supported in WASI builds." << std::endl;
    return 1;
  }
#endif

  return main_impl(argc, argv, std::cout, std::cerr);
}
