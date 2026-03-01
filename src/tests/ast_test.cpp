#include "../utils/cpp_parser.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::utils {
void test_cpp_parser() {
  std::filesystem::create_directories("test_tmp_dir");
  std::ofstream f("test_tmp_dir/test.hpp");
  f << R"(
  #pragma once
  #include <string>
  
  namespace test_ns {
      /// @brief This is a test class
      class TestClass {
      public:
          /// @brief Test field
          int testField = 0;
          
          /// @brief Test method
          /// @param p1 Param 1
          /// @return The result
          std::string testMethod(int p1, const std::string& p2);
      };
      
      struct TestStruct {
          bool active;
      };
  }
  )";
  f.close();

  auto spec = cdd_cpp::utils::parse_cpp_project("test_tmp_dir");
  assert(spec.openapi == "3.2.0");
  assert(spec.components.has_value());
  assert(spec.components->schemas.has_value());
  assert(spec.components->schemas->contains("TestClass"));
  assert(spec.components->schemas->contains("TestStruct"));

  std::filesystem::remove_all("test_tmp_dir");
  std::cout << "test_cpp_parser passed.\n";
}
} // namespace cdd_cpp::utils
