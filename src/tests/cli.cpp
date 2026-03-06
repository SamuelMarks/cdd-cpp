#include <array>
#include <cassert>
#include <cstdlib>
#include <expected>
#include <iostream>
#include <memory>
#include <string>

namespace cdd_cpp::cli {
std::expected<std::string, std::string> exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pclose_wrapper = [](FILE *f) { pclose(f); };
  std::unique_ptr<FILE, decltype(pclose_wrapper)> pipe(popen(cmd, "r"),
                                                       pclose_wrapper);
  if (!pipe) {
    return std::unexpected("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

void test_to_docs_json() {
  std::string json_spec = R"({
    "openapi": "3.2.0",
    "paths": {
      "/test": {
        "get": {
          "operationId": "getTest"
        }
      }
    }
  })";
  FILE *f = fopen("test_spec.json", "w");
  if (f) {
    fwrite(json_spec.c_str(), 1, json_spec.size(), f);
    fclose(f);
  }

  auto res_exp = exec("./cdd-cpp to_docs_json -i test_spec.json");
  std::string res = res_exp ? *res_exp : "";
  if (res.empty() || res.find("not found") != std::string::npos) {
    auto fb =
        exec("./build/cdd-cpp to_docs_json -i test_spec.json"); // fallback
    if (fb)
      res = *fb;
  }

  assert(res.find("\"imports\"") != std::string::npos);
  assert(res.find("\"wrapper_start\"") != std::string::npos);
  assert(res.find("\"wrapper_end\"") != std::string::npos);
  assert(res.find("\"snippet\"") != std::string::npos);

  auto res_no_exp = exec(
      "./cdd-cpp to_docs_json --no-imports --no-wrapping -i test_spec.json");
  std::string res_no = res_no_exp ? *res_no_exp : "";
  if (res_no.empty() || res_no.find("not found") != std::string::npos) {
    auto fb = exec("./build/cdd-cpp to_docs_json --no-imports --no-wrapping "
                   "-i test_spec.json");
    if (fb)
      res_no = *fb;
  }

  assert(res_no.find("\"imports\"") == std::string::npos);
  assert(res_no.find("\"wrapper_start\"") == std::string::npos);
  assert(res_no.find("\"wrapper_end\"") == std::string::npos);
  assert(res_no.find("\"snippet\"") != std::string::npos);

  std::cout << "cli::test_to_docs_json passed.\n";
}
} // namespace cdd_cpp::cli
