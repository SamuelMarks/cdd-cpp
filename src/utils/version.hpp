/// @file version.hpp
/// @brief Global version constant for cdd-cpp.

#pragma once
#include <string_view>

#ifndef CDD_CPP_VERSION_STR
#define CDD_CPP_VERSION_STR "0.0.4"
#endif

namespace cdd_cpp::utils {

/// @brief Current release version string of `cdd-cpp`.
inline constexpr std::string_view CDD_CPP_VERSION = CDD_CPP_VERSION_STR;

} // namespace cdd_cpp::utils
