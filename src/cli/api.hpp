#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace cdd_cpp::cli {

/// @brief Configuration for generating code from OpenAPI
struct FromOpenApiConfig {
  std::string input;
  std::string input_dir;
  std::string output;
  std::string truth;
  bool no_github_actions = false;
  bool no_installable_package = false;
  bool tests = false;
  bool mcp = false;
  std::string subcommand = "to_sdk"; // to_sdk, to_sdk_cli, to_server
};

/// @brief Configuration for generating OpenAPI from source code
struct ToOpenApiConfig {
  std::string input;
  std::string output;
  std::string truth;
};

/// @brief Configuration for generating docs JSON
struct DocsJsonConfig {
  std::string input;
  std::string output;
  std::string truth;
  bool no_imports = false;
  bool no_wrapping = false;
};

/// @brief Configuration for serving JSON-RPC
struct ServeJsonRpcConfig {
  int port = 8080;
  std::string listen_host = "127.0.0.1";
};

/// @brief Configuration for sync
struct SyncConfig {
  std::string input;
  std::string output;
  std::string truth;
};

/// @brief Generate code from an OpenAPI specification.
int from_openapi(const FromOpenApiConfig &config, std::ostream &out = std::cout,
                 std::ostream &err = std::cerr);

/// @brief Generate an OpenAPI specification from source code.
int to_openapi(const ToOpenApiConfig &config, std::ostream &out = std::cout,
               std::ostream &err = std::cerr);

/// @brief Generate JSON documentation with code snippets for an OpenAPI
/// specification.
int to_docs_json(const DocsJsonConfig &config, std::ostream &out = std::cout,
                 std::ostream &err = std::cerr);

/// @brief Expose CLI interface as a JSON-RPC server.
int serve_json_rpc(const ServeJsonRpcConfig &config,
                   std::ostream &out = std::cout,
                   std::ostream &err = std::cerr);

/// @brief Synchronize an OpenAPI specification with source code.
int sync(const SyncConfig &config, std::ostream &out = std::cout,
         std::ostream &err = std::cerr);

/// @brief Start a Model Context Protocol (MCP) STDIO server.
int mcp(std::ostream &out = std::cout, std::ostream &err = std::cerr,
        std::istream &in = std::cin);

} // namespace cdd_cpp::cli
