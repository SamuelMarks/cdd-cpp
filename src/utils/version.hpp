/// @file version.hpp
/// @brief Global version constant for cdd-cpp.

#pragma once
#include <string_view>

#ifndef CDD_CPP_VERSION_STR
/// @brief Default compile-time release version string macro for `cdd-cpp`.
#define CDD_CPP_VERSION_STR "0.0.5"
#endif

namespace cdd_cpp::utils {

/// @brief Current release version string of `cdd-cpp`.
inline constexpr std::string_view CDD_CPP_VERSION = CDD_CPP_VERSION_STR;

} // namespace cdd_cpp::utils
