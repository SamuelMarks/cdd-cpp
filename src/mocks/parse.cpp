#include "parse.hpp"

namespace cdd_cpp::mocks {
void parse(const std::string &input) noexcept {
  simdjson::dom::parser parser;
  auto result = parser.parse(input); // GCOV_EXCL_BR_LINE
  if (result.error()) {
    return;
  }
  // TODO: implement parsing for mocks
} // GCOV_EXCL_BR_LINE
} // namespace cdd_cpp::mocks
