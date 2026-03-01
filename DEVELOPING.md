# Developing `cdd-cpp`
## Requirements
- C++20 compatible compiler
- CMake 3.15+
- Make / Ninja
## Building
```bash
make install_deps
make build
```
## Testing
```bash
make test
```
## Adding new features
- Follow the modular architecture (`src/{functions,classes,docstrings,routes,tests,mocks,openapi}/{parse,emit}.cpp`).
- Add tests to `src/tests`. We require 100% test coverage.
- Add docstrings to all exposed headers. We require 100% doc coverage.
