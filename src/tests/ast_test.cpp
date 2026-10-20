
#include "../utils/cpp_ast.hpp"
#include "../utils/cpp_parser.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace cdd_cpp::utils {
void test_cpp_parser() {
  std::error_code ec;
  std::filesystem::create_directories("test_tmp_dir", ec);

  std::ofstream f1("test_tmp_dir/test.hpp");
  f1 << R"(
  /* The best class */
  class TestClass {
      /* A good field */
      int testInt; // A good field
      int32_t testInt32;
      short testShort;
      long testLong;
      int64_t testInt64;
      float testFloat;
      double testDouble;
      bool testBool;
      boolean testBoolean;
      std::string testString;
      string testShortString;
      std::vector<int> testVector;
      vector<int> testShortVec;
      TestStruct testObject;
  };
  )";
  f1.close();

  std::ofstream f2("test_tmp_dir/test2.cpp");
  f2 << R"(
  class TestClass2 {};
  /// @route POST /cli_route
  std::expected<std::string, std::string> handle_cliOp2(cdd_cli::Client& client) noexcept {
      return "";
  }
  )";
  f2.close();

  std::ofstream f3("test_tmp_dir/test3.h");
  f3 << "class TestClass3 {};";
  f3.close();

  std::ofstream f4("test_tmp_dir/test4.c");
  f4 << R"(
  class TestClass4 {};
  /// @route POST /cli_route
  std::expected<std::string, std::string> handle_cliOp(cdd_cli::Client& client) noexcept {
      return "";
  }
  /// @route POST /cli_route_unique
  std::expected<std::string, std::string> handle_cliOpUnique(cdd_cli::Client& client) noexcept {
      return "";
  }
  void run_cli(cdd_cli::Client& client) {
      auto r = handle_cliOpUnique(client);
      auto r2 = handle_unparsed(client, query_params, path_params["id"], body);
  }
  void setup_svr(httplib::Server& svr) {
      svr.Get("/items/{id}", [&](const httplib::Request& req, httplib::Response& res) {});
  }
  )";
  f4.close();
  auto spec = cdd_cpp::utils::parse_cpp_project("test_tmp_dir");
  assert(spec.openapi == "3.2.0");
  assert(spec.components.has_value());
  assert(spec.components->schemas.has_value());
  assert(spec.components->schemas->contains("TestClass"));
  assert(spec.components->schemas->at("TestClass").description ==
         "/* The best class */");
  assert(spec.components->schemas->at("TestClass")
             .properties->contains("testInt"));
  assert(spec.components->schemas->at("TestClass")
             .properties->at("testInt")
             .description == "// A good field");
  assert(spec.components->schemas->contains("TestClass2"));
  assert(spec.components->schemas->contains("TestClass3"));
  assert(spec.components->schemas->contains("TestClass4"));

  std::filesystem::remove_all("test_tmp_dir", ec);

  auto missing_res =
      cdd_cpp::utils::try_parse_cpp_project("nonexistent_dir_xyz");
  assert(!missing_res.has_value());
  assert(missing_res.error().code == ErrorCode::DirectoryIterationError);
  auto fallback_spec = cdd_cpp::utils::parse_cpp_project("nonexistent_dir_xyz");
  assert(fallback_spec.openapi == "3.2.0");

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
  assert(ast.classes.size() == 1);
  assert(ast.classes[0].fields.size() == 2);
  assert(ast.classes[0].fields[0].docstring.find("@doc f1") !=
         std::string::npos);
  assert(ast.functions.size() == 1);
  assert(ast.functions[0].params.size() == 2);
  assert(ast.functions[0].docstring.find("@doc f") != std::string::npos);

  std::string out_cpp = emit_cpp(ast);
  assert(out_cpp.find("int a") != std::string::npos);
  assert(out_cpp.find("double b") != std::string::npos);
  assert(out_cpp.find("@doc f1") != std::string::npos);

  // Test edge cases in parse_cpp: line comments, escaped quotes, templates,
  // default args, declarations, unbalanced braces
  std::string edge_cpp = R"(
  int top_var = 1;

  // Single line comment before struct
  struct EdgeStruct {
      using MyAlias = int;
      /// Pre-field doc
      std::map<std::string, int> mapping = {};
  };

  /// Function with default args, template params, and nested braces
  std::string MyClass::method(const std::vector<int>& items = {},   , int   = 0, const int* = nullptr, int> x) {
      std::string s = "escaped \" quote";
      char c = '\'';
      // comment inside body
      /* * block comment inside body * */
      int div = 4 / 2;
      (void)div;
      if (items.empty()) {
          return s;
      }
      return fn_call(1);
  }

  void proto_only(int x);
  void macro_like() (int y);
  void trailing_no_semi(int z))";
  auto edge_ast = parse_cpp(edge_cpp);
  assert(edge_ast.classes.size() == 1);
  assert(edge_ast.classes[0].fields.size() == 1);
  assert(edge_ast.classes[0].fields[0].docstring.find("Pre-field doc") !=
         std::string::npos);
  assert(edge_ast.functions.size() >= 2);
  std::string emitted_edge = emit_cpp(edge_ast);
  assert(emitted_edge.find("proto_only") != std::string::npos);

  CppAST bare_ast;
  CppClass bare_cls;
  bare_cls.name = "BareCls";
  bare_ast.classes.push_back(bare_cls);
  CppFunction bare_fn;
  bare_fn.return_type = "void";
  bare_fn.name = "bareFn";
  bare_ast.functions.push_back(bare_fn);
  assert(emit_cpp(bare_ast).find("struct BareCls") != std::string::npos);

  // Unbalanced brace/paren inputs
  auto unbal1 = parse_cpp("struct Unbalanced { int a;");
  assert(unbal1.classes.empty());
  auto unbal2 = parse_cpp("void fn(int a {");
  assert(unbal2.functions.empty());
  auto unbal3 = parse_cpp("void fn(int a) { if (true) {");
  assert(unbal3.functions.empty());
  auto unbal4 = parse_cpp("struct UnbalStr { std::string s = \"unclosed; };");
  assert(unbal4.classes.empty());
  auto unbal5 = parse_cpp("struct UnbalCmt { /* unclosed };");
  assert(unbal5.classes.empty());
  auto unbal6 = parse_cpp("struct UnbalLineCmt { // unclosed");
  assert(unbal6.classes.empty());
  auto unbal7 = parse_cpp("// top-level unclosed line comment");
  assert(unbal7.classes.empty());
  auto unbal8 = parse_cpp("/* top-level unclosed block comment *");
  assert(unbal8.classes.empty());
  auto unbal_str_top = parse_cpp("const char *s = \"unclosed");
  assert(unbal_str_top.classes.empty());
  auto unbal9 = parse_cpp("struct TrailingEOF");
  assert(unbal9.classes.empty());
  auto unbal10 = parse_cpp("(int a); > b(int c);");
  assert(unbal10.functions.size() == 1);
  auto line0_doc = parse_cpp("// Line0 doc\nstruct Line0Struct {};");
  assert(line0_doc.classes.size() == 1);
  assert(line0_doc.classes[0].docstring == "// Line0 doc");

  auto qual_fns = parse_cpp(R"(
    const char *kFake = "struct FakeInString { \\\" };";
    struct Derived : Base {
      // field doc
      /* block doc */
      public:
      int x = 1;
      void inline_method() {}
    };
    inline virtual static std::map<std::string, std::vector<int>> get_nested() { return {}; }
    (void)0;
  )");
  assert(qual_fns.classes.size() == 1);
  assert(qual_fns.classes[0].methods.size() == 1);
  assert(qual_fns.classes[0].methods[0].name == "inline_method");
  assert(!qual_fns.functions.empty());

  std::cout << "test_cpp_parser passed.\n";
}
} // namespace cdd_cpp::utils
