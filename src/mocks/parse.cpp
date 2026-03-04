#include "parse.hpp"

namespace cdd_cpp::mocks {
void parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  auto doc = parser.parse(input);
  (void)doc;
  // TODO: implement parsing for mocks
}
} // namespace cdd_cpp::mocks
