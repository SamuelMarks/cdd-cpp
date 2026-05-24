// GCOV_EXCL_BR_START

#include "../functions/emit.hpp"
#include "../functions/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::functions {
void test_parse() {
  parse("{}");
  std::cout << "functions::test_parse passed.\n";
}
void test_emit() {
  std::string res = emit();
  assert(res == "{}");
  std::cout << "functions::test_emit passed.\n";
}
} // namespace cdd_cpp::functions
// GCOV_EXCL_BR_STOP
