
#include "../client_sdk/emit.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../utils/cpp_parser.hpp"
#include "api.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::cli {

int sync(const SyncConfig &config, std::ostream &out, std::ostream &err) {
  (void)err;
  out << "Syncing code in " << config.input << " with spec " << config.output
      << "...\n";
  // Parse C++ to get updated routes/mocks
  auto cpp_spec = utils::parse_cpp_project(config.input);

  // Parse existing OpenAPI spec to merge
  std::ifstream fs(config.output);
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
  std::ofstream out_fs(config.output);
  out_fs << openapi::emit(merged_spec) << "\n";
  out << "Updated OpenAPI spec at " << config.output << "\n";

  // Regenerate Mocks
  std::string mock_code = mocks::emit(merged_spec);
  std::ofstream mock_fs(config.input + "/mocks_generated.hpp");
  mock_fs << mock_code << "\n";

  // Regenerate Client
  auto client_files = client_sdk::emit_client(merged_spec);
  for (const auto &[filename, content] : client_files) {
    std::string out_path = config.input + "/" + filename;
    std::filesystem::path p(out_path);
    if (p.has_parent_path()) {
      std::error_code ec;
      std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream client_fs(out_path);
    client_fs << content << "\n";
  }

  out << "Updated mocks and clients in " << config.input << "\n";
  return 0;
}

} // namespace cdd_cpp::cli
