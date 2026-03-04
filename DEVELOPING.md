# Developing

## Pre-requisites

- C++23 compatible compiler (GCC, Clang, MSVC)
- CMake 3.15+
- Python 3.10+ (for tools)
- Optional: `vcpkg` or `emsdk`

## Steps to build

1. Run `make install_base`
2. Run `make install_deps`
3. Generate project using `make build`
4. Run tests using `make test`

## Docker build

Docker images can be tested via `make build_docker`. This tests compilation inside an isolated Alpine and Debian 12 environment.
