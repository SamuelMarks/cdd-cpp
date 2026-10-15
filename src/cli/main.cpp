/// @file main.cpp
/// @brief Main entrypoint for cdd-cpp delegating to cdd_cpp::cli::main_impl.

#include "api.hpp"
#include <iostream>

/// @brief Main application entrypoint.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @return Exit code (0 on success, non-zero on error).
int main(int argc, char **argv) noexcept {
  return cdd_cpp::cli::main_impl(argc, argv, std::cout, std::cerr, std::cin);
}
