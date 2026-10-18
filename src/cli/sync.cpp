/// @file sync.cpp
/// @brief Implementation of CLI subcommand handlers and bidirectional C++
/// project / OpenAPI synchronization.

#include "../arazzo/emit.hpp"
#include "../arazzo/parse.hpp"
#include "../client_sdk/emit.hpp"
#include "../client_sdk_cli/emit.hpp"
#include "../google_discovery/parse.hpp"
#include "../mocks/emit.hpp"
#include "../models/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../openapi/upgraders/upgrader.hpp"
#include "../orm/emit.hpp"
#include "../server/emit.hpp"
#include "../utils/cpp_parser.hpp"
#include "../utils/json_writer.hpp"
#include "api.hpp"
#include <charconv>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#if !defined(__wasi__) && !defined(__EMSCRIPTEN__)
#include "../utils/httplib.h"
#endif

namespace cdd_cpp::cli {

/// @brief Print top-level CLI help usage.
/// @param out Output stream for help text.
static void print_help(std::ostream &out) noexcept {
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
         "  cdd-cpp sync -i <dir> -o <file>\n"
         "  cdd-cpp mcp\n"
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
         "  sync         : Synchronize an OpenAPI specification with source "
         "code.\n"
         "  mcp          : Start a Model Context Protocol (MCP) STDIO server.\n"
         "\nOptions:\n"
         "  -i, --input                         Path or URL to the OpenAPI "
         "specification.\n"
         "  -d, --input-dir                 Directory containing OpenAPI "
         "specifications.\n"
         "  -o, --output                    Output file or directory path.\n"
         "  -m, --mcp                       Generate MCP server.\n"
         "      --tests                     Generate integration tests and "
         "mocks.\n"
         "      --no-github-actions         Do not generate GitHub Actions "
         "scaffolding.\n"
         "      --no-installable-package    Do not generate installable "
         "package scaffolding.\n"
         "      --no-imports                Omit the imports field.\n"
         "      --no-wrapping               Omit the wrapper fields.\n"
         "  -h, --help                      Show this help message.\n"
         "  -v, --version                   Show version information.\n";
}

/// @brief Print CLI version information.
/// @param out Output stream for version text.
static void print_version(std::ostream &out) noexcept {
  out << "cdd-cpp version 0.0.4\n";
}

/// @brief Read file contents into a string.
/// @param path File path to read.
/// @return Result containing file contents or FileReadError.
static utils::Result<std::string> read_file(const std::string &path) noexcept {
  std::ifstream fs(path);
  if (!fs.is_open())
    return utils::make_error(utils::ErrorCode::FileReadError,
                             "Could not open input file: " + path, path);
  std::string content((std::istreambuf_iterator<char>(fs)),
                      std::istreambuf_iterator<char>());
  return content;
}

/// @brief Retrieve string argument or fallback to environment variable.
/// @param val Explicit CLI argument value.
/// @param env_var Environment variable name.
/// @param def Default value if neither is set.
/// @return Resolved string value.
static std::string get_arg_or_env(const std::string &val,
                                  const std::string &env_var,
                                  const std::string &def = "") noexcept {
  if (!val.empty())
    return val;
  if (const char *env_p = std::getenv(env_var.c_str()))
    return std::string(env_p);
  return def;
}

/// @brief Retrieve boolean flag or fallback to environment variable.
/// @param val Explicit CLI boolean flag.
/// @param env_var Environment variable name.
/// @return Resolved boolean value.
static bool get_bool_arg_or_env(bool val, const std::string &env_var) noexcept {
  if (val)
    return true;
  if (const char *env_p = std::getenv(env_var.c_str())) {
    std::string s(env_p);
    return s == "1" || s == "true" || s == "TRUE";
  }
  return false;
}

/// @brief Check whether any CLI argument from index 2 onward requests help.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @return True if `--help` or `-h` is present.
static bool has_help_flag(int argc, char **argv) noexcept {
  for (int i = 2; i < argc; ++i) {
    std::string_view arg(argv[i]);
    if (arg == "--help" || arg == "-h")
      return true;
  }
  return false;
}

/// @brief Match a CLI option flag with a value argument (`-x val` or `--long
/// val`).
/// @param arg Current argument string.
/// @param short_flag Short option flag (e.g. "-i").
/// @param long_flag Long option flag (e.g. "--input").
/// @param i Loop index (incremented if matched).
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out_val Destination string to populate on match.
/// @return True if matched and value extracted.
static bool match_opt(std::string_view arg, std::string_view short_flag,
                      std::string_view long_flag, int &i, int argc, char **argv,
                      std::string &out_val) noexcept {
  if ((arg == short_flag || arg == long_flag) && i + 1 < argc) {
    out_val = argv[++i];
    return true;
  }
  return false;
}

/// @brief Write a map of relative filenames and contents to a base directory.
/// @param files Map of relative file path to file content.
/// @param base_dir Target directory path.
/// @param out Output stream for status messages.
/// @param err Error stream for diagnostic messages.
/// @return 0 on success, 1 on file open error.
static int
write_generated_files(const std::map<std::string, std::string> &files,
                      const std::string &base_dir, std::ostream &out,
                      std::ostream &err) noexcept {
  for (const auto &[fname, content] : files) {
    std::string out_path = base_dir + "/" + fname;
    std::error_code ec;
    std::filesystem::create_directories(
        std::filesystem::path(out_path).parent_path(), ec);
    std::ofstream out_file(out_path);
    if (!out_file) {
      err << "Could not open output file: " << out_path << "\n";
      return 1;
    }
    out_file << content;
    out << "Successfully generated " << out_path << "\n";
  }
  return 0;
}

int mcp_command_help(std::ostream &out) noexcept {
  out << "Usage:\n"
      << "  cdd-cpp mcp\n\n"
      << "Start a Model Context Protocol (MCP) STDIO server.\n";
  return 0;
}

std::string handle_mcp_cli_message(const std::string &request_json) noexcept {
  return cdd_cpp::server::serve_json_rpc(request_json);
}

int mcp(std::ostream &out, std::ostream &err, std::istream &in) noexcept {
  (void)err;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty())
      continue;
    out << handle_mcp_cli_message(line) << "\n";
  }
  return 0;
}

