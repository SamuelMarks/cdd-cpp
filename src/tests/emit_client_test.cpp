#include "../classes/emit_client.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::classes {
void test_emit_client() {
  openapi::OpenAPI spec;
  spec.servers = std::vector<openapi::Server>{{"https://api.example.com"}};
  openapi::PathItem pi;
  openapi::Operation get_op;
  get_op.operationId = "getPet";
  pi.get = get_op;
  std::map<std::string, openapi::PathItem> paths;
  paths["/pet"] = pi;
  spec.paths = paths;

  std::string generated = emit_client(spec);

  assert(generated.find("class Client") != std::string::npos);
  assert(generated.find("curl_easy_init") != std::string::npos);
  assert(generated.find(
             "curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());") !=
         std::string::npos);
  assert(generated.find("std::string getPet()") != std::string::npos);
  assert(generated.find("base_url + \"/pet\"") != std::string::npos);

  std::cout << "classes::test_emit_client passed.\n";
}
} // namespace cdd_cpp::classes
