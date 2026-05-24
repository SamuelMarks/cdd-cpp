// GCOV_EXCL_BR_START

#include "../utils/cpp_ast.hpp"
#include "../utils/cpp_parser.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::utils {
void test_cpp_parser() {
  std::filesystem::create_directories("test_tmp_dir"); // GCOV_EXCL_BR_LINE

  std::ofstream f1("test_tmp_dir/test.hpp");
  f1 << R"(
  /* The best class */
  class TestClass {
      /* A good field */
      int testInt; // A good field
      long testLong;
      float testFloat;
      double testDouble;
      bool testBool;
      std::string testString;
      std::vector<int> testVector;
      TestStruct testObject;
  };
  )";
  f1.close(); // GCOV_EXCL_BR_LINE

  std::ofstream f2("test_tmp_dir/test2.cpp"); // GCOV_EXCL_BR_LINE
  f2 << "class TestClass2 {};";               // GCOV_EXCL_BR_LINE
  f2.close();                                 // GCOV_EXCL_BR_LINE

  std::ofstream f3("test_tmp_dir/test3.h"); // GCOV_EXCL_BR_LINE
  f3 << "class TestClass3 {};";             // GCOV_EXCL_BR_LINE
  f3.close();                               // GCOV_EXCL_BR_LINE

  std::ofstream f4("test_tmp_dir/test4.c"); // GCOV_EXCL_BR_LINE
  f4 << "class TestClass4 {};";             // GCOV_EXCL_BR_LINE
  f4.close();                               // GCOV_EXCL_BR_LINE
  auto spec =
      cdd_cpp::utils::parse_cpp_project("test_tmp_dir");   // GCOV_EXCL_BR_LINE
  assert(spec.openapi == "3.2.0");                         // GCOV_EXCL_BR_LINE
  assert(spec.components.has_value());                     // GCOV_EXCL_BR_LINE
  assert(spec.components->schemas.has_value());            // GCOV_EXCL_BR_LINE
  assert(spec.components->schemas->contains("TestClass")); // GCOV_EXCL_BR_LINE
  assert(spec.components->schemas->at("TestClass")
             .description == // GCOV_EXCL_BR_LINE
         "/* The best class */");
  assert(spec.components->schemas
             ->at("TestClass") // GCOV_EXCL_BR_LINE
             .properties->contains("testInt"));
  assert(spec.components->schemas
             ->at("TestClass") // GCOV_EXCL_BR_LINE
             .properties->at("testInt")
             .description == "// A good field");
  assert(spec.components->schemas->contains("TestClass2")); // GCOV_EXCL_BR_LINE
  assert(spec.components->schemas->contains("TestClass3")); // GCOV_EXCL_BR_LINE
  assert(spec.components->schemas->contains("TestClass4")); // GCOV_EXCL_BR_LINE

  std::filesystem::remove_all("test_tmp_dir"); // GCOV_EXCL_BR_LINE

  std::string cpp_code = R"(
  /* @doc A */
  class A {
      int f1; // @doc f1
      double f2;
  };
  /* @doc f */
  int f(int a, double b) {
      return 0;
  }
  )";
  auto ast = parse_cpp(cpp_code);
  assert(ast.classes.size() == 1);           // GCOV_EXCL_BR_LINE
  assert(ast.classes[0].fields.size() == 2); // GCOV_EXCL_BR_LINE
  assert(
      ast.classes[0].fields[0].docstring.find("@doc f1") != // GCOV_EXCL_BR_LINE
      std::string::npos);
  assert(ast.functions.size() == 1);           // GCOV_EXCL_BR_LINE
  assert(ast.functions[0].params.size() == 2); // GCOV_EXCL_BR_LINE
  assert(ast.functions[0].docstring.find("@doc f") !=
         std::string::npos); // GCOV_EXCL_BR_LINE

  std::string out_cpp = emit_cpp(ast);
  assert(out_cpp.find("int a") != std::string::npos);    // GCOV_EXCL_BR_LINE
  assert(out_cpp.find("double b") != std::string::npos); // GCOV_EXCL_BR_LINE
  assert(out_cpp.find("@doc f1") != std::string::npos);  // GCOV_EXCL_BR_LINE

  std::cout << "test_cpp_parser passed.\n"; // GCOV_EXCL_BR_LINE
}
} // namespace cdd_cpp::utils
// GCOV_EXCL_BR_STOP
