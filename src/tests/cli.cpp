#include "../cli/api.hpp"
#include "../utils/httplib.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace cdd_cpp::cli {

namespace {

struct TempDirFixture {
  std::filesystem::path dir;
  explicit TempDirFixture(const std::string &name)
      : dir(std::filesystem::temp_directory_path() / name) {
    std::error_code ec;
    std::filesystem::remove_all(dir, ec);
    std::filesystem::create_directories(dir, ec);
  }
  ~TempDirFixture() {
    std::error_code ec;
    std::filesystem::permissions(dir, std::filesystem::perms::all, ec);
    for (auto &entry : std::filesystem::recursive_directory_iterator(dir, ec)) {
      std::filesystem::permissions(entry.path(), std::filesystem::perms::all,
                                   ec);
    }
    std::filesystem::remove_all(dir, ec);
  }
  std::string path(const std::string &rel = "") const {
    if (rel.empty())
      return dir.string();
    return (dir / rel).string();
  }
};

struct CliResult {
  int code;
  std::string out;
  std::string err;
  std::string combined() const { return out + err; }
};

CliResult run_cli(const std::vector<std::string> &args,
                  const std::string &stdin_str = "") {
  std::vector<char *> argv;
  argv.reserve(args.size());
  for (const auto &a : args) {
    argv.push_back(const_cast<char *>(a.c_str()));
  }
  std::ostringstream out_ss, err_ss;
  std::istringstream in_ss(stdin_str);
  int code = main_impl(static_cast<int>(argv.size()), argv.data(), out_ss,
                       err_ss, in_ss);
  return {code, out_ss.str(), err_ss.str()};
}

void write_str(const std::string &filepath, const std::string &content) {
  std::filesystem::path p(filepath);
  if (p.has_parent_path()) {
    std::error_code ec;
    std::filesystem::create_directories(p.parent_path(), ec);
  }
  std::ofstream fs(filepath);
  fs << content;
}

} // namespace