int from_openapi(const FromOpenApiConfig &config, std::ostream &out,
                 std::ostream &err) noexcept {
  if (config.input.empty() && config.input_dir.empty()) {
    err << "Missing -i <spec.json> or --input-dir <specs_dir>\n";
    return 1;
  }

  std::string content;
  if (!config.input.empty()) {
    auto content_res = read_file(config.input);
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
  const auto &spec = *spec_res;

  std::map<std::string, std::string> multiple_files;

  if (config.subcommand == "to_sdk_cli") {
    multiple_files =
        client_sdk_cli::emit_cli(spec, config.no_github_actions,
                                 config.no_installable_package, config.tests);
  } else if (config.subcommand == "to_sdk") {
    multiple_files =
        client_sdk::emit_client(spec, config.no_github_actions,
                                config.no_installable_package, config.tests);
  } else if (config.subcommand == "to_server") {
    auto model_files = cdd_cpp::models::emit_modular(spec);
    multiple_files.insert(model_files.begin(), model_files.end());

    auto orm_files = cdd_cpp::orm::emit_modular(spec, config.with_postgres,
                                                config.with_faker);
    multiple_files.insert(orm_files.begin(), orm_files.end());

    auto server_files = cdd_cpp::server::emit_modular(
        spec, config.no_installable_package, config.tests, config.with_postgres,
        config.with_faker);
    multiple_files.insert(server_files.begin(), server_files.end());

    if (!config.no_github_actions) {
      std::string ci_content =
          "name: CI\non: [push]\njobs:\n  build:\n    runs-on: "
          "ubuntu-latest\n    steps:\n      - uses: actions/checkout@v6\n    "
          "  - run: cmake . && cmake --build .\n";
      if (config.tests)
        ci_content += "      - run: cd tests && ctest\n";
      multiple_files[".github/workflows/ci.yml"] = ci_content;
    }
  } else {
    err << "Unknown subcommand: " << config.subcommand << "\n";
    return 1;
  }

  return write_generated_files(multiple_files, config.output, out, err);
}

int from_google_discovery(const FromGoogleDiscoveryConfig &config,
                          std::ostream &out, std::ostream &err) noexcept {
  if (config.input.empty()) {
    err << "Missing -i <discovery.json>\n";
    return 1;
  }

  auto content_res = read_file(config.input);
  if (!content_res) {
    err << "Error: " << content_res.error() << "\n";
    return 1;
  }

  auto apis_res = google_discovery::parse(*content_res);
  if (!apis_res) {
    err << "Error: " << apis_res.error() << "\n";
    return 1;
  }

  for (const auto &spec : *apis_res) {
    std::map<std::string, std::string> multiple_files;

    if (config.subcommand == "to_sdk_cli") {
      multiple_files =
          client_sdk_cli::emit_cli(spec, config.no_github_actions,
                                   config.no_installable_package, config.tests);
    } else if (config.subcommand == "to_sdk") {
      multiple_files =
          client_sdk::emit_client(spec, config.no_github_actions,
                                  config.no_installable_package, config.tests);
    } else {
      err << "Unknown subcommand: " << config.subcommand << "\n";
      return 1;
    }

    std::string api_output = config.output;
    if (apis_res->size() > 1) {
      api_output += "/" + spec.info.title;
    }
    if (write_generated_files(multiple_files, api_output, out, err) != 0) {
      return 1;
    }
  }
  return 0;
}

int to_openapi(const ToOpenApiConfig &config, std::ostream &out,
               std::ostream &err) noexcept {
  if (config.input.empty()) {
    err << "Missing -i <path/to/code>\n";
    return 1;
  }
  auto spec = utils::parse_cpp_project(config.input);
  std::string spec_str = openapi::emit(spec);
  if (config.output.empty()) {
    out << spec_str << "\n";
  } else {
    std::ofstream fs(config.output);
    if (!fs.is_open()) {
      err << "Error: "
          << utils::Error(utils::ErrorCode::FileWriteError,
                          "Could not open output file: " + config.output,
                          config.output)
          << "\n";
      return 1;
    }
    fs << spec_str << "\n";
  }
  return 0;
}

int from_arazzo(const FromArazzoConfig &config, std::ostream &out,
                std::ostream &err) noexcept {
  if (config.input.empty()) {
    err << "Missing -i <spec.arazzo.json>\n";
    return 1;
  }

  auto content_res = read_file(config.input);
  if (!content_res) {
    err << "Error: " << content_res.error() << "\n";
    return 1;
  }

  auto spec_res = arazzo::parse(*content_res);
  if (!spec_res) {
    err << "Error: " << spec_res.error() << "\n";
    return 1;
  }
  const auto &spec = *spec_res;

  std::map<std::string, std::string> multiple_files;
  if (config.subcommand == "to_sdk") {
    multiple_files = client_sdk::emit_arazzo_client(
        spec, std::nullopt, config.no_github_actions,
        config.no_installable_package, config.tests);
  } else if (config.subcommand == "to_sdk_cli") {
    multiple_files = client_sdk_cli::emit_arazzo_cli(
        spec, std::nullopt, config.no_github_actions,
        config.no_installable_package, config.tests);
  } else if (config.subcommand == "to_server") {
    multiple_files = server::emit_arazzo_modular(
        spec, config.no_installable_package, config.tests);
  } else {
    err << "Unknown subcommand: " << config.subcommand << "\n";
    return 1;
  }

  return write_generated_files(multiple_files, config.output, out, err);
}

int to_arazzo(const ToArazzoConfig &config, std::ostream &out,
              std::ostream &err) noexcept {
  if (config.input.empty()) {
    err << "Missing -i <path/to/code>\n";
    return 1;
  }
  auto spec = utils::parse_cpp_project_arazzo(config.input);
  std::string spec_str = arazzo::emit(spec);
  if (config.output.empty()) {
    out << spec_str << "\n";
  } else {
    std::ofstream fs(config.output);
    if (!fs.is_open()) {
      err << "Error: "
          << utils::Error(utils::ErrorCode::FileWriteError,
                          "Could not open output file: " + config.output,
                          config.output)
          << "\n";
      return 1;
    }
    fs << spec_str << "\n";
  }
  return 0;
}

int to_docs_json(const DocsJsonConfig &config, std::ostream &out,
                 std::ostream &err) noexcept {
  if (config.input.empty()) {
    err << "Missing -i <spec.json>\n";
    return 1;
  }

  auto content_res = read_file(config.input);
  if (!content_res) {
    err << "Error: " << content_res.error() << "\n";
    return 1;
  }
  auto spec_res = openapi::parse(*content_res);
  if (!spec_res) {
    err << "Error: " << spec_res.error() << "\n";
    return 1;
  }
  const auto &spec = *spec_res;

  utils::JsonWriter jw;
  jw.start_array();
  jw.start_object();
  jw.key_value("language", "cpp");
  jw.key("operations");
  jw.start_array();

  if (spec.paths.has_value()) {
    for (const auto &[path, pi] : *spec.paths) {
      auto add_op = [&](const std::string &method,
                        const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        jw.start_object();
        jw.key_value("method", method);
        jw.key_value("path", path);
        if (op->operationId.has_value()) {
          jw.key_value("operationId", *op->operationId);
        }
        jw.key("code");
        jw.start_object();

        if (!config.no_imports) {
          jw.key_value("imports",
                       "#include <iostream>\n#include <cdd_client.hpp>");
        }
        if (!config.no_wrapping) {
          jw.key_value("wrapper_start",
                       "int main() {\n    cdd_cpp::Client client;\n");
        }

        std::string op_name = op->operationId.value_or("request");
        jw.key_value("snippet", "    auto res = client." + op_name +
                                    "();\n    out << res << \"\\n\";");

        if (!config.no_wrapping) {
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

  if (config.output.empty()) {
    out << jw.str() << "\n";
  } else {
    std::ofstream fs(config.output);
    if (!fs.is_open()) {
      err << "Error: "
          << utils::Error(utils::ErrorCode::FileWriteError,
                          "Could not open output file: " + config.output,
                          config.output)
          << "\n";
      return 1;
    }
    fs << jw.str() << "\n";
  }
  return 0;
}

int serve_json_rpc(const ServeJsonRpcConfig &config, std::ostream &out,
                   std::ostream &err) noexcept {
#if !defined(__wasi__) && !defined(__EMSCRIPTEN__)
  (void)err;
  httplib::Server svr;

  svr.Post("/", [](const httplib::Request &req, httplib::Response &res) {
    std::string result = cdd_cpp::server::serve_json_rpc(req.body);
    res.set_content(result, "application/json");
    res.set_header("Access-Control-Allow-Origin", "*");
  });

  svr.Options("/", [](const httplib::Request &, httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_content("", "text/plain");
  });

  svr.Get("/stop",
          [&](const httplib::Request &, httplib::Response &) { svr.stop(); });

  out << "JSON RPC server listening on " << config.listen_host << ":"
      << config.port << std::endl;
  svr.listen(config.listen_host, config.port);
  return 0;
#else
  (void)config;
  (void)out;
  err << "serve_json_rpc is not supported in WASI builds." << std::endl;
  return 1;
#endif
}

int sync(const SyncConfig &config, std::ostream &out,
         std::ostream &err) noexcept {
  out << "Syncing code in " << config.input << " with spec " << config.output
      << "...\n";
  auto cpp_spec = utils::parse_cpp_project(config.input);

  std::ifstream fs(config.output);
  openapi::OpenAPI merged_spec = cpp_spec;
  if (fs) {
    std::string content((std::istreambuf_iterator<char>(fs)),
                        std::istreambuf_iterator<char>());
    auto file_spec_res = openapi::parse(content);
    if (file_spec_res) {
      if (config.truth == "spec") {
        merged_spec = *file_spec_res;
        if (!merged_spec.components)
          merged_spec.components = openapi::Components{};
        if (!merged_spec.components->schemas)
          merged_spec.components->schemas =
              std::map<std::string, openapi::Schema>{};
        for (const auto &[k, v] : *cpp_spec.components->schemas) {
          if (!merged_spec.components->schemas->contains(k)) {
            (*merged_spec.components->schemas)[k] = v;
          }
        }
      } else {
        merged_spec.info = file_spec_res->info;
      }
    } else {
      err << "Warning: could not parse existing spec at " << config.output
          << ": " << file_spec_res.error().message << "\n";
    }
  }

  std::ofstream out_fs(config.output);
  if (!out_fs) {
    err << "Error: Could not write to output spec file " << config.output
        << "\n";
    return 1;
  }
  out_fs << openapi::emit(merged_spec) << "\n";
  out << "Updated OpenAPI spec at " << config.output << "\n";

  std::string mock_code = mocks::emit(merged_spec);
  std::ofstream mock_fs(config.input + "/mocks_generated.hpp");
  mock_fs << mock_code << "\n";

  auto client_files = client_sdk::emit_client(merged_spec);
  if (write_generated_files(client_files, config.input, out, err) != 0) {
    return 1;
  }

  out << "Updated mocks and clients in " << config.input << "\n";
  return 0;
}

int run_mcp(int argc, char **argv, std::ostream &out, std::ostream &err,
            std::istream &in) noexcept {
  if (has_help_flag(argc, argv)) {
    return mcp_command_help(out);
  }
  return mcp(out, err, in);
}

int run_sync(int argc, char **argv, std::ostream &out,
             std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n  cdd-cpp sync -i <dir> -o <file> [-t|--truth "
           "<code|spec>]\n"
           "\nOptions:\n"
           "  -i, --input                         Path to source code "
           "directory.\n"
           "  -o, --output                    Path to OpenAPI spec file.\n"
           "  -t, --truth                     Source of truth (code or "
           "spec).\n";
    return 0;
  }
  SyncConfig config;
  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    } else if (match_opt(arg, "-t", "--truth", i, argc, argv, config.truth)) {
    }
  }
  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT");
  if (config.input.empty() || config.output.empty()) {
    err << "Missing --input or --output\n";
    return 1;
  }
  return sync(config, out, err);
}

int run_from_openapi(int argc, char **argv, std::ostream &out,
                     std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
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
        << "  -i, --input                         Path or URL to the "
           "OpenAPI specification.\n"
        << "  -d, --input-dir                 Directory containing OpenAPI "
           "specifications.\n"
        << "  -o, --output                    Output file or directory "
           "path.\n"
        << "  -m, --mcp                       Generate MCP server.\n"
        << "      --tests                     Generate integration tests "
           "and mocks.\n"
        << "      --no-github-actions         Do not generate GitHub "
           "Actions scaffolding.\n"
        << "      --no-installable-package    Do not generate installable "
           "package scaffolding.\n";
    return 0;
  }

  if (argc < 3) {
    err << "Missing subcommand for from_openapi\n";
    return 1;
  }

  FromOpenApiConfig config;
  config.subcommand = argv[2];
  config.output = "";

  for (int i = 3; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-d", "--input-dir", i, argc, argv,
                         config.input_dir)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    } else if (arg == "-m" || arg == "--mcp") {
      config.mcp = true;
    } else if (arg == "--no-github-actions") {
      config.no_github_actions = true;
    } else if (arg == "--no-installable-package") {
      config.no_installable_package = true;
    } else if (arg == "--tests") {
      config.tests = true;
    } else if (arg == "--with-postgres") {
      config.with_postgres = true;
    } else if (arg == "--with-faker") {
      config.with_faker = true;
    }
  }

  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.input_dir = get_arg_or_env(config.input_dir, "CDD_INPUT_DIR");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT", ".");
  config.no_github_actions =
      get_bool_arg_or_env(config.no_github_actions, "CDD_NO_GITHUB_ACTIONS");
  config.no_installable_package = get_bool_arg_or_env(
      config.no_installable_package, "CDD_NO_INSTALLABLE_PACKAGE");
  config.tests = get_bool_arg_or_env(config.tests, "CDD_TESTS");

  return from_openapi(config, out, err);
}

