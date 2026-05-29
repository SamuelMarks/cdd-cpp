#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace cdd_cpp::cli {

struct FromOpenApiConfig {
  std::string input;
  std::string input_dir;
  std::string output;
  bool no_github_actions = false;
  bool no_installable_package = false;
  bool tests = false;
  std::string subcommand = "to_sdk"; // to_sdk, to_sdk_cli, to_server
};

struct ToOpenApiConfig {
  std::string input;
  std::string output;
};

struct DocsJsonConfig {
  std::string input;
  std::string output;
  bool no_imports = false;
  bool no_wrapping = false;
};

struct ServeJsonRpcConfig {
  int port = 8080;
  std::string listen_host = "127.0.0.1";
};

/// @brief Generate code from an OpenAPI specification.
int generate_from_openapi(const FromOpenApiConfig &config,
                          std::ostream &out = std::cout,
                          std::ostream &err = std::cerr);

/// @brief Generate an OpenAPI specification from source code.
int generate_to_openapi(const ToOpenApiConfig &config,
                        std::ostream &out = std::cout,
                        std::ostream &err = std::cerr);

/// @brief Generate JSON documentation with code snippets for an OpenAPI
/// specification.
int generate_docs_json(const DocsJsonConfig &config,
                       std::ostream &out = std::cout,
                       std::ostream &err = std::cerr);

/// @brief Expose CLI interface as a JSON-RPC server.
int serve_json_rpc(const ServeJsonRpcConfig &config,
                   std::ostream &out = std::cout,
                   std::ostream &err = std::cerr);

} // namespace cdd_cpp::cli