std::expected<std::string, std::string> exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pclose_wrapper = [](FILE *f) { pclose(f); };

  FILE *p_res = popen(std::string(std::string(cmd) + " 2>&1").c_str(), "r");
  std::unique_ptr<FILE, decltype(pclose_wrapper)> pipe(p_res, pclose_wrapper);
  if (!pipe) {
    return std::unexpected("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

void test_to_docs_json() noexcept {
  TempDirFixture tmp("cdd_cli_test_docs");

  write_str(tmp.path("code/routes.hpp"), R"(
      void setup(httplib::Server& svr) {
        svr.Get("/r", [&](const httplib::Request& req, httplib::Response& res) {
            auto id = req.path_params.at("id");
            auto q = req.get_param_value("q");
            auto auth = req.get_header_value("Auth");
            auto u = body.get<User>();
        });
        svr.Post("/r", [&](const httplib::Request& req, httplib::Response& res) {});
        svr.Put("/r", [&](const httplib::Request& req, httplib::Response& res) {});
        svr.Delete("/r", [&](const httplib::Request& req, httplib::Response& res) {});
        svr.Patch("/r", [&](const httplib::Request& req, httplib::Response& res) {});
        svr.Options("/r", [&](const httplib::Request& req, httplib::Response& res) {});
        svr.Head("/r", [&](const httplib::Request& req, httplib::Response& res) {});
      }
  )");

  write_str(tmp.path("code/client.cpp"), R"(
    std::expected<std::string, std::string> Client::getTest(int id, double score, const std::string& body) noexcept {
        std::string full_url = base_url + "/c/{id}";
        CURLOPT_CUSTOMREQUEST, "GET"
    }
    std::expected<std::string, std::string> Client::postTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "POST"
    }
    std::expected<std::string, std::string> Client::putTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "PUT"
    }
    std::expected<std::string, std::string> Client::deleteTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "DELETE"
    }
    std::expected<std::string, std::string> Client::patchTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "PATCH"
    }
    std::expected<std::string, std::string> Client::optionsTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "OPTIONS"
    }
    std::expected<std::string, std::string> Client::headTest() noexcept {
        std::string full_url = base_url + "/c";
        CURLOPT_CUSTOMREQUEST, "HEAD"
    }
  )");

  write_str(tmp.path("code/cli.cpp"), R"(
    switch (current_node) {
        case 5: {
            auto res = handle_myOp(client, path_params["pid"], query_params["q"], body);
            if (!res) {
                std::cerr << "Request failed: " << res.error() << "\n";
                return 1;
            }
            std::cout << *res << "\n";
            break;
        }
        case 6: {
            auto res = handle_emptyOp(client);
            break;
        }
    }
  )");

  std::string json_spec = R"({
    "openapi": "3.2.0",
    "paths": {
      "/test": {
        "get": {
          "operationId": "getTest"
        },
        "post": {}
      }
    }
  })";
  std::string spec_file = tmp.path("test_spec.json");
  write_str(spec_file, json_spec);

  auto res_exp = run_cli({"cdd-cpp", "to_docs_json", "-i", spec_file});
  assert(res_exp.code == 0);
  assert(res_exp.out.find("\"imports\"") != std::string::npos);
  assert(res_exp.out.find("\"wrapper_start\"") != std::string::npos);
  assert(res_exp.out.find("\"wrapper_end\"") != std::string::npos);
  assert(res_exp.out.find("\"snippet\"") != std::string::npos);

  auto res_no_exp = run_cli({"cdd-cpp", "to_docs_json", "--no-imports",
                             "--no-wrapping", "-i", spec_file});
  assert(res_no_exp.code == 0);
  assert(res_no_exp.out.find("\"imports\"") == std::string::npos);
  assert(res_no_exp.out.find("\"wrapper_start\"") == std::string::npos);
  assert(res_no_exp.out.find("\"wrapper_end\"") == std::string::npos);
  assert(res_no_exp.out.find("\"snippet\"") != std::string::npos);

  auto help_res = run_cli({"cdd-cpp", "--help"});
  assert(help_res.code == 0 &&
         help_res.out.find("Usage:") != std::string::npos);
  assert(run_cli({"cdd-cpp", "-h"}).code == 0);

  auto version_res = run_cli({"cdd-cpp", "--version"});
  assert(version_res.code == 0 &&
         version_res.out.find("cdd-cpp version") != std::string::npos);
  assert(run_cli({"cdd-cpp", "-v"}).code == 0);
  assert(run_cli({"cdd-cpp", "mcp", "-h"}).code == 0);

  // Concise binary invocation integration test
  std::error_code fs_ec;
  const char *bin_cmd = std::filesystem::exists("./cdd-cpp", fs_ec)
                            ? "./cdd-cpp --version"
                            : "./build/cdd-cpp --version";
  auto bin_version = exec(bin_cmd);
  assert(bin_version &&
         bin_version->find("cdd-cpp version") != std::string::npos);

  auto err_file =
      run_cli({"cdd-cpp", "to_docs_json", "-i", tmp.path("non_existent.json")});
  assert(err_file.code != 0 &&
         err_file.err.find("Could not open input file") != std::string::npos);

  auto err_cmd = run_cli({"cdd-cpp", "unknown_command"});
  assert(err_cmd.code != 0 &&
         err_cmd.err.find("Unknown command") != std::string::npos);

  auto to_openapi_err = run_cli({"cdd-cpp", "to_openapi"});
  assert(to_openapi_err.code != 0 &&
         to_openapi_err.err.find("Missing -i") != std::string::npos);

  auto to_openapi_help = run_cli({"cdd-cpp", "to_openapi", "--help"});
  assert(to_openapi_help.code == 0 &&
         to_openapi_help.out.find("Usage:") != std::string::npos);

  std::string out_openapi_file = tmp.path("out.json");
  auto to_openapi_file_res =
      run_cli({"cdd-cpp", "to_openapi", "-i", tmp.path("code"), "-o",
               out_openapi_file});
  assert(to_openapi_file_res.code == 0);
  assert(std::filesystem::exists(out_openapi_file, fs_ec));

  auto to_openapi_bad_out = run_cli({"cdd-cpp", "to_openapi", "-i",
                                     tmp.path("code"), "-o", tmp.path("code")});
  assert(to_openapi_bad_out.code != 0);

  auto to_openapi_res =
      run_cli({"cdd-cpp", "to_openapi", "-i", tmp.path("code")});
  assert(to_openapi_res.code == 0 &&
         to_openapi_res.out.find("openapi") != std::string::npos);

  auto from_err = run_cli({"cdd-cpp", "from_openapi"});
  assert(from_err.code != 0 &&
         from_err.err.find("Missing subcommand") != std::string::npos);

  auto from_help = run_cli({"cdd-cpp", "from_openapi", "--help"});
  assert(from_help.code == 0 &&
         from_help.out.find("Usage:") != std::string::npos);

  auto from_sdk_err = run_cli({"cdd-cpp", "from_openapi", "to_sdk"});
  assert(from_sdk_err.code != 0 &&
         from_sdk_err.err.find("Missing -i") != std::string::npos);

  auto from_sdk_cli_err = run_cli({"cdd-cpp", "from_openapi", "to_sdk_cli"});
  assert(from_sdk_cli_err.code != 0 &&
         from_sdk_cli_err.err.find("Missing -i") != std::string::npos);

  auto from_server_err = run_cli({"cdd-cpp", "from_openapi", "to_server"});
  assert(from_server_err.code != 0 &&
         from_server_err.err.find("Missing -i") != std::string::npos);

  std::string out_dir = tmp.path("out_dir");
  auto from_sdk = run_cli({"cdd-cpp", "from_openapi", "to_sdk", "-i", spec_file,
                           "-o", out_dir, "--no-github-actions",
                           "--no-installable-package", "--tests", "--mcp"});
  assert(from_sdk.code == 0);

  auto from_sdk_cli = run_cli({"cdd-cpp", "from_openapi", "to_sdk_cli", "-i",
                               spec_file, "-o", out_dir, "--tests"});
  assert(from_sdk_cli.code == 0);

  auto from_server = run_cli({"cdd-cpp", "from_openapi", "to_server", "-i",
                              spec_file, "-o", out_dir, "--tests"});
  assert(from_server.code == 0);

  std::string bad_spec_file = tmp.path("bad_spec.json");
  write_str(bad_spec_file, "{bad");
  auto from_server_bad = run_cli({"cdd-cpp", "from_openapi", "to_server", "-i",
                                  bad_spec_file, "-o", out_dir});
  assert(from_server_bad.code != 0 &&
         from_server_bad.err.find("Error:") != std::string::npos);

  auto from_unknown = run_cli(
      {"cdd-cpp", "from_openapi", "unknown", "-i", spec_file, "-o", out_dir});
  assert(from_unknown.code != 0 &&
         from_unknown.err.find("Unknown subcommand") != std::string::npos);

  auto from_sdk_dir = run_cli({"cdd-cpp", "from_openapi", "to_sdk",
                               "--input-dir", tmp.path("code"), "-o", out_dir});
  assert(from_sdk_dir.code == 0);

  auto from_sdk_cli_dir = run_cli({"cdd-cpp", "from_openapi", "to_sdk_cli",
                                   "-d", tmp.path("code"), "-o", out_dir});
  assert(from_sdk_cli_dir.code == 0);

  auto from_server_dir =
      run_cli({"cdd-cpp", "from_openapi", "to_server", "--input-dir",
               tmp.path("code"), "-o", out_dir});
  assert(from_server_dir.code == 0);

  auto serve_err = run_cli({"cdd-cpp", "serve_json_rpc", "--help"});
  assert(serve_err.code == 0 &&
         serve_err.out.find("Usage:") != std::string::npos);

  std::string gd_spec = R"({
    "kind": "discovery#restDescription",
    "name": "test_api",
    "version": "v1",
    "schemas": {},
    "resources": {}
  })";
  std::string gd_spec_file = tmp.path("test_gd_spec.json");
  write_str(gd_spec_file, gd_spec);

  auto from_gd_help = run_cli({"cdd-cpp", "from_google_discovery", "--help"});
  assert(from_gd_help.code == 0 &&
         from_gd_help.out.find("Usage:") != std::string::npos);

  auto from_gd_err = run_cli({"cdd-cpp", "from_google_discovery"});
  assert(from_gd_err.code != 0 &&
         from_gd_err.err.find("Missing subcommand") != std::string::npos);

  auto from_gd_sdk_err =
      run_cli({"cdd-cpp", "from_google_discovery", "to_sdk"});
  assert(from_gd_sdk_err.code != 0 &&
         from_gd_sdk_err.err.find("Missing -i") != std::string::npos);

  auto from_gd_unknown = run_cli({"cdd-cpp", "from_google_discovery", "unknown",
                                  "-i", gd_spec_file, "-o", out_dir});
  assert(from_gd_unknown.code != 0 &&
         from_gd_unknown.err.find("Unknown subcommand") != std::string::npos);

  auto from_gd_sdk = run_cli(
      {"cdd-cpp", "from_google_discovery", "to_sdk", "-i", gd_spec_file, "-o",
       out_dir, "--no-github-actions", "--no-installable-package", "--tests"});
  assert(from_gd_sdk.code == 0);

  auto from_gd_sdk_cli =
      run_cli({"cdd-cpp", "from_google_discovery", "to_sdk_cli", "-i",
               gd_spec_file, "-o", out_dir, "--tests"});
  assert(from_gd_sdk_cli.code == 0);

  setenv("CDD_INPUT", gd_spec_file.c_str(), 1);
  setenv("CDD_OUTPUT", tmp.path("env_out_gd_sdk").c_str(), 1);
  setenv("CDD_NO_GITHUB_ACTIONS", "1", 1);
  setenv("CDD_NO_INSTALLABLE_PACKAGE", "1", 1);
  setenv("CDD_TESTS", "1", 1);
  auto from_gd_env = run_cli({"cdd-cpp", "from_google_discovery", "to_sdk"});
  assert(from_gd_env.code == 0);
  unsetenv("CDD_INPUT");
  unsetenv("CDD_OUTPUT");
  unsetenv("CDD_NO_GITHUB_ACTIONS");
  unsetenv("CDD_NO_INSTALLABLE_PACKAGE");
  unsetenv("CDD_TESTS");

  auto err_read_file = run_cli({"cdd-cpp", "from_openapi", "to_sdk", "-i",
                                tmp.path("nonexistent.json"), "-o", out_dir});
  assert(err_read_file.code != 0 &&
         err_read_file.err.find("Could not open input file") !=
             std::string::npos);

  auto empty_args = run_cli({"cdd-cpp"});
  assert(empty_args.code != 0 &&
         empty_args.out.find("Usage:") != std::string::npos);

  auto to_docs_err_no_i = run_cli({"cdd-cpp", "to_docs_json"});
  assert(to_docs_err_no_i.code != 0 &&
         to_docs_err_no_i.err.find("Missing -i") != std::string::npos);

  auto to_docs_help = run_cli({"cdd-cpp", "to_docs_json", "--help"});
  assert(to_docs_help.code == 0 &&
         to_docs_help.out.find("Usage:") != std::string::npos);

  std::string docs_out_file = tmp.path("docs_out.json");
  auto to_docs_out = run_cli(
      {"cdd-cpp", "to_docs_json", "-i", spec_file, "-o", docs_out_file});
  assert(to_docs_out.code == 0 &&
         std::filesystem::exists(docs_out_file, fs_ec));

  auto to_docs_bad_out = run_cli(
      {"cdd-cpp", "to_docs_json", "-i", spec_file, "-o", tmp.path("code")});
  assert(to_docs_bad_out.code != 0);

  std::string bad_file = tmp.path("bad.json");
  write_str(bad_file, "{bad}");
  auto to_docs_bad = run_cli({"cdd-cpp", "to_docs_json", "-i", bad_file});
  assert(to_docs_bad.code != 0 &&
         to_docs_bad.err.find("Error:") != std::string::npos);

  auto from_server_bad_parse = run_cli(
      {"cdd-cpp", "from_openapi", "to_server", "-i", bad_file, "-o", out_dir});
  assert(from_server_bad_parse.code != 0 &&
         from_server_bad_parse.err.find("Error:") != std::string::npos);

  std::string ro_dir = tmp.path("read_only_dir");
  std::filesystem::create_directories(ro_dir + "/src/client.hpp", fs_ec);
  auto err_out_file = run_cli(
      {"cdd-cpp", "from_openapi", "to_sdk", "-i", spec_file, "-o", ro_dir});
  assert(err_out_file.code != 0 &&
         err_out_file.err.find("Could not open output file") !=
             std::string::npos);

  std::string env_out_json = tmp.path("env_out.json");
  setenv("CDD_NO_IMPORTS", "true", 1);
  setenv("CDD_NO_WRAPPING", "1", 1);
  setenv("CDD_INPUT", spec_file.c_str(), 1);
  setenv("CDD_OUTPUT", env_out_json.c_str(), 1);
  auto env_var_res = run_cli({"cdd-cpp", "to_docs_json"});
  assert(env_var_res.code == 0 && std::filesystem::exists(env_out_json, fs_ec));
  unsetenv("CDD_NO_IMPORTS");
  unsetenv("CDD_NO_WRAPPING");
  unsetenv("CDD_INPUT");
  unsetenv("CDD_OUTPUT");

  setenv("CDD_INPUT", spec_file.c_str(), 1);
  setenv("CDD_OUTPUT", tmp.path("env_out_sdk").c_str(), 1);
  setenv("CDD_NO_GITHUB_ACTIONS", "1", 1);
  setenv("CDD_NO_INSTALLABLE_PACKAGE", "1", 1);
  setenv("CDD_TESTS", "1", 1);
  auto env_var_res3 = run_cli({"cdd-cpp", "from_openapi", "to_sdk"});
  assert(env_var_res3.code == 0);
  unsetenv("CDD_INPUT");
  unsetenv("CDD_OUTPUT");
  unsetenv("CDD_NO_GITHUB_ACTIONS");
  unsetenv("CDD_NO_INSTALLABLE_PACKAGE");
  unsetenv("CDD_TESTS");

  auto err_port = run_cli({"cdd-cpp", "serve_json_rpc", "--port", "abc"});
  assert(err_port.code != 0 &&
         err_port.err.find("Invalid port number") != std::string::npos);

  // In-process thread test for serve_json_rpc with sub-10ms polling
  int rpc_port = 18086;
  std::thread server_thread([rpc_port]() {
    run_cli({"cdd-cpp", "serve_json_rpc", "--port", std::to_string(rpc_port),
             "--listen", "127.0.0.1"});
  });

  httplib::Client cli("127.0.0.1", rpc_port);
  cli.set_connection_timeout(1, 0);
  bool ready = false;
  for (int attempt = 0; attempt < 100; ++attempt) {
    if (auto res = cli.Options("/")) {
      if (res->status == 200) {
        ready = true;
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  assert(ready);

  auto post_res =
      cli.Post("/", "{\"jsonrpc\":\"2.0\",\"method\":\"ping\",\"id\":1}",
               "application/json");
  assert(post_res && post_res->status == 200);

  auto stop_res = cli.Get("/stop");
  (void)stop_res;
  server_thread.join();

  std::cout << "cli::test_to_docs_json passed.\n";
}

void test_mcp_cli() {
  TempDirFixture tmp("cdd_cli_test_mcp");
  std::string spec_file = tmp.path("spec.json");
  write_str(
      spec_file,
      R"({"openapi":"3.2.0","info":{"title":"T","version":"1.0"},"paths":{"/p":{"get":{"operationId":"op1"}}}})");

  auto res_help = run_cli({"cdd-cpp", "mcp", "--help"});
  assert(res_help.code == 0 &&
         res_help.out.find("Usage:") != std::string::npos);

  auto res1 = run_cli({"cdd-cpp", "mcp"}, "   \n");
  assert(res1.out.find("Parse error") != std::string::npos);

  auto res1_2 = run_cli({"cdd-cpp", "mcp"}, "invalid_json\n");
  assert(res1_2.out.find("Invalid Request") != std::string::npos);

  auto res2 = run_cli({"cdd-cpp", "mcp"}, "{\"id\":1}\n");
  assert(res2.out.find("Invalid Request") != std::string::npos);

  auto res3 = run_cli(
      {"cdd-cpp", "mcp"},
      "{\"jsonrpc\":\"2.0\",\"id\":\"abc\",\"method\":\"unknown_method\"}\n");
  assert(res3.out.find("Method not found") != std::string::npos);

  auto res4 = run_cli(
      {"cdd-cpp", "mcp"},
      "{\"jsonrpc\":\"2.0\",\"id\":null,\"method\":\"initialize\",\"params\":{"
      "\"protocolVersion\":\"1.0\",\"capabilities\":{},\"clientInfo\":{"
      "\"name\":\"test\",\"version\":\"1.0\"}}}\n");
  assert(res4.out.find("\"serverInfo\"") != std::string::npos);

  auto res5 = run_cli(
      {"cdd-cpp", "mcp"},
      "{\"jsonrpc\":\"2.0\",\"method\":\"notifications/initialized\"}\n");
  assert(res5.code == 0);

  auto res6 =
      run_cli({"cdd-cpp", "mcp"},
              "{\"jsonrpc\":\"2.0\",\"id\":2,\"method\":\"tools/list\"}\n");
  assert(res6.out.find("tools") != std::string::npos);

  auto res7 = run_cli({"cdd-cpp", "mcp"},
                      "{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"tools/"
                      "call\",\"params\":{\"name\":\"test\"}}\n");
  assert(res7.out.find("Tool not found") != std::string::npos);

  auto res7_1 = run_cli({"cdd-cpp", "mcp"},
                        "{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"tools/"
                        "call\",\"params\":{\"name\":\"cdd_generate\"}}\n");
  assert(res7_1.out.find("OK") != std::string::npos);

  // Test cdd_generate with real arguments (from_openapi and to_openapi)
  std::string gen_sdk_out = tmp.path("mcp_sdk_out");
  std::string call_gen_sdk =
      "{\"jsonrpc\":\"2.0\",\"id\":10,\"method\":\"tools/call\",\"params\":{"
      "\"name\":\"cdd_generate\",\"arguments\":{\"command\":\"to_sdk\","
      "\"input\":\"" +
      spec_file + "\",\"output\":\"" + gen_sdk_out + "\"}}}\n";
  auto res_gen_sdk = run_cli({"cdd-cpp", "mcp"}, call_gen_sdk);
  std::error_code fs_ec;
  assert(res_gen_sdk.out.find("OK") != std::string::npos);
  assert(std::filesystem::exists(gen_sdk_out + "/src/client.hpp", fs_ec));

  std::string gen_spec_out = tmp.path("mcp_out_spec.json");
  std::string call_gen_to_oa =
      "{\"jsonrpc\":\"2.0\",\"id\":11,\"method\":\"tools/call\",\"params\":{"
      "\"name\":\"cdd_generate\",\"arguments\":{\"command\":\"to_openapi\","
      "\"input\":\"" +
      gen_sdk_out + "\",\"output\":\"" + gen_spec_out + "\"}}}\n";
  auto res_gen_oa = run_cli({"cdd-cpp", "mcp"}, call_gen_to_oa);
  assert(res_gen_oa.out.find("OK") != std::string::npos);
  assert(std::filesystem::exists(gen_spec_out, fs_ec));

  std::string call_gen_err =
      "{\"jsonrpc\":\"2.0\",\"id\":12,\"method\":\"tools/call\",\"params\":{"
      "\"name\":\"cdd_generate\",\"arguments\":{\"command\":\"to_sdk\","
      "\"input\":\"" +
      tmp.path("missing.json") + "\",\"output\":\"" + gen_sdk_out + "\"}}}\n";
  auto res_gen_err = run_cli({"cdd-cpp", "mcp"}, call_gen_err);
  assert(res_gen_err.out.find("-32603") != std::string::npos);

  auto res_inspect = run_cli(
      {"cdd-cpp", "mcp"}, "{\"jsonrpc\":\"2.0\",\"id\":13,\"method\":\"tools/"
                          "call\",\"params\":{\"name\":\"cdd_inspect\"}}\n");
  assert(res_inspect.out.find("OK") != std::string::npos);

  auto res11 = run_cli({"cdd-cpp", "mcp"}, "\n");
  assert(res11.code == 0);
}

void test_main_cli_coverage() {
  TempDirFixture tmp("cdd_cli_test_cov");
  std::string spec_file = tmp.path("test_spec.json");
  write_str(spec_file, R"({
    "openapi": "3.2.0",
    "paths": {
      "/test": {
        "get": { "operationId": "getTest" }
      }
    }
  })");

  auto err_read_gd =
      run_cli({"cdd-cpp", "from_google_discovery", "to_sdk", "-i",
               tmp.path("nonexistent.json"), "-o", tmp.path("out")});
  assert(err_read_gd.code != 0 &&
         err_read_gd.err.find("Could not open input file") !=
             std::string::npos);

  auto from_server_flags =
      run_cli({"cdd-cpp", "from_openapi", "to_server", "-i", spec_file, "-o",
               tmp.path("out_dir_flags"), "--with-postgres", "--with-faker",
               "--no-installable-package"});
  assert(from_server_flags.code == 0);

  std::string empty_spec = tmp.path("empty_spec.json");
  write_str(empty_spec, "{\"openapi\":\"3.2.0\"}");
  auto from_server_empty =
      run_cli({"cdd-cpp", "from_openapi", "to_server", "-i", empty_spec, "-o",
               tmp.path("out_empty"), "--no-installable-package",
               "--no-github-actions"});
  assert(from_server_empty.code == 0);

  std::string bad_gd = tmp.path("bad_gd.json");
  write_str(bad_gd, "{bad}");
  auto err_parse_gd = run_cli({"cdd-cpp", "from_google_discovery", "to_sdk",
                               "-i", bad_gd, "-o", tmp.path("out")});
  assert(err_parse_gd.code != 0 &&
         err_parse_gd.err.find("Error:") != std::string::npos);

  std::string gd1 = tmp.path("gd1.json");
  write_str(
      gd1,
      R"({"kind":"discovery#restDescription","name":"api1","version":"v1","schemas":{},"resources":{}})");
  std::string gd2 = tmp.path("gd2.json");
  write_str(
      gd2,
      R"({"kind":"discovery#directoryList","items":[{"kind":"discovery#restDescription","name":"api2","version":"v1","schemas":{},"resources":{}},{"kind":"discovery#restDescription","name":"api3","version":"v1","schemas":{},"resources":{}}]})");

  auto gd_dir_res = run_cli({"cdd-cpp", "from_google_discovery", "to_sdk", "-i",
                             gd2, "-o", tmp.path("out_multi")});
  assert(gd_dir_res.code == 0);

  std::error_code fs_ec;
  std::string ro_gd = tmp.path("read_only_gd");
  std::filesystem::create_directories(ro_gd + "/src/client.hpp", fs_ec);
  auto err_out_gd = run_cli(
      {"cdd-cpp", "from_google_discovery", "to_sdk", "-i", gd1, "-o", ro_gd});
  assert(err_out_gd.code != 0 &&
         err_out_gd.err.find("Could not open output file") !=
             std::string::npos);

  auto sync_help = run_cli({"cdd-cpp", "sync", "--help"});
  assert(sync_help.code == 0 &&
         sync_help.out.find("Usage:") != std::string::npos);

  auto sync_err = run_cli({"cdd-cpp", "sync"});
  assert(sync_err.code != 0 &&
         sync_err.err.find("Missing --input or --output") != std::string::npos);

  std::string sync_dir = tmp.path("test_sync_dir");
  std::filesystem::create_directories(sync_dir, fs_ec);
  std::string sync_spec = tmp.path("test_sync_spec.json");
  write_str(sync_spec, "{\"openapi\":\"3.2.0\"}");

  auto sync_ok = run_cli({"cdd-cpp", "sync", "-i", sync_dir, "-o", sync_spec});
  assert(sync_ok.code == 0 &&
         sync_ok.out.find("Syncing code") != std::string::npos);

  auto sync_truth = run_cli(
      {"cdd-cpp", "sync", "-i", sync_dir, "-o", sync_spec, "--truth", "code"});
  assert(sync_truth.code == 0 &&
         sync_truth.out.find("Syncing code") != std::string::npos);

  auto sync_truth_t = run_cli(
      {"cdd-cpp", "sync", "-i", sync_dir, "-o", sync_spec, "-t", "code"});
  assert(sync_truth_t.code == 0 &&
         sync_truth_t.out.find("Syncing code") != std::string::npos);

  setenv("CDD_INPUT", sync_dir.c_str(), 1);
  setenv("CDD_OUTPUT", sync_spec.c_str(), 1);
  auto sync_env = run_cli({"cdd-cpp", "sync"});
  assert(sync_env.code == 0 &&
         sync_env.out.find("Syncing code") != std::string::npos);
  unsetenv("CDD_INPUT");
  unsetenv("CDD_OUTPUT");

  // Exercise remaining CLI flag/env branches
  assert(run_cli({"cdd-cpp", "sync", "-i", sync_dir}).code != 0);
  assert(
      run_cli({"cdd-cpp", "sync", "-i", sync_dir, "-o", sync_spec, "--unknown"})
          .code == 0);
  assert(run_cli({"cdd-cpp", "from_openapi", "to_sdk", "-i", spec_file, "-o",
                  tmp.path("out_m"), "-m", "--unknown"})
             .code == 0);
  assert(run_cli({"cdd-cpp", "from_google_discovery", "to_sdk", "-i", gd1, "-o",
                  tmp.path("out_gd_u"), "--unknown"})
             .code == 0);
  assert(run_cli({"cdd-cpp", "to_openapi", "-i", sync_dir, "--unknown"}).code ==
         0);
  assert(run_cli({"cdd-cpp", "to_docs_json", "-i", empty_spec, "--unknown"})
             .code == 0);
  assert(run_cli({"cdd-cpp", "serve_json_rpc", "--unknown", "--port", "abc"})
             .code != 0);

  setenv("CDD_TESTS", "TRUE", 1);
  setenv("CDD_NO_GITHUB_ACTIONS", "false", 1);
  assert(run_cli({"cdd-cpp", "from_openapi", "to_sdk", "-i", spec_file, "-o",
                  tmp.path("out_env_true")})
             .code == 0);
  assert(
      std::filesystem::exists(tmp.path("out_env_true/tests/client_test.cpp")));
  assert(std::filesystem::exists(
      tmp.path("out_env_true/.github/workflows/ci.yml")));
  unsetenv("CDD_TESTS");
  unsetenv("CDD_NO_GITHUB_ACTIONS");
}

} // namespace cdd_cpp::cli
