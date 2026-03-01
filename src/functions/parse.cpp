#include "parse.hpp"

namespace cdd_cpp::functions {
void parse(const std::string &input) {
  simdjson::dom::parser parser;
  auto doc = parser.parse(input);
  // TODO: implement parsing for functions
}
} // namespace cdd_cpp::functions
