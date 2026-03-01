#include "parse.hpp"

namespace cdd_cpp::docstrings {
void parse(const std::string &input) {
  simdjson::dom::parser parser;
  auto doc = parser.parse(input);
  // TODO: implement parsing for docstrings
}
} // namespace cdd_cpp::docstrings
