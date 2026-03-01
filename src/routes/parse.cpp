#include "parse.hpp"

namespace cdd_cpp::routes {
void parse(const std::string &input) {
  simdjson::dom::parser parser;
  auto doc = parser.parse(input);
  // TODO: implement parsing for routes
}
} // namespace cdd_cpp::routes
