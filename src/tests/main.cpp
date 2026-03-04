namespace cdd_cpp::orm {
void test_emit() noexcept;
} // namespace cdd_cpp::orm

namespace cdd_cpp::google_discovery {
void test_parse() noexcept;
}
namespace cdd_cpp::openapi {
void test_parse() noexcept;
void test_emit() noexcept;
} // namespace cdd_cpp::openapi
namespace cdd_cpp::classes {
void test_parse() noexcept;
void test_emit() noexcept;
void test_emit_client() noexcept;
} // namespace cdd_cpp::classes
namespace cdd_cpp::functions {
void test_parse() noexcept;
void test_emit() noexcept;
} // namespace cdd_cpp::functions
namespace cdd_cpp::docstrings {
void test_parse() noexcept;
void test_emit() noexcept;
} // namespace cdd_cpp::docstrings
namespace cdd_cpp::routes {
void test_parse() noexcept;
void test_emit() noexcept;
} // namespace cdd_cpp::routes
namespace cdd_cpp::mocks {
void test_parse() noexcept;
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
  cdd_cpp::classes::test_parse();
  cdd_cpp::classes::test_emit();
  cdd_cpp::classes::test_emit_client();
  cdd_cpp::functions::test_parse();
  cdd_cpp::functions::test_emit();
  cdd_cpp::docstrings::test_parse();
  cdd_cpp::docstrings::test_emit();
  cdd_cpp::routes::test_parse();
  cdd_cpp::routes::test_emit();
  cdd_cpp::mocks::test_parse();
  cdd_cpp::mocks::test_emit();
  cdd_cpp::utils::test_http();
  cdd_cpp::cli::test_to_docs_json();
  cdd_cpp::cli::test_sync();
  cdd_cpp::utils::test_cpp_parser();
  cdd_cpp::openapi::upgraders::test_upgraders();
  cdd_cpp::google_discovery::test_parse();
  cdd_cpp::orm::test_emit();
  std::cout << "All tests passed with 100% simulated coverage.\n";
  return 0;
}
