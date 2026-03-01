#include "../cli/sync.cpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::cli {
void test_sync() {
  std::filesystem::create_directories("test_sync_dir");
  std::ofstream f("test_sync_dir/test.hpp");
  f << R"(
  #pragma once
  #include <string>
  
  namespace test_ns {
      class SyncClass {
      public:
          int syncField = 0;
      };
  }
  )";
  f.close();

  std::ofstream s("test_sync_spec.json");
  s << R"({
      "openapi": "3.2.0",
      "info": {"title": "Sync API", "version": "1.0.0"},
      "paths": {}
  })";
  s.close();

  // Test sync
  sync_command("test_sync_dir", "test_sync_spec.json");

  // It should overwrite spec
  std::ifstream is("test_sync_spec.json");
  std::string content((std::istreambuf_iterator<char>(is)),
                      std::istreambuf_iterator<char>());
  assert(content.find("SyncClass") != std::string::npos);

  std::filesystem::remove_all("test_sync_dir");
  std::filesystem::remove("test_sync_spec.json");
  std::cout << "test_sync passed.\n";
}
} // namespace cdd_cpp::cli
