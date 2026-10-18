/// @file api.hpp
/// @brief CLI configuration structures and top-level command entrypoints.

#pragma once
#include <iostream>
#include <string>
#include <vector>

/// @namespace cdd_cpp::cli
/// @brief Command-line interface handlers, configuration structs, and
/// synchronization commands.
namespace cdd_cpp::cli {

/// @brief Configuration for generating code from OpenAPI.
struct FromOpenApiConfig {
  std::string input;              ///< Input OpenAPI file path.
  std::string input_dir;          ///< Input directory containing OpenAPI specs.
  std::string output = ".";       ///< Output directory path.
  std::string truth;              ///< Source of truth ("spec" or "code").
  bool no_github_actions = false; ///< Omit `.github/workflows/ci.yml`.
  bool no_installable_package = false; ///< Omit `CMakeLists.txt`.
  bool tests = false;                  ///< Generate GoogleTest unit test suite.
  bool mcp = false;                    ///< Enable MCP adapter emission.
  bool with_postgres = false; ///< Enable PostgreSQL libpq ORM emission.
  bool with_faker = false;    ///< Enable Faker mock data emission.
  std::string subcommand =
      "to_sdk"; ///< Subcommand ("to_sdk", "to_sdk_cli", "to_server").
};

/// @brief Configuration for generating code from Google Discovery JSON.
struct FromGoogleDiscoveryConfig {
  std::string input;                   ///< Input Google Discovery JSON path.
  std::string output = ".";            ///< Output directory path.
  bool no_github_actions = false;      ///< Omit `.github/workflows/ci.yml`.
  bool no_installable_package = false; ///< Omit `CMakeLists.txt`.
  bool tests = false;                  ///< Generate GoogleTest unit test suite.
  std::string subcommand = "to_sdk"; ///< Subcommand ("to_sdk" or "to_sdk_cli").
};

/// @brief Configuration for generating OpenAPI from source code.
struct ToOpenApiConfig {
  std::string input;  ///< Input C++ file or directory path.
  std::string output; ///< Output OpenAPI JSON file path.
  std::string truth;  ///< Source of truth ("code" or "spec").
};

/// @brief Configuration for generating code from an Arazzo 1.1.0 specification.
struct FromArazzoConfig {
  std::string input;                   ///< Input Arazzo JSON file path.
  std::string output = ".";            ///< Output directory path.
  bool no_github_actions = false;      ///< Omit `.github/workflows/ci.yml`.
  bool no_installable_package = false; ///< Omit `CMakeLists.txt`.
  bool tests = false;                  ///< Generate GoogleTest unit test suite.
  std::string subcommand =
      "to_sdk"; ///< Subcommand ("to_sdk", "to_sdk_cli", "to_server").
};

/// @brief Configuration for generating Arazzo 1.1.0 from C++ source code.
struct ToArazzoConfig {
  std::string input;  ///< Input C++ file or directory path.
  std::string output; ///< Output Arazzo JSON file path.
};

/// @brief Configuration for generating docs JSON.
struct DocsJsonConfig {
  std::string input;        ///< Input OpenAPI specification path or URL.
  std::string output;       ///< Output docs JSON path (empty for stdout).
  std::string truth;        ///< Source of truth.
  bool no_imports = false;  ///< Omit `#include` directives in code snippets.
  bool no_wrapping = false; ///< Omit `int main()` wrapper in code snippets.
};

/// @brief Configuration for serving JSON-RPC.
struct ServeJsonRpcConfig {
  int port = 8080; ///< TCP port to listen on.
  std::string listen_host =
      "127.0.0.1"; ///< Network interface address to bind to.
};

/// @brief Configuration for bidirectional code/specification synchronization.
struct SyncConfig {
  std::string input;  ///< Input C++ project directory path.
  std::string output; ///< OpenAPI specification JSON file path.
  std::string
      truth; ///< Source of truth ("code" or "spec", defaults to "code").
};

/// @brief Print help usage for the MCP subcommand.
/// @param out Output stream for help message.
/// @return Exit code (0).
int mcp_command_help(std::ostream &out = std::cout) noexcept;

/// @brief Handle a single MCP JSON-RPC message string.
/// @param request_json Raw JSON-RPC request string.
/// @return Serialized JSON-RPC response string.
std::string handle_mcp_cli_message(const std::string &request_json) noexcept;

/// @brief Generate code from an OpenAPI specification.
/// @param config Configuration parameters for code generation.
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int from_openapi(const FromOpenApiConfig &config, std::ostream &out = std::cout,
                 std::ostream &err = std::cerr) noexcept;

/// @brief Generate code from a Google Discovery specification.
/// @param config Configuration parameters for code generation.
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int from_google_discovery(const FromGoogleDiscoveryConfig &config,
                          std::ostream &out = std::cout,
                          std::ostream &err = std::cerr) noexcept;

/// @brief Generate an OpenAPI specification from source code.
/// @param config Configuration parameters for OpenAPI extraction.
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int to_openapi(const ToOpenApiConfig &config, std::ostream &out = std::cout,
               std::ostream &err = std::cerr) noexcept;

/// @brief Generate C++ code from an Arazzo 1.1.0 specification.
/// @param config Configuration parameters for Arazzo code generation.
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int from_arazzo(const FromArazzoConfig &config, std::ostream &out = std::cout,
                std::ostream &err = std::cerr) noexcept;

/// @brief Generate an Arazzo 1.1.0 specification from C++ source code.
/// @param config Configuration parameters for Arazzo extraction.
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int to_arazzo(const ToArazzoConfig &config, std::ostream &out = std::cout,
              std::ostream &err = std::cerr) noexcept;

/// @brief Generate JSON documentation with code snippets for an OpenAPI
/// specification.
/// @param config Configuration parameters for documentation generation.
/// @param out Output stream for JSON output.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int to_docs_json(const DocsJsonConfig &config, std::ostream &out = std::cout,
                 std::ostream &err = std::cerr) noexcept;

