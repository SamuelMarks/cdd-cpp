#include <array>
#include <cassert>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace cdd_cpp::cli {
std::expected<std::string, std::string> exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pclose_wrapper = [](FILE *f) { pclose(f); };
  std::unique_ptr<FILE, decltype(pclose_wrapper)> pipe(
      popen(std::string(std::string(cmd) + " 2>&1").c_str(), "r"),
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
  std::filesystem::create_directories("test_tmp_dir");

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
  FILE *f = fopen("test_tmp_dir/test_spec.json", "w");
  if (f) {
    fwrite(json_spec.c_str(), 1, json_spec.size(), f);
    fclose(f);
  }

  FILE *f2 = fopen("test_spec.json", "w");
  if (f2) {
    fwrite(json_spec.c_str(), 1, json_spec.size(), f2);
    fclose(f2);
  }

  std::cout << "Starting to_docs_json test\n";
  auto res_exp = exec("./cdd-cpp to_docs_json -i test_spec.json");
  std::cout << "Done to_docs_json\n";
  std::string res = res_exp ? *res_exp : "";

  assert(res.find("\"imports\"") != std::string::npos);
  assert(res.find("\"wrapper_start\"") != std::string::npos);
  assert(res.find("\"wrapper_end\"") != std::string::npos);
  assert(res.find("\"snippet\"") != std::string::npos);

  std::cout << "Starting to_docs_json no options\n";
  auto res_no_exp = exec(
      "./cdd-cpp to_docs_json --no-imports --no-wrapping -i test_spec.json");
  std::cout << "Done to_docs_json no options\n";
  std::string res_no = res_no_exp ? *res_no_exp : "";

  assert(res_no.find("\"imports\"") == std::string::npos);
  assert(res_no.find("\"wrapper_start\"") == std::string::npos);
  assert(res_no.find("\"wrapper_end\"") == std::string::npos);
  assert(res_no.find("\"snippet\"") != std::string::npos);

  std::cout << "Testing help\n";
  auto help_res = exec("./cdd-cpp --help");
  assert(help_res && help_res->find("Usage:") != std::string::npos);

  std::cout << "Testing version\n";
  auto version_res = exec("./cdd-cpp --version");
  assert(version_res &&
         version_res->find("cdd-cpp version") != std::string::npos);

  std::cout << "Testing non existent file\n";
  auto err_file = exec("./cdd-cpp to_docs_json -i non_existent_file.json");
  assert(err_file &&
         err_file->find("Could not open file") != std::string::npos);

  std::cout << "Testing unknown command\n";
  auto err_cmd = exec("./cdd-cpp unknown_command");
  assert(err_cmd && err_cmd->find("Unknown command") != std::string::npos);

  std::cout << "Testing to_openapi no arg\n";
  auto to_openapi_err = exec("./cdd-cpp to_openapi");
  assert(to_openapi_err &&
         to_openapi_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing to_openapi help\n";
  auto to_openapi_help = exec("./cdd-cpp to_openapi --help");
  assert(to_openapi_help &&
         to_openapi_help->find("Usage:") != std::string::npos);

  std::cout << "Testing to_openapi test_tmp_dir output file\n";
  auto to_openapi_file_res =
      exec("./cdd-cpp to_openapi -i test_tmp_dir -o test_tmp_dir/out.json");
  assert(to_openapi_file_res);
  assert(std::filesystem::exists("test_tmp_dir/out.json"));

  std::cout << "Testing to_openapi test_tmp_dir\n";
  auto to_openapi_res = exec("./cdd-cpp to_openapi -i test_tmp_dir");
  assert(to_openapi_res &&
         to_openapi_res->find("openapi") != std::string::npos);

  std::cout << "Testing from_openapi\n";
  auto from_err = exec("./cdd-cpp from_openapi");
  assert(from_err && from_err->find("Missing subcommand") != std::string::npos);

  std::cout << "Testing from_openapi help\n";
  auto from_help = exec("./cdd-cpp from_openapi --help");
  assert(from_help && from_help->find("Usage:") != std::string::npos);

  std::cout << "Testing sync help\n";
  auto sync_help = exec("./cdd-cpp sync --help");
  assert(sync_help && sync_help->find("Usage:") != std::string::npos);

  std::cout << "Testing from_openapi to_sdk err\n";
  auto from_sdk_err = exec("./cdd-cpp from_openapi to_sdk");
  assert(from_sdk_err && from_sdk_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing from_openapi to_sdk_cli err\n";
  auto from_sdk_cli_err = exec("./cdd-cpp from_openapi to_sdk_cli");
  assert(from_sdk_cli_err &&
         from_sdk_cli_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing from_openapi to_server err\n";
  auto from_server_err = exec("./cdd-cpp from_openapi to_server");
  assert(from_server_err &&
         from_server_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing from_openapi to_sdk ok with flags\n";
  auto from_sdk =
      exec("./cdd-cpp from_openapi to_sdk -i test_spec.json -o out_dir "
           "--no-github-actions --no-installable-package --tests");
  assert(from_sdk);

  std::cout << "Testing from_openapi to_sdk_cli ok\n";
  auto from_sdk_cli = exec(
      "./cdd-cpp from_openapi to_sdk_cli -i test_spec.json -o out_dir --tests");
  assert(from_sdk_cli);

  std::cout << "Testing from_openapi to_server ok\n";
  auto from_server = exec(
      "./cdd-cpp from_openapi to_server -i test_spec.json -o out_dir --tests");
  assert(from_server);

  std::cout << "Testing from_openapi to_server parsing error\n";
  FILE *f3 = fopen("test_tmp_dir/bad_spec.json", "w");
  if (f3) {
    std::string bad_json = "{bad";
    fwrite(bad_json.c_str(), 1, bad_json.size(), f3);
    fclose(f3);
  }
  auto from_server_bad = exec("./cdd-cpp from_openapi to_server -i "
                              "test_tmp_dir/bad_spec.json -o out_dir");
  assert(from_server_bad &&
         from_server_bad->find("Error:") != std::string::npos);

  std::cout << "Testing from_openapi unknown subcommand\n";
  auto from_unknown =
      exec("./cdd-cpp from_openapi unknown -i test_spec.json -o out_dir");
  assert(from_unknown &&
         from_unknown->find("Unknown subcommand") != std::string::npos);

  std::cout << "Testing from_openapi to_sdk dir\n";
  auto from_sdk_dir =
      exec("./cdd-cpp from_openapi to_sdk --input-dir test_tmp_dir -o out_dir");
  assert(from_sdk_dir);

  std::cout << "Testing from_openapi to_sdk_cli dir\n";
  auto from_sdk_cli_dir = exec(
      "./cdd-cpp from_openapi to_sdk_cli --input-dir test_tmp_dir -o out_dir");
  assert(from_sdk_cli_dir);

  std::cout << "Testing from_openapi to_server dir\n";
  auto from_server_dir = exec(
      "./cdd-cpp from_openapi to_server --input-dir test_tmp_dir -o out_dir");
  assert(from_server_dir);

  std::cout << "Testing sync err\n";
  auto sync_err = exec("./cdd-cpp sync");
  assert(sync_err && sync_err->find("Usage:") != std::string::npos);

  std::cout << "Testing sync ok\n";
  auto sync_res = exec("./cdd-cpp sync -d test_tmp_dir -s test_spec.json");
  assert(sync_res);
  std::filesystem::remove_all("test_tmp_dir");

  std::cout << "Testing serve_json_rpc help\n";
  auto serve_err = exec("./cdd-cpp serve_json_rpc --help");
  assert(serve_err && serve_err->find("Usage:") != std::string::npos);

  std::cout << "cli::test_to_docs_json passed.\n";
}
} // namespace cdd_cpp::cli
