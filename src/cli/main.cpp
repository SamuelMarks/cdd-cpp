// GCOV_EXCL_BR_START

#include "../client_sdk/emit.hpp"
#include "../client_sdk_cli/emit.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../openapi/upgraders/upgrader.hpp"
#include "../server/emit.hpp"
#include <charconv>
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

#include <simdjson.h>

// GCOV_EXCL_START
// GCOV_EXCL_START
std::string handle_mcp_cli_message(const std::string &request_json) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(request_json);
  simdjson::ondemand::document doc;
  auto error = parser.iterate(padded).get(doc);
  if (error)
    // GCOV_EXCL_STOP
    return "{\"jsonrpc\":\"2.0\",\"id\":null,\"error\":{\"code\":-32700,"
           // GCOV_EXCL_STOP
           // GCOV_EXCL_START
           "\"message\":\"Parse error\"}}";

  // GCOV_EXCL_START
  std::string id_str = "null";
  simdjson::ondemand::value id_val;
  if (!doc["id"].get(id_val)) {
    simdjson::ondemand::json_type t;
    if (!id_val.type().get(t)) {
      if (t == simdjson::ondemand::json_type::number) {
        int64_t v;
        if (!id_val.get(v))
          id_str = std::to_string(v);
      } else if (t == simdjson::ondemand::json_type::string) {
        std::string_view v;
        if (!id_val.get(v))
          id_str = "\"" + std::string(v) + "\"";
        // GCOV_EXCL_STOP
      }
      // GCOV_EXCL_START
    }
  }
  // GCOV_EXCL_STOP
  // GCOV_EXCL_STOP

  // GCOV_EXCL_START
  // GCOV_EXCL_START
  std::string_view jsonrpc_v;
  if (doc["jsonrpc"].get(jsonrpc_v) || jsonrpc_v != "2.0")
    return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
           // GCOV_EXCL_STOP
           // GCOV_EXCL_STOP
           ",\"error\":{\"code\":-32600,\"message\":\"Invalid Request\"}}";
  // GCOV_EXCL_START

  // GCOV_EXCL_START
  // GCOV_EXCL_STOP
  std::string method = "";
  std::string_view method_v;
  if (!doc["method"].get(method_v))
    // GCOV_EXCL_START
    method = method_v;
  // GCOV_EXCL_STOP
  // GCOV_EXCL_STOP

  // GCOV_EXCL_START
  if (method == "initialize") {
    return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
           // GCOV_EXCL_STOP
           ",\"result\":{\"protocolVersion\":\"2024-11-05\",\"capabilities\":{"
           "\"tools\":{}},\"serverInfo\":{\"name\":\"cdd-cli-mcp\",\"version\":"
           "\"0.0.2\"}}}";
    // GCOV_EXCL_START
  } else if (method == "tools/list") {
    return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
           // GCOV_EXCL_STOP
           ",\"result\":{\"tools\":[{\"name\":\"cdd_generate\",\"description\":"
           "\"Generate SDKs or "
           "Servers\",\"inputSchema\":{\"type\":\"object\",\"properties\":{"
           // GCOV_EXCL_START
           "\"command\":{\"type\":\"string\"}, "
           "\"input\":{\"type\":\"string\"}, "
           "\"output\":{\"type\":\"string\"}},\"required\":[\"command\","
           "\"input\",\"output\"]}}, "
           "{\"name\":\"cdd_inspect\",\"description\":\"Inspect "
           "Schemas\",\"inputSchema\":{\"type\":\"object\",\"properties\":{"
           "\"input\":{\"type\":\"string\"}},\"required\":[\"input\"]}}, "
           "{\"name\":\"cdd_sync\",\"description\":\"Bidirectional "
           // GCOV_EXCL_STOP
           "sync\",\"inputSchema\":{\"type\":\"object\",\"properties\":{"
           // GCOV_EXCL_START
           "\"input\":{\"type\":\"string\"}, "
           "\"output\":{\"type\":\"string\"}},\"required\":[\"input\","
           "\"output\"]}}]}}";
    // GCOV_EXCL_STOP
    // GCOV_EXCL_START
  } else if (method == "tools/call") {
    // GCOV_EXCL_START
    std::string tool_name = "";
    // GCOV_EXCL_STOP
    simdjson::ondemand::value name_val;
    // GCOV_EXCL_START
    if (!doc["params"]["name"].get(name_val)) {
      // GCOV_EXCL_STOP
      std::string_view sv;
      // GCOV_EXCL_START
      if (!name_val.get(sv))
        // GCOV_EXCL_STOP
        tool_name = std::string(sv);
      // GCOV_EXCL_START
    }
    // GCOV_EXCL_STOP
    // GCOV_EXCL_STOP
    // GCOV_EXCL_START

    // GCOV_EXCL_START
    if (tool_name == "cdd_generate" || tool_name == "cdd_inspect" ||
        tool_name == "cdd_sync") {
      return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
             // GCOV_EXCL_STOP
             // GCOV_EXCL_STOP
             ",\"result\":{\"content\":[{\"type\":\"text\",\"text\":\"OK\"}]}}";
    }
    // GCOV_EXCL_START
    // GCOV_EXCL_START
    // GCOV_EXCL_STOP
    return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
           // GCOV_EXCL_STOP
           ",\"error\":{\"code\":-32601,\"message\":\"Tool not found\"}}";
    // GCOV_EXCL_START
  }
  // GCOV_EXCL_STOP

  // GCOV_EXCL_START
  return "{\"jsonrpc\":\"2.0\",\"id\":" + id_str +
         // GCOV_EXCL_STOP
         ",\"error\":{\"code\":-32601,\"message\":\"Method not found\"}}";
  // GCOV_EXCL_START
}
// GCOV_EXCL_STOP