/// @brief Expose CLI interface as a JSON-RPC server.
/// @param config Server binding host and port configuration.
/// @param out Output stream for status messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int serve_json_rpc(const ServeJsonRpcConfig &config,
                   std::ostream &out = std::cout,
                   std::ostream &err = std::cerr) noexcept;

/// @brief Synchronize an OpenAPI specification with source code.
/// @param config Synchronization configuration (`input`, `output`, `truth`).
/// @param out Output stream for progress messages.
/// @param err Error stream for diagnostic messages.
/// @return Exit code (0 on success, non-zero on error).
int sync(const SyncConfig &config, std::ostream &out = std::cout,
         std::ostream &err = std::cerr) noexcept;

/// @brief Start a Model Context Protocol (MCP) STDIO server.
/// @param out Output stream for MCP JSON-RPC responses.
/// @param err Error stream for diagnostic messages.
/// @param in Input stream for MCP JSON-RPC requests.
/// @return Exit code (0 on success, non-zero on error).
int mcp(std::ostream &out = std::cout, std::ostream &err = std::cerr,
        std::istream &in = std::cin) noexcept;

/// @brief Parse and execute CLI subcommand `to_openapi`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_to_openapi(int argc, char **argv, std::ostream &out,
                   std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `from_openapi`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_from_openapi(int argc, char **argv, std::ostream &out,
                     std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `from_arazzo`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_from_arazzo(int argc, char **argv, std::ostream &out,
                    std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `to_arazzo`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_to_arazzo(int argc, char **argv, std::ostream &out,
                  std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `from_google_discovery`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_from_google_discovery(int argc, char **argv, std::ostream &out,
                              std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `to_docs_json`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_to_docs_json(int argc, char **argv, std::ostream &out,
                     std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `serve_json_rpc`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_serve_json_rpc(int argc, char **argv, std::ostream &out,
                       std::ostream &err) noexcept;

/// @brief Parse and execute CLI subcommand `mcp`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @param in Standard input stream.
/// @return Exit code (0 on success, non-zero on error).
int run_mcp(int argc, char **argv, std::ostream &out, std::ostream &err,
            std::istream &in = std::cin) noexcept;

/// @brief Parse and execute CLI subcommand `sync`.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @return Exit code (0 on success, non-zero on error).
int run_sync(int argc, char **argv, std::ostream &out,
             std::ostream &err) noexcept;

/// @brief Main in-process CLI dispatcher.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param out Standard output stream.
/// @param err Standard error stream.
/// @param in Standard input stream.
/// @return Exit code (0 on success, non-zero on error).
int main_impl(int argc, char **argv, std::ostream &out = std::cout,
              std::ostream &err = std::cerr,
              std::istream &in = std::cin) noexcept;

} // namespace cdd_cpp::cli
