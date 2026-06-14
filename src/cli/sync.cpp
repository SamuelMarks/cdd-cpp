// GCOV_EXCL_BR_START

#include "../client_sdk/emit.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../utils/cpp_parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::cli {

void sync(const std::string &code_dir, const std::string &spec_file) noexcept {
  std::cout << "Syncing code in " << code_dir << " with spec " << spec_file
            << "...\n";
  // Parse C++ to get updated routes/mocks
  auto cpp_spec = utils::parse_cpp_project(code_dir);

  // Parse existing OpenAPI spec to merge
  std::ifstream fs(spec_file);
  openapi::OpenAPI merged_spec = cpp_spec;
  if (fs) {
    std::string content((std::istreambuf_iterator<char>(fs)),
                        std::istreambuf_iterator<char>());
    auto file_spec_res = openapi::parse(content);
    if (file_spec_res) {
      merged_spec.info = file_spec_res->info; // Keep original info
    }
  }

  // Update OpenAPI Spec file
  std::ofstream out_fs(spec_file);
  out_fs << openapi::emit(merged_spec) << "\n";
  std::cout << "Updated OpenAPI spec at " << spec_file << "\n";

  // Regenerate Mocks
  std::string mock_code = mocks::emit(merged_spec);
  std::ofstream mock_fs(code_dir + "/mocks_generated.hpp");
  mock_fs << mock_code << "\n";

  // Regenerate Client
  auto client_files = client_sdk::emit_client(merged_spec);
  for (const auto &[filename, content] : client_files) {
    std::string out_path = code_dir + "/" + filename;
    std::filesystem::path p(out_path);
    if (p.has_parent_path()) {
      std::error_code ec;
      std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream client_fs(out_path);
    client_fs << content << "\n";
  }

  std::cout << "Updated mocks and clients in " << code_dir << "\n";
}

} // namespace cdd_cpp::cli

// GCOV_EXCL_BR_STOP