int run_from_google_discovery(int argc, char **argv, std::ostream &out,
                              std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n"
        << "  cdd-cpp from_google_discovery to_sdk_cli -i <discovery.json> "
           "-o <target_directory> [--no-github-actions] "
           "[--no-installable-package] [--tests]\n"
        << "  cdd-cpp from_google_discovery to_sdk -i <discovery.json> -o "
           "<target_directory> [--no-github-actions] "
           "[--no-installable-package] [--tests]\n";
    return 0;
  }

  if (argc < 3) {
    err << "Missing subcommand for from_google_discovery\n";
    return 1;
  }

  FromGoogleDiscoveryConfig config;
  config.subcommand = argv[2];
  config.output = "";

  for (int i = 3; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    } else if (arg == "--no-github-actions") {
      config.no_github_actions = true;
    } else if (arg == "--no-installable-package") {
      config.no_installable_package = true;
    } else if (arg == "--tests") {
      config.tests = true;
    }
  }

  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT", ".");
  config.no_github_actions =
      get_bool_arg_or_env(config.no_github_actions, "CDD_NO_GITHUB_ACTIONS");
  config.no_installable_package = get_bool_arg_or_env(
      config.no_installable_package, "CDD_NO_INSTALLABLE_PACKAGE");
  config.tests = get_bool_arg_or_env(config.tests, "CDD_TESTS");

  return from_google_discovery(config, out, err);
}

