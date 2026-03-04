# cdd-cpp

[![CI](https://github.com/samuel/cdd-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/samuel/cdd-cpp/actions/workflows/ci.yml)
[![Test Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen.svg)](#)
[![Doc Coverage](https://img.shields.io/badge/docs-100%25-brightgreen.svg)](#)

A bidirectional compiler that converts between OpenAPI specifications and native C++ code (models, routes, ORMs, clients) using AST/static analysis.

## Features

- **Bidirectional Compilation**: Generate an OpenAPI spec from your C++ codebase, or generate C++ code (models, routes, clients) from an OpenAPI spec.
- **Format Upgrades**: Automatically upgrades older Swagger (2.0) and OpenAPI (3.0/3.1) specifications to the latest supported OpenAPI 3.2.0 format.
- **Google Discovery Integration**: Parses Google Discovery API JSON files and maps them into standard OpenAPI specifications.
- **AST/Static Analysis**: Accurate and context-aware conversion.
- **WASM Support**: Compile `cdd-cpp` to WebAssembly for execution in the browser or via unified CLI tools.
- **JSON-RPC Server**: Expose the compiler as a JSON-RPC service.

## Installation

```bash
git clone https://github.com/samuel/cdd-cpp
cd cdd-cpp
make build
```

## CLI Usage

```
cdd-cpp --help
cdd-cpp --version
cdd-cpp to_openapi -f <path/to/code> -o spec.json
cdd-cpp serve_json_rpc --port 8082 --listen 0.0.0.0
cdd-cpp to_docs_json --no-imports --no-wrapping -i spec.json -o docs.json
cdd-cpp from_openapi to_sdk_cli -i spec.json -o target_directory
cdd-cpp from_openapi to_sdk_cli --input-dir ./specs/ -o target_directory
cdd-cpp from_openapi to_sdk -i spec.json -o target_directory
cdd-cpp from_openapi to_sdk --input-dir ./specs/ -o target_directory
cdd-cpp from_openapi to_server -i spec.json -o target_directory
cdd-cpp from_openapi to_server --input-dir ./specs/ -o target_directory
```

WASM support is possible and implemented. See [WASM.md](WASM.md) for details.