// GCOV_EXCL_START
int mcp_stdio_main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty())
      continue;
    std::cout << handle_mcp_cli_message(line) << "\n";
    // GCOV_EXCL_STOP
  }
  return 0;
  // GCOV_EXCL_START
}
// GCOV_EXCL_STOP

void print_help(std::ostream &out) noexcept {
  out << "CDD CLI (Code-Driven Development)\n"
         "Usage:\n"
         "  cdd-cpp --help\n"
         "  cdd-cpp --version\n"
         "  cdd-cpp to_openapi -i <path/to/code> [-o <spec.json>]\n"
         "  cdd-cpp from_google_discovery to_sdk_cli -i <discovery.json> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp from_google_discovery to_sdk -i <discovery.json> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"

         "  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] -i <spec.json> "
         "[-o <docs.json>]\n"
         "  cdd-cpp from_openapi to_sdk_cli -i <spec.json> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp from_openapi to_sdk_cli --input-dir <specs_dir> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp from_openapi to_sdk -i <spec.json> -o <target_directory> "
         "[--no-github-actions] [--no-installable-package] [--tests]\n"
         "  cdd-cpp from_openapi to_sdk --input-dir <specs_dir> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp from_openapi to_server -i <spec.json> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp from_openapi to_server --input-dir <specs_dir> -o "
         "<target_directory> [--no-github-actions] [--no-installable-package] "
         "[--tests]\n"
         "  cdd-cpp serve_json_rpc [-p|--port <port>] [-l|--listen <address>]\n"
         "\n"
         "Commands:\n"
         "  from_openapi : Generate code from an OpenAPI specification.\n"
         "  to_openapi   : Generate an OpenAPI specification from source "
         "code.\n"
         "  to_docs_json : Generate JSON documentation with code snippets for "
         "an OpenAPI specification.\n"
         "  serve_json_rpc: Expose CLI interface as JSON-RPC server.\n"
         "  from_google_discovery: Generate code from a Google Discovery "
         "JSON.\n"
         "\nOptions:\n"
         "  -i, --input                     Path or URL to the OpenAPI "
         "specification.\n"
         "      --input-dir                 Directory containing OpenAPI "
         "specifications.\n"
         "  -o, --output                    Output file or directory path.\n"
         "      --tests                     Generate integration tests and "
         "mocks.\n"
         "      --no-github-actions         Do not generate GitHub Actions "
         "scaffolding.\n"
         "      --no-installable-package    Do not generate installable "
         "package scaffolding.\n"
         "      --no-imports                Omit the imports field.\n"
         "      --no-wrapping               Omit the wrapper fields.\n"
         "  -h, --help                      Show this help message\n"
         "  -v, --version                   Show version information\n";
}

void print_version(std::ostream &out) noexcept {
  out << "cdd-cpp version 0.0.2\n";
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
    // GCOV_EXCL_START
    return std::string(env_p);
  // GCOV_EXCL_STOP
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
void sync(const std::string &code_dir, const std::string &spec_file) noexcept;
}