int run_to_openapi(int argc, char **argv, std::ostream &out,
                   std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n  cdd-cpp to_openapi -i <path/to/code> [-o "
           "<spec.json>]\n"
           "\nOptions:\n"
           "  -i, --input                         Path to source code "
           "directory or file.\n"
           "  -o, --output                    Output file or directory "
           "path.\n";
    return 0;
  }

  ToOpenApiConfig config;
  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    }
  }
  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT");

  return to_openapi(config, out, err);
}

int run_from_arazzo(int argc, char **argv, std::ostream &out,
                    std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n"
        << "  cdd-cpp from_arazzo to_sdk -i <spec.arazzo.json> -o "
           "<target_directory> [--no-github-actions] "
           "[--no-installable-package] [--tests]\n"
        << "  cdd-cpp from_arazzo to_sdk_cli -i <spec.arazzo.json> -o "
           "<target_directory> [--no-github-actions] "
           "[--no-installable-package] [--tests]\n"
        << "  cdd-cpp from_arazzo to_server -i <spec.arazzo.json> -o "
           "<target_directory> [--no-installable-package] [--tests]\n";
    return 0;
  }
  if (argc < 3) {
    err << "Missing subcommand for from_arazzo\n";
    return 1;
  }
  FromArazzoConfig config;
  config.subcommand = argv[2];
  config.output = "";
  for (int i = 3; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    } else if (arg == "--no-github-actions") {
      config.no_github_actions = true;
    } else if (arg == "--no-installable-package") {
      config.no_installable_package = true;
    } else if (arg == "--tests") {
      config.tests = true;
    }
  }
  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT", ".");
  return from_arazzo(config, out, err);
}

