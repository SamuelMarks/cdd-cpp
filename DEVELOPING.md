# Developing

## Prerequisites

- C++23 compatible compiler (Clang 16+, GCC 13+, or MSVC 19.36+)
- CMake 3.15+
- `libcurl` development headers
- Python 3.10+ (for coverage and pre-commit scripts)
- Optional: `libpq` (PostgreSQL client library), `vcpkg`, or `wasi-sdk` / `emsdk`

## Steps to Build & Test

1. Install required dependencies (`make deps` or `.\make.bat deps`).
2. Configure and build the project:
   ```sh
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j4
   ```
3. Run the test suite:
   ```sh
   ./build/cdd-tests
   ```
4. Verify 100% documentation and test coverage:
   ```sh
   python3 tools/doc_coverage.py --verbose
   python3 tools/test_coverage.py
   ```
5. Or run the unified pre-commit check suite (formatting, build, tests, coverage, WASM, Docker):
   ```sh
   python3 scripts/run_pre_commit_checks.py
   ```

## Docker Build

Docker images can be tested via:

```sh
docker build -t cdd-cpp-alpine -f alpine.Dockerfile .
docker build -t cdd-cpp-debian -f debian.Dockerfile .
```

This verifies compilation and test execution inside isolated Alpine Linux and Debian environments.
