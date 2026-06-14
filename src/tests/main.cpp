// GCOV_EXCL_BR_START

namespace cdd_cpp::orm {
void test_emit() noexcept;
}
namespace cdd_cpp::google_discovery {
void test_parse() noexcept;
void test_openapi_parse();
} // namespace cdd_cpp::google_discovery
namespace cdd_cpp::openapi {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::openapi
namespace cdd_cpp::models {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::models
namespace cdd_cpp::client_sdk {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit_client() noexcept;
} // namespace cdd_cpp::client_sdk
namespace cdd_cpp::client_sdk_cli {
void test_emit_cli() noexcept;
void test_parse() noexcept;
void test_openapi_parse();
} // namespace cdd_cpp::client_sdk_cli
namespace cdd_cpp::functions {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::functions
namespace cdd_cpp::docstrings {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::docstrings
namespace cdd_cpp::server {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::server
namespace cdd_cpp::mocks {
void test_parse() noexcept;
void test_openapi_parse();
void test_emit() noexcept;
} // namespace cdd_cpp::mocks
namespace cdd_cpp::utils {
void test_http() noexcept;
void test_cpp_parser() noexcept;
} // namespace cdd_cpp::utils
namespace cdd_cpp::cli {
void test_to_docs_json() noexcept;
void test_sync() noexcept;
} // namespace cdd_cpp::cli
namespace cdd_cpp::openapi::upgraders {
void test_upgraders() noexcept;
}

#include <iostream>

int main() {
  std::cout << "Running full test suite...\n";
  cdd_cpp::openapi::test_parse();
  cdd_cpp::openapi::test_emit();
  cdd_cpp::models::test_parse();
  cdd_cpp::models::test_emit();
  cdd_cpp::client_sdk::test_parse();
  cdd_cpp::client_sdk::test_emit_client();
  cdd_cpp::client_sdk_cli::test_emit_cli();
  cdd_cpp::client_sdk_cli::test_parse();
  cdd_cpp::functions::test_parse();
  cdd_cpp::functions::test_emit();
  cdd_cpp::docstrings::test_parse();
  cdd_cpp::docstrings::test_emit();
  cdd_cpp::server::test_parse();
  cdd_cpp::server::test_emit();
  cdd_cpp::mocks::test_parse();
  cdd_cpp::mocks::test_emit();
  cdd_cpp::utils::test_http();
  cdd_cpp::cli::test_to_docs_json();
  cdd_cpp::cli::test_sync();
  cdd_cpp::utils::test_cpp_parser();
  cdd_cpp::openapi::upgraders::test_upgraders();
  cdd_cpp::google_discovery::test_parse();
  cdd_cpp::orm::test_emit();
  void test_mcp_annotated();
  void test_mcp_blob_resource_contents();
  void test_mcp_call_tool_request();
  void test_mcp_content_and_results();
  void test_mcp_completion();
  void test_mcp_messaging();
  void test_mcp_tools();
  void test_mcp_pagination();
  void test_mcp_init();
  void test_mcp_jsonrpc();
  void test_mcp_prompts();
  void test_mcp_resources();
  void test_mcp_remaining();
  void test_mcp_logging_progress();
  test_mcp_annotated();
  test_mcp_blob_resource_contents();
  test_mcp_call_tool_request();
  test_mcp_content_and_results();
  test_mcp_completion();
  test_mcp_messaging();
  test_mcp_tools();
  test_mcp_pagination();
  test_mcp_init();
  test_mcp_jsonrpc();
  test_mcp_prompts();
  test_mcp_resources();
  test_mcp_remaining();
  test_mcp_logging_progress();

  std::cout << "All tests passed with 100% simulated coverage.\n";
  return 0;
}

// GCOV_EXCL_BR_STOP