int run_to_arazzo(int argc, char **argv, std::ostream &out,
                  std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n  cdd-cpp to_arazzo -i <path/to/code> [-o "
           "<spec.arazzo.json>]\n";
    return 0;
  }
  ToArazzoConfig config;
  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    }
  }
  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT");
  return to_arazzo(config, out, err);
}

int run_to_docs_json(int argc, char **argv, std::ostream &out,
                     std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] "
           "-i <spec.json> [-o <docs.json>]\n"
           "\nOptions:\n"
           "  -i, --input                         Path or URL to the "
           "OpenAPI specification.\n"
           "  -o, --output                    Output file or directory "
           "path.\n"
           "      --no-imports                Omit the imports field.\n"
           "      --no-wrapping               Omit the wrapper fields.\n";
    return 0;
  }

  DocsJsonConfig config;
  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg == "--no-imports") {
      config.no_imports = true;
    } else if (arg == "--no-wrapping") {
      config.no_wrapping = true;
    } else if (match_opt(arg, "-i", "--input", i, argc, argv, config.input)) {
    } else if (match_opt(arg, "-o", "--output", i, argc, argv, config.output)) {
    }
  }

  config.no_imports = get_bool_arg_or_env(config.no_imports, "CDD_NO_IMPORTS");
  config.no_wrapping =
      get_bool_arg_or_env(config.no_wrapping, "CDD_NO_WRAPPING");
  config.input = get_arg_or_env(config.input, "CDD_INPUT");
  config.output = get_arg_or_env(config.output, "CDD_OUTPUT");

  return to_docs_json(config, out, err);
}

