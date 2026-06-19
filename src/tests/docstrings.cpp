
#include "../docstrings/emit.hpp"
#include "../docstrings/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::docstrings {
void test_parse() {
  parse("test");
  std::cout << "docstrings::test_parse passed.\n";
}
void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test API";
  spec.info.version = "1.0";
  spec.info.description = "Line1\r\nLine2";

  openapi::ServerVariable sv;
  sv.default_value = "def";
  sv.enum_values = std::vector<std::string>{"val1", "val2"};
  sv.description = "desc";

  openapi::Server srv;
  srv.url = "http://localhost";
  srv.variables = std::map<std::string, openapi::ServerVariable>{{"myVar", sv}};
  spec.servers = std::vector<openapi::Server>{srv};

  openapi::Operation op;
  op.summary = "Op summary";
  op.description = "Op desc\r\nLine2";

  openapi::Parameter p;
  p.name = "myParam";
  p.in = "query";
  p.description = "Param desc\r\nLine2";
  op.parameters = std::vector<openapi::Parameter>{p};

  openapi::PathItem pi;
  pi.description = "Path desc\r\nLine2";
  pi.get = op;

  spec.paths = std::map<std::string, openapi::PathItem>{{"/test", pi}};

  std::string out = emit_api_docstrings(spec);
  assert(out.find("Line1") != std::string::npos);
  assert(out.find("Line2") != std::string::npos);
  assert(out.find("val1,val2") != std::string::npos);

  std::cout << "docstrings::test_emit passed.\n";
}
} // namespace cdd_cpp::docstrings
