#include "parse.hpp"

namespace cdd_cpp::mocks {
void parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  auto result = parser.parse(input);
  if (result.error()) {
    return;
  }
  // TODO: implement parsing for mocks
}
} // namespace cdd_cpp::mocks
