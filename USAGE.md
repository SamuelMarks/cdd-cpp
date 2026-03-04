# Usage

`cdd-cpp` can be used as a CLI tool or integrated into your C++ codebase.

## CLI

```bash
cdd-cpp --help
```

### Server Sync

Run `cdd-cpp serve_json_rpc` to expose an HTTP server on port 8080 (customizable via `--port`).

## Library Usage

To integrate `cdd_lib` into your CMake project:

```cmake
add_subdirectory(cdd-cpp)
target_link_libraries(your_target PRIVATE cdd_lib)
```