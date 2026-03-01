#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace cdd_cpp::cli {
std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pclose_wrapper = [](FILE *f) { pclose(f); };
  std::unique_ptr<FILE, decltype(pclose_wrapper)> pipe(popen(cmd, "r"),
                                                       pclose_wrapper);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
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
  FILE* f = fopen("test_spec.json", "w");
  if (f) {
      fwrite(json_spec.c_str(), 1, json_spec.size(), f);
      fclose(f);
  }

  std::string res = exec("./cdd-cpp to_docs_json -i test_spec.json");
  if (res.empty() || res.find("not found") != std::string::npos) {
      res = exec("../build/cdd-cpp to_docs_json -i test_spec.json"); // fallback
  }

  assert(res.find("\"imports\"") != std::string::npos);
  assert(res.find("\"wrapper_start\"") != std::string::npos);
  assert(res.find("\"wrapper_end\"") != std::string::npos);
  assert(res.find("\"snippet\"") != std::string::npos);

  std::string res_no = exec("./cdd-cpp to_docs_json --no-imports --no-wrapping -i test_spec.json");
  if (res_no.empty() || res_no.find("not found") != std::string::npos) {
      res_no = exec("../build/cdd-cpp to_docs_json --no-imports --no-wrapping -i test_spec.json");
  }

  assert(res_no.find("\"imports\"") == std::string::npos);
  assert(res_no.find("\"wrapper_start\"") == std::string::npos);
  assert(res_no.find("\"wrapper_end\"") == std::string::npos);
  assert(res_no.find("\"snippet\"") != std::string::npos);

  std::cout << "cli::test_to_docs_json passed.\n";
}
} // namespace cdd_cpp::cli
