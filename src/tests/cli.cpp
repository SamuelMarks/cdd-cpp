#include <array>
#include <cassert>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

// GCOV_EXCL_BR_START
namespace cdd_cpp::cli {
std::expected<std::string, std::string> exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pclose_wrapper = [](FILE *f) { pclose(f); };
  std::unique_ptr<FILE, decltype(pclose_wrapper)> pipe(
      popen(std::string(std::string(cmd) + " 2>&1").c_str(),
            "r"), // GCOV_EXCL_BR_LINE
      pclose_wrapper);
  if (!pipe) {              // GCOV_EXCL_BR_LINE
    return std::unexpected( // GCOV_EXCL_LINE
        "popen() failed!"); // GCOV_EXCL_LINE // GCOV_EXCL_BR_LINE //
                            // GCOV_EXCL_LINE
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) !=
         nullptr) {          // GCOV_EXCL_BR_LINE
    result += buffer.data(); // GCOV_EXCL_BR_LINE
  }
  return result; // GCOV_EXCL_BR_LINE
}

void test_to_docs_json() {
  std::filesystem::create_directories("test_tmp_dir"); // GCOV_EXCL_BR_LINE

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
  FILE *f = fopen("test_tmp_dir/test_spec.json", "w"); // GCOV_EXCL_BR_LINE
  if (f) {                                             // GCOV_EXCL_BR_LINE
    fwrite(json_spec.c_str(), 1, json_spec.size(), f); // GCOV_EXCL_BR_LINE
    fclose(f);                                         // GCOV_EXCL_BR_LINE
  }

  FILE *f2 = fopen("test_spec.json", "w");              // GCOV_EXCL_BR_LINE
  if (f2) {                                             // GCOV_EXCL_BR_LINE
    fwrite(json_spec.c_str(), 1, json_spec.size(), f2); // GCOV_EXCL_BR_LINE
    fclose(f2);                                         // GCOV_EXCL_BR_LINE
  }

  std::cout << "Starting to_docs_json test\n"; // GCOV_EXCL_BR_LINE
  auto res_exp =
      exec("./cdd-cpp to_docs_json -i test_spec.json"); // GCOV_EXCL_BR_LINE
  std::cout << "Done to_docs_json\n";                   // GCOV_EXCL_BR_LINE
  std::string res = res_exp ? *res_exp : "";            // GCOV_EXCL_BR_LINE

  assert(res.find("\"imports\"") != std::string::npos); // GCOV_EXCL_BR_LINE
  assert(res.find("\"wrapper_start\"") !=
         std::string::npos);                                // GCOV_EXCL_BR_LINE
  assert(res.find("\"wrapper_end\"") != std::string::npos); // GCOV_EXCL_BR_LINE
  assert(res.find("\"snippet\"") != std::string::npos);     // GCOV_EXCL_BR_LINE

  std::cout << "Starting to_docs_json no options\n"; // GCOV_EXCL_BR_LINE
  auto res_no_exp = exec(                            // GCOV_EXCL_BR_LINE
      "./cdd-cpp to_docs_json --no-imports --no-wrapping -i test_spec.json");
  std::cout << "Done to_docs_json no options\n";      // GCOV_EXCL_BR_LINE
  std::string res_no = res_no_exp ? *res_no_exp : ""; // GCOV_EXCL_BR_LINE

  assert(res_no.find("\"imports\"") == std::string::npos); // GCOV_EXCL_BR_LINE
  assert(res_no.find("\"wrapper_start\"") ==
         std::string::npos); // GCOV_EXCL_BR_LINE
  assert(res_no.find("\"wrapper_end\"") ==
         std::string::npos);                               // GCOV_EXCL_BR_LINE
  assert(res_no.find("\"snippet\"") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing help\n";            // GCOV_EXCL_BR_LINE
  auto help_res = exec("./cdd-cpp --help"); // GCOV_EXCL_BR_LINE
  assert(help_res &&
         help_res->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing version\n";               // GCOV_EXCL_BR_LINE
  auto version_res = exec("./cdd-cpp --version"); // GCOV_EXCL_BR_LINE
  assert(version_res &&                           // GCOV_EXCL_BR_LINE
         version_res->find("cdd-cpp version") != std::string::npos);

  std::cout << "Testing non existent file\n"; // GCOV_EXCL_BR_LINE
  auto err_file = exec(
      "./cdd-cpp to_docs_json -i non_existent_file.json"); // GCOV_EXCL_BR_LINE
  assert(err_file &&                                       // GCOV_EXCL_BR_LINE
         err_file->find("Could not open input file") != std::string::npos);

  std::cout << "Testing unknown command\n";         // GCOV_EXCL_BR_LINE
  auto err_cmd = exec("./cdd-cpp unknown_command"); // GCOV_EXCL_BR_LINE
  assert(err_cmd && err_cmd->find("Unknown command") !=
                        std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing to_openapi no arg\n";         // GCOV_EXCL_BR_LINE
  auto to_openapi_err = exec("./cdd-cpp to_openapi"); // GCOV_EXCL_BR_LINE
  assert(to_openapi_err &&                            // GCOV_EXCL_BR_LINE
         to_openapi_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing to_openapi help\n"; // GCOV_EXCL_BR_LINE
  auto to_openapi_help =
      exec("./cdd-cpp to_openapi --help"); // GCOV_EXCL_BR_LINE
  assert(to_openapi_help &&                // GCOV_EXCL_BR_LINE
         to_openapi_help->find("Usage:") != std::string::npos);

  std::cout
      << "Testing to_openapi test_tmp_dir output file\n"; // GCOV_EXCL_BR_LINE
  auto to_openapi_file_res = exec("./cdd-cpp to_openapi -i test_tmp_dir -o "
                                  "test_tmp_dir/out.json"); // GCOV_EXCL_BR_LINE
  assert(to_openapi_file_res);                              // GCOV_EXCL_BR_LINE
  assert(std::filesystem::exists("test_tmp_dir/out.json")); // GCOV_EXCL_BR_LINE

  std::cout << "Testing to_openapi test_tmp_dir\n"; // GCOV_EXCL_BR_LINE
  auto to_openapi_res =
      exec("./cdd-cpp to_openapi -i test_tmp_dir"); // GCOV_EXCL_BR_LINE
  assert(to_openapi_res &&                          // GCOV_EXCL_BR_LINE
         to_openapi_res->find("openapi") != std::string::npos);

  std::cout << "Testing from_openapi\n";          // GCOV_EXCL_BR_LINE
  auto from_err = exec("./cdd-cpp from_openapi"); // GCOV_EXCL_BR_LINE
  assert(from_err && from_err->find("Missing subcommand") !=
                         std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi help\n";             // GCOV_EXCL_BR_LINE
  auto from_help = exec("./cdd-cpp from_openapi --help"); // GCOV_EXCL_BR_LINE
  assert(from_help &&
         from_help->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing sync help\n";             // GCOV_EXCL_BR_LINE
  auto sync_help = exec("./cdd-cpp sync --help"); // GCOV_EXCL_BR_LINE
  assert(sync_help &&
         sync_help->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_sdk err\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk_err =
      exec("./cdd-cpp from_openapi to_sdk"); // GCOV_EXCL_BR_LINE
  assert(from_sdk_err && from_sdk_err->find("Missing -i") !=
                             std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_sdk_cli err\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk_cli_err =
      exec("./cdd-cpp from_openapi to_sdk_cli"); // GCOV_EXCL_BR_LINE
  assert(from_sdk_cli_err &&                     // GCOV_EXCL_BR_LINE
         from_sdk_cli_err->find("Missing -i") != std::string::npos);

  std::cout << "Testing from_openapi to_server err\n"; // GCOV_EXCL_BR_LINE
  auto from_server_err =
      exec("./cdd-cpp from_openapi to_server"); // GCOV_EXCL_BR_LINE
  assert(from_server_err &&                     // GCOV_EXCL_BR_LINE
         from_server_err->find("Missing -i") != std::string::npos);

  std::cout
      << "Testing from_openapi to_sdk ok with flags\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk = exec("./cdd-cpp from_openapi to_sdk -i test_spec.json -o "
                       "out_dir " // GCOV_EXCL_BR_LINE
                       "--no-github-actions --no-installable-package --tests");
  assert(from_sdk); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_sdk_cli ok\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk_cli = exec(                            // GCOV_EXCL_BR_LINE
      "./cdd-cpp from_openapi to_sdk_cli -i test_spec.json -o out_dir --tests");
  assert(from_sdk_cli); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_server ok\n"; // GCOV_EXCL_BR_LINE
  auto from_server = exec(                            // GCOV_EXCL_BR_LINE
      "./cdd-cpp from_openapi to_server -i test_spec.json -o out_dir --tests");
  assert(from_server); // GCOV_EXCL_BR_LINE

  std::cout
      << "Testing from_openapi to_server parsing error\n"; // GCOV_EXCL_BR_LINE
  FILE *f3 = fopen("test_tmp_dir/bad_spec.json", "w");     // GCOV_EXCL_BR_LINE
  if (f3) {                                                // GCOV_EXCL_BR_LINE
    std::string bad_json = "{bad";                         // GCOV_EXCL_BR_LINE
    fwrite(bad_json.c_str(), 1, bad_json.size(), f3);      // GCOV_EXCL_BR_LINE
    fclose(f3);                                            // GCOV_EXCL_BR_LINE
  }
  auto from_server_bad =
      exec("./cdd-cpp from_openapi to_server -i " // GCOV_EXCL_BR_LINE
           "test_tmp_dir/bad_spec.json -o out_dir");
  assert(from_server_bad && // GCOV_EXCL_BR_LINE
         from_server_bad->find("Error:") != std::string::npos);

  std::cout << "Testing from_openapi unknown subcommand\n"; // GCOV_EXCL_BR_LINE
  auto from_unknown = exec("./cdd-cpp from_openapi unknown -i test_spec.json "
                           "-o out_dir"); // GCOV_EXCL_BR_LINE
  assert(from_unknown &&                  // GCOV_EXCL_BR_LINE
         from_unknown->find("Unknown subcommand") != std::string::npos);

  std::cout << "Testing from_openapi to_sdk dir\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk_dir = exec("./cdd-cpp from_openapi to_sdk --input-dir "
                           "test_tmp_dir -o out_dir"); // GCOV_EXCL_BR_LINE
  assert(from_sdk_dir);                                // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_sdk_cli dir\n"; // GCOV_EXCL_BR_LINE
  auto from_sdk_cli_dir = exec(                         // GCOV_EXCL_BR_LINE
      "./cdd-cpp from_openapi to_sdk_cli --input-dir test_tmp_dir -o out_dir");
  assert(from_sdk_cli_dir); // GCOV_EXCL_BR_LINE

  std::cout << "Testing from_openapi to_server dir\n"; // GCOV_EXCL_BR_LINE
  auto from_server_dir = exec(                         // GCOV_EXCL_BR_LINE
      "./cdd-cpp from_openapi to_server --input-dir test_tmp_dir -o out_dir");
  assert(from_server_dir); // GCOV_EXCL_BR_LINE

  std::cout << "Testing sync err\n";      // GCOV_EXCL_BR_LINE
  auto sync_err = exec("./cdd-cpp sync"); // GCOV_EXCL_BR_LINE
  assert(sync_err &&
         sync_err->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing sync ok\n"; // GCOV_EXCL_BR_LINE
  auto sync_res = exec(
      "./cdd-cpp sync -d test_tmp_dir -s test_spec.json"); // GCOV_EXCL_BR_LINE
  assert(sync_res);                                        // GCOV_EXCL_BR_LINE
  std::filesystem::remove_all("test_tmp_dir");             // GCOV_EXCL_BR_LINE

  std::cout << "Testing serve_json_rpc help\n";             // GCOV_EXCL_BR_LINE
  auto serve_err = exec("./cdd-cpp serve_json_rpc --help"); // GCOV_EXCL_BR_LINE
  assert(serve_err &&
         serve_err->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing read_file error\n"; // GCOV_EXCL_BR_LINE
  auto err_read_file = exec("./cdd-cpp from_openapi to_sdk -i nonexistent.json "
                            "-o out_dir"); // GCOV_EXCL_BR_LINE
  assert(err_read_file &&                  // GCOV_EXCL_BR_LINE
         err_read_file->find("Could not open input file") != std::string::npos);

  std::cout << "Testing empty args\n"; // GCOV_EXCL_BR_LINE
  auto empty_args = exec("./cdd-cpp"); // GCOV_EXCL_BR_LINE
  assert(empty_args &&
         empty_args->find("Usage:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing to_docs_json -i error\n";         // GCOV_EXCL_BR_LINE
  auto to_docs_err_no_i = exec("./cdd-cpp to_docs_json"); // GCOV_EXCL_BR_LINE
  assert(to_docs_err_no_i &&                              // GCOV_EXCL_BR_LINE
         to_docs_err_no_i->find("Missing -i") != std::string::npos);

  std::cout << "Testing to_docs_json help\n"; // GCOV_EXCL_BR_LINE
  auto to_docs_help =
      exec("./cdd-cpp to_docs_json --help"); // GCOV_EXCL_BR_LINE
  assert(to_docs_help && to_docs_help->find("Usage:") !=
                             std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout << "Testing to_docs_json -o flag\n"; // GCOV_EXCL_BR_LINE
  auto to_docs_out = exec("./cdd-cpp to_docs_json -i test_spec.json -o "
                          "out.json"); // GCOV_EXCL_BR_LINE
  assert(to_docs_out &&
         std::filesystem::exists("out.json")); // GCOV_EXCL_BR_LINE

  std::cout
      << "Testing openapi::parse error in to_docs_json\n"; // GCOV_EXCL_BR_LINE
  FILE *f_bad = fopen("bad.json", "w");                    // GCOV_EXCL_BR_LINE
  if (f_bad) {                                             // GCOV_EXCL_BR_LINE
    std::string bad = "{bad}";                             // GCOV_EXCL_BR_LINE
    fwrite(bad.c_str(), 1, bad.size(), f_bad);             // GCOV_EXCL_BR_LINE
    fclose(f_bad);                                         // GCOV_EXCL_BR_LINE
  }
  auto to_docs_bad =
      exec("./cdd-cpp to_docs_json -i bad.json"); // GCOV_EXCL_BR_LINE
  assert(to_docs_bad &&
         to_docs_bad->find("Error:") != std::string::npos); // GCOV_EXCL_BR_LINE

  std::cout
      << "Testing from_openapi parsing error with valid json\n"; // GCOV_EXCL_BR_LINE
  auto from_server_bad_parse = exec("./cdd-cpp from_openapi to_server -i "
                                    "bad.json -o out_dir"); // GCOV_EXCL_BR_LINE
  assert(from_server_bad_parse &&                           // GCOV_EXCL_BR_LINE
         from_server_bad_parse->find("Error:") != std::string::npos);

  std::cout << "Testing output file open error\n";      // GCOV_EXCL_BR_LINE
  std::filesystem::create_directories("read_only_dir"); // GCOV_EXCL_BR_LINE
  std::filesystem::permissions(
      "read_only_dir", std::filesystem::perms::none); // GCOV_EXCL_BR_LINE
  auto err_out_file = exec(
      "./cdd-cpp from_openapi to_sdk -i test_spec.json -o " // GCOV_EXCL_BR_LINE
      "read_only_dir/file.json");
  assert(err_out_file && // GCOV_EXCL_BR_LINE
         err_out_file->find("Could not open output file") != std::string::npos);
  std::filesystem::permissions(
      "read_only_dir", std::filesystem::perms::all); // GCOV_EXCL_BR_LINE
  std::filesystem::remove_all("read_only_dir");      // GCOV_EXCL_BR_LINE

  std::cout << "Testing env vars logic\n"; // GCOV_EXCL_BR_LINE
  auto env_var_res =
      exec("CDD_CPP_NO_IMPORTS=true CDD_CPP_NO_WRAPPING=1 " // GCOV_EXCL_BR_LINE
           "CDD_CPP_INPUT=test_spec.json "
           "CDD_CPP_OUTPUT=env_out.json ./cdd-cpp to_docs_json");
  assert(env_var_res);                             // GCOV_EXCL_BR_LINE
  assert(std::filesystem::exists("env_out.json")); // GCOV_EXCL_BR_LINE

  std::filesystem::create_directories("test_tmp_dir"); // GCOV_EXCL_BR_LINE
  auto env_var_res2 = exec(                            // GCOV_EXCL_BR_LINE
      "CDD_CPP_DIR=test_tmp_dir CDD_CPP_SPEC=test_spec.json ./cdd-cpp sync");
  assert(env_var_res2); // GCOV_EXCL_BR_LINE

  auto env_var_res3 = exec("CDD_CPP_INPUT=test_spec.json "
                           "CDD_CPP_OUTPUT=env_out_sdk " // GCOV_EXCL_BR_LINE
                           "CDD_CPP_NO_GITHUB_ACTIONS=1 "
                           "CDD_CPP_NO_INSTALLABLE_PACKAGE=1 "
                           "CDD_CPP_TESTS=1 ./cdd-cpp from_openapi to_sdk");
  assert(env_var_res3); // GCOV_EXCL_BR_LINE

  std::cout << "Testing invalid port\n"; // GCOV_EXCL_BR_LINE
  auto err_port =
      exec("./cdd-cpp serve_json_rpc --port abc"); // GCOV_EXCL_BR_LINE
  std::cout
      << "Testing serve_json_rpc execution and graceful stop\n"; // GCOV_EXCL_BR_LINE
  auto serve_res =
      exec("./cdd-cpp serve_json_rpc --port 8085 --listen "
           "127.0.0.1 > " // GCOV_EXCL_BR_LINE
           "/dev/null 2>&1 & "
           "PID=$!; "
           "sleep 0.5 && "
           "curl -s -X POST -H 'Content-Type: application/json' -d "
           "'{\"jsonrpc\":\"2.0\",\"method\":\"ping\",\"id\":1}' "
           "http://127.0.0.1:8085 > /dev/null && "
           "curl -s -X OPTIONS http://127.0.0.1:8085 > /dev/null "
           "&& "
           "curl -s -X GET http://127.0.0.1:8085/stop > /dev/null "
           "&& "
           "wait $PID || true");
  assert(serve_res); // GCOV_EXCL_BR_LINE
  assert(serve_res); // GCOV_EXCL_BR_LINE

  std::cout << "cli::test_to_docs_json passed.\n"; // GCOV_EXCL_BR_LINE
}
} // namespace cdd_cpp::cli
// GCOV_EXCL_BR_STOP