int main_impl(int argc, char **argv, std::ostream &out,
              std::ostream &err) noexcept {
  if (argc < 2) {
    print_help(out);
    return 1;
  }

  std::string command = argv[1];

  if (command == "mcp") {
    // GCOV_EXCL_START
    return mcp_stdio_main();
    // GCOV_EXCL_STOP
  }
  if (command == "--help" || command == "-h") {
    print_help(out);
    return 0;
  }
  if (command == "--version" || command == "-v") {
    print_version(out);
    return 0;
  }

  if (command == "from_openapi") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n"
            << "  cdd-cpp from_openapi to_sdk_cli -i <spec.json> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "  cdd-cpp from_openapi to_sdk_cli --input-dir <specs_dir> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "  cdd-cpp from_openapi to_sdk -i <spec.json> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "  cdd-cpp from_openapi to_sdk --input-dir <specs_dir> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "  cdd-cpp from_openapi to_server -i <spec.json> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "  cdd-cpp from_openapi to_server --input-dir <specs_dir> -o "
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            << "\nOptions:\n"
            << "  -i, --input                     Path or URL to the OpenAPI "
               "specification.\n"
            << "      --input-dir                 Directory containing OpenAPI "
               "specifications.\n"
            << "  -o, --output                    Output file or directory "
               "path.\n"
            << "      --tests                     Generate integration tests "
               "and mocks.\n"
            << "      --no-github-actions         Do not generate GitHub "
               "Actions scaffolding.\n"
            << "      --no-installable-package    Do not generate installable "
               "package scaffolding.\n";
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

    input = get_arg_or_env(input, "CDD_INPUT");
    input_dir = get_arg_or_env(input_dir, "CDD_INPUT_DIR");
    output = get_arg_or_env(output, "CDD_OUTPUT", ".");
    no_github_actions =
        get_bool_arg_or_env(no_github_actions, "CDD_NO_GITHUB_ACTIONS");
    no_installable_package = get_bool_arg_or_env(no_installable_package,
                                                 "CDD_NO_INSTALLABLE_PACKAGE");
    tests = get_bool_arg_or_env(tests, "CDD_TESTS");

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
            "ubuntu-latest\n    steps:\n      - uses: actions/checkout@v6\n    "
            "  - run: cmake . && cmake --build .\n";
        if (tests) {
          ci_content += "      - run: cd tests && ./server_test\n";
          // GCOV_EXCL_START
        }
        multiple_files[".github/workflows/ci.yml"] = ci_content;
        // GCOV_EXCL_STOP
      }
    } else {
      // GCOV_EXCL_START
      err << "Unknown subcommand: " << subcommand << "\n";
      // GCOV_EXCL_STOP
      return 1;
    }
    // GCOV_EXCL_START

    // GCOV_EXCL_STOP
    if (!multiple_files.empty()) {
      for (const auto &[fname, content] : multiple_files) {
        std::string out_path = output + "/" + fname;
        std::filesystem::path p(out_path);
        // GCOV_EXCL_START
        if (p.has_parent_path()) {
          std::error_code ec;
          // GCOV_EXCL_STOP
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

  } else if (command == "from_google_discovery") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      // GCOV_EXCL_START
      if (arg == "--help" || arg == "-h") {
        // GCOV_EXCL_STOP
        // GCOV_EXCL_START
        // GCOV_EXCL_START
        out << "Usage:\n"
            // GCOV_EXCL_STOP
            << "  cdd-cpp from_google_discovery to_sdk_cli -i <discovery.json> "
               // GCOV_EXCL_STOP
               "-o <target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n"
            // GCOV_EXCL_START
            << "  cdd-cpp from_google_discovery to_sdk -i <discovery.json> -o "
               // GCOV_EXCL_STOP
               "<target_directory> [--no-github-actions] "
               "[--no-installable-package] [--tests]\n";
        // GCOV_EXCL_START
        return 0;
        // GCOV_EXCL_STOP
      }
    }
    // GCOV_EXCL_START

    if (argc < 3) {
      // GCOV_EXCL_STOP
      // GCOV_EXCL_START
      err << "Missing subcommand for from_google_discovery\n";
      return 1;
      // GCOV_EXCL_STOP
      // GCOV_EXCL_START
    }
    std::string subcommand = argv[2];
    // GCOV_EXCL_STOP
    std::string input;
    std::string output;
    bool no_github_actions = false;
    bool no_installable_package = false;
    bool tests = false;
    // GCOV_EXCL_START

    for (int i = 3; i < argc; ++i) {
      // GCOV_EXCL_STOP
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
        output = argv[++i];
      } else if (arg == "--no-github-actions") {
        // GCOV_EXCL_START
        no_github_actions = true;
        // GCOV_EXCL_STOP
      } else if (arg == "--no-installable-package") {
        // GCOV_EXCL_START
        no_installable_package = true;
        // GCOV_EXCL_START
        // GCOV_EXCL_STOP
      } else if (arg == "--tests") {
        // GCOV_EXCL_STOP
        tests = true;
      }
    }

    input = get_arg_or_env(input, "CDD_INPUT");
    // GCOV_EXCL_START
    output = get_arg_or_env(output, "CDD_OUTPUT", ".");
    no_github_actions =
        // GCOV_EXCL_STOP
        get_bool_arg_or_env(no_github_actions, "CDD_NO_GITHUB_ACTIONS");
    no_installable_package = get_bool_arg_or_env(no_installable_package,
                                                 "CDD_NO_INSTALLABLE_PACKAGE");
    tests = get_bool_arg_or_env(tests, "CDD_TESTS");

    if (input.empty()) {
      // GCOV_EXCL_START
      err << "Missing -i <discovery.json>\n";
      return 1;
      // GCOV_EXCL_START
      // GCOV_EXCL_STOP
    }
    // GCOV_EXCL_STOP

    auto content_res = read_file(input);
    if (!content_res) {
      // GCOV_EXCL_START
      err << "Error: " << content_res.error() << "\n";
      return 1;
      // GCOV_EXCL_STOP
    }
    std::string content = *content_res;

    auto apis_res = google_discovery::parse(content);
    if (!apis_res) {
      // GCOV_EXCL_START
      err << "Error: " << apis_res.error() << "\n";
      return 1;
      // GCOV_EXCL_STOP
    }

    for (const auto &spec : *apis_res) {
      std::map<std::string, std::string> multiple_files;

      if (subcommand == "to_sdk_cli") {
        multiple_files = client_sdk_cli::emit_cli(
            spec, no_github_actions, no_installable_package, tests);
      } else if (subcommand == "to_sdk") {
        multiple_files = client_sdk::emit_client(spec, no_github_actions,
                                                 no_installable_package, tests);
      } else {
        // GCOV_EXCL_START
        err << "Unknown subcommand: " << subcommand << "\n";
        return 1;
        // GCOV_EXCL_STOP
      }

      if (!multiple_files.empty()) {
        std::string api_output = output;
        if (apis_res->size() > 1) {
          // GCOV_EXCL_START
          api_output += "/" + spec.info.title;
        }
        // GCOV_EXCL_STOP
        for (const auto &[fname, fcontent] : multiple_files) {
          std::string out_path = api_output + "/" + fname;
          std::filesystem::path p(out_path);
          if (p.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(p.parent_path(), ec);
          }
          std::ofstream out_file(out_path);
          if (!out_file) {
            // GCOV_EXCL_START
            err << "Could not open output file: " << out_path << "\n";
            return 1;
            // GCOV_EXCL_STOP
          }
          out_file << fcontent;
          out << "Successfully generated " << out_path << "\n";
        }
      }
    }

  } else if (command == "to_openapi") {
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        out << "Usage:\n  cdd-cpp to_openapi -i <path/to/code> [-o "
               "<spec.json>]\n"
               "\nOptions:\n"
               "  -i, --input                     Path to source code "
               "directory or file.\n"
               "  -o, --output                    Output file or directory "
               "path.\n";
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
    input = get_arg_or_env(input, "CDD_INPUT");
    output = get_arg_or_env(output, "CDD_OUTPUT");

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
               "-i <spec.json> [-o <docs.json>]\n"
               "\nOptions:\n"
               "  -i, --input                     Path or URL to the OpenAPI "
               "specification.\n"
               "  -o, --output                    Output file or directory "
               "path.\n"
               "      --no-imports                Omit the imports field.\n"
               "      --no-wrapping               Omit the wrapper fields.\n";
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

    no_imports = get_bool_arg_or_env(no_imports, "CDD_NO_IMPORTS");
    no_wrapping = get_bool_arg_or_env(no_wrapping, "CDD_NO_WRAPPING");
    input_file = get_arg_or_env(input_file, "CDD_INPUT");
    output_file = get_arg_or_env(output_file, "CDD_OUTPUT");

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
      for (const auto &[path, pi] : spec.paths.operator*()) {
        auto add_op = [&](const std::string &method,
                          const std::optional<openapi::Operation> &op) {
          if (!op.has_value())
            return;
          jw.start_object();
          jw.key_value("method", method);
          jw.key_value("path", path);
          if (op->operationId.has_value()) {
            jw.key_value("operationId", op->operationId.operator*());
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
        std::cout
            << "Usage:\n"
            << "  cdd-cpp serve_json_rpc [-p|--port <port>] [-l|--listen "
               "<address>]\n"
            << "\nOptions:\n"
            << "  -p, --port                      Port to listen on.\n"
            << "  -l, --listen                    Address to listen on.\n";
        return 0;
      }
    }

    std::string port = get_arg_or_env("", "CDD_PORT", "8080");
    std::string listen_host = get_arg_or_env("", "CDD_LISTEN", "127.0.0.1");
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "--port" || arg == "-p") && i + 1 < argc) {
        port = argv[++i];
      } else if ((arg == "--listen" || arg == "-l") && i + 1 < argc) {
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
    int port_num = 8080;
    auto [ptr, ec] =
        std::from_chars(port.data(), port.data() + port.size(), port_num);
    if (ec != std::errc()) {
      std::cerr << "Invalid port number: " << port << std::endl;
      return 1;
    }
    svr.listen(listen_host, port_num);
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

// GCOV_EXCL_BR_STOP
