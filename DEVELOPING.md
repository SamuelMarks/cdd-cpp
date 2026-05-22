# Developing

## Pre-requisites

- C++23 compatible compiler (GCC, Clang, MSVC)
- CMake 3.15+
- Python 3.10+ (for tools)
- Optional: `vcpkg` or `emsdk`

## Steps to build

1. Install required packages (CMake, Python3, C++ Compiler)
2. Generate project using `cmake -B build -S .` and build with `cmake --build build`
3. Run tests using `./build/cdd-tests`
4. Or run the unified pre-commit checks: `python3 scripts/run_pre_commit_checks.py`

## Docker build

Docker images can be tested via `docker build -t cdd-cpp-alpine -f alpine.Dockerfile .`. This tests compilation inside an isolated Alpine and Debian 12 environment.
