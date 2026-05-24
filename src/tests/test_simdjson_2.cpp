// GCOV_EXCL_BR_START

#include "../openapi/parse.hpp"
#include <iostream>

int main() {
  auto spec_res = cdd_cpp::openapi::parse("{ invalid json }");
  if (!spec_res) {
    std::cout << "Error: " << spec_res.error() << "\n";
  }
  return 0;
}
// GCOV_EXCL_BR_STOP
