/// @file sync_test.cpp
/// @brief Unit tests for bidirectional C++ code and OpenAPI specification
/// synchronization.

#include "../cli/api.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace cdd_cpp::cli {

void test_sync() {
  std::error_code ec;
  std::filesystem::create_directories("test_sync_dir", ec);
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
      "paths": {},
      "components": {
          "schemas": {
              "ExistingSpecModel": {
                  "type": "object",
                  "properties": {"id": {"type": "integer"}}
              }
          }
      }
  })";
  s.close();

  // 1. Test sync with default truth ("code")
  SyncConfig config;
  config.input = "test_sync_dir";
  config.output = "test_sync_spec.json";
  std::ostringstream out_ss, err_ss;
  [[maybe_unused]] int rc = sync(config, out_ss, err_ss);
  assert(rc == 0);

  std::ifstream is("test_sync_spec.json");
  std::string content((std::istreambuf_iterator<char>(is)),
                      std::istreambuf_iterator<char>());
  is.close();
  assert(content.find("SyncClass") != std::string::npos);
  assert(content.find("Sync API") != std::string::npos);

  // 2. Test sync with truth == "spec"
  std::ofstream s2("test_sync_spec.json");
  s2 << R"({
      "openapi": "3.2.0",
      "info": {"title": "Spec Truth API", "version": "2.0.0"},
      "paths": {}
  })";
  s2.close();
  config.truth = "spec";
  rc = sync(config, out_ss, err_ss);
  assert(rc == 0);

  std::ifstream is2("test_sync_spec.json");
  std::string content2((std::istreambuf_iterator<char>(is2)),
                       std::istreambuf_iterator<char>());
  is2.close();
  assert(content2.find("Spec Truth API") != std::string::npos);
  assert(content2.find("SyncClass") != std::string::npos);

  // 3. Test sync with truth == "spec" when spec already has components.schemas
  rc = sync(config, out_ss, err_ss);
  assert(rc == 0);

  // 4. Test sync with corrupted existing spec file (triggers warning)
  std::ofstream s_bad("test_sync_spec.json");
  s_bad << "{ invalid json ";
  s_bad.close();
  rc = sync(config, out_ss, err_ss);
  assert(rc == 0);
  assert(err_ss.str().find("Warning:") != std::string::npos);

  // 5. Test sync when output path cannot be opened for writing (is a directory)
  SyncConfig bad_out_cfg;
  bad_out_cfg.input = "test_sync_dir";
  bad_out_cfg.output = "test_sync_dir";
  [[maybe_unused]] int rc_bad = sync(bad_out_cfg, out_ss, err_ss);
  assert(rc_bad == 1);

  // 6. Test sync when output spec file does not exist yet
  std::filesystem::remove("test_sync_spec.json", ec);
  [[maybe_unused]] int rc_new = sync(config, out_ss, err_ss);
  assert(rc_new == 0);
  assert(std::filesystem::exists("test_sync_spec.json", ec));

  // 7. Test sync when client file output path cannot be opened for writing
  // (target file path is an existing directory, which deterministically fails
  // with EISDIR even under root/CAP_DAC_OVERRIDE in CI containers)
  std::filesystem::remove_all("test_sync_dir/src", ec);
  std::filesystem::create_directories("test_sync_dir/src/client.hpp", ec);
  [[maybe_unused]] int rc_ro = sync(config, out_ss, err_ss);
  assert(rc_ro == 1);

  std::filesystem::remove_all("test_sync_dir", ec);
  std::filesystem::remove("test_sync_spec.json", ec);
  std::cout << "test_sync passed.\n";
}

} // namespace cdd_cpp::cli