int run_serve_json_rpc(int argc, char **argv, std::ostream &out,
                       std::ostream &err) noexcept {
  if (has_help_flag(argc, argv)) {
    out << "Usage:\n"
        << "  cdd-cpp serve_json_rpc [-p|--port <port>] [-l|--listen "
           "<address>]\n"
        << "\nOptions:\n"
        << "  -p, --port                      Port to listen on.\n"
        << "  -l, --listen                    Address to listen on.\n";
    return 0;
  }

  std::string port = get_arg_or_env("", "CDD_PORT", "8080");
  std::string listen_host = get_arg_or_env("", "CDD_LISTEN", "127.0.0.1");
  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (match_opt(arg, "-p", "--port", i, argc, argv, port)) {
    } else if (match_opt(arg, "-l", "--listen", i, argc, argv, listen_host)) {
    }
  }

  int port_num = 8080;
  auto [ptr, ec] =
      std::from_chars(port.data(), port.data() + port.size(), port_num);
  if (ec != std::errc()) {
    err << "Invalid port number: " << port << std::endl;
    return 1;
  }

  ServeJsonRpcConfig config;
  config.port = port_num;
  config.listen_host = listen_host;
  return serve_json_rpc(config, out, err);
}

int main_impl(int argc, char **argv, std::ostream &out, std::ostream &err,
              std::istream &in) noexcept {
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
  if (command == "mcp") {
    return run_mcp(argc, argv, out, err, in);
  }
  if (command == "sync") {
    return run_sync(argc, argv, out, err);
  }
  if (command == "from_openapi") {
    return run_from_openapi(argc, argv, out, err);
  }
  if (command == "from_google_discovery") {
    return run_from_google_discovery(argc, argv, out, err);
  }
  if (command == "to_openapi") {
    return run_to_openapi(argc, argv, out, err);
  }
  if (command == "from_arazzo") {
    return run_from_arazzo(argc, argv, out, err);
  }
  if (command == "to_arazzo") {
    return run_to_arazzo(argc, argv, out, err);
  }
  if (command == "to_docs_json") {
    return run_to_docs_json(argc, argv, out, err);
  }
  if (command == "serve_json_rpc") {
    return run_serve_json_rpc(argc, argv, out, err);
  }

  err << "Unknown command: " << command << "\n";
  print_help(out);
  return 1;
}

} // namespace cdd_cpp::cli
