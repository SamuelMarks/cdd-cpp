# Usage

`cdd-cpp` can be used as a standalone CLI compiler, a JSON-RPC 2.0 / MCP server, or integrated directly into your C++23 codebase via `cdd_lib`.

## CLI Usage

```sh
cdd-cpp --help
```

### Generating C++ Code from OpenAPI (`from_openapi`)

```sh
# Generate a C++ Client SDK (libcurl + simdjson) with GoogleTest suite
cdd-cpp from_openapi to_sdk -i spec.json -o ./sdk --tests

# Generate a C++ CLI application wrapping the Client SDK
cdd-cpp from_openapi to_sdk_cli -i spec.json -o ./cli --tests

# Generate a modular C++ HTTP server (cpp-httplib) with PostgreSQL (libpq) ORM and Faker stubs
cdd-cpp from_openapi to_server -i spec.json -o ./server --tests --with-postgres --with-faker

# Generate a standalone Model Context Protocol (MCP) server project from an OpenAPI spec
cdd-cpp from_openapi to_server -i spec.json -o ./mcp_server --mcp
```

### Extracting OpenAPI 3.2.0 from C++ Code (`to_openapi`)

```sh
cdd-cpp to_openapi -i ./server -o openapi.json
```

### Generating & Extracting Arazzo 1.1.0 Workflows (`from_arazzo` / `to_arazzo`)

```sh
# Emit C++ Client SDK with Arazzo 1.1.0 workflow orchestrators
cdd-cpp from_arazzo to_sdk -i workflows.arazzo.json -o ./workflows_sdk --tests

# Emit C++ CLI for executing Arazzo workflows
cdd-cpp from_arazzo to_sdk_cli -i workflows.arazzo.json -o ./workflows_cli --tests

# Emit C++ Server workflow engine endpoints
cdd-cpp from_arazzo to_server -i workflows.arazzo.json -o ./workflows_server --tests

# Reverse-parse C++ workflow code back into an Arazzo 1.1.0 specification
cdd-cpp to_arazzo -i ./workflows_sdk -o workflows.updated.arazzo.json
```

### Bidirectional In-Place Synchronization (`sync`)

```sh
# Sync C++ directory with OpenAPI spec (code as source of truth by default)
cdd-cpp sync -i ./src -o spec.json -t code

# Sync C++ directory with OpenAPI spec (spec as source of truth)
cdd-cpp sync -i ./src -o spec.json -t spec
```

### Google Discovery API Compilation (`from_google_discovery`)

```sh
cdd-cpp from_google_discovery to_sdk -i discovery.json -o ./google_sdk --tests
cdd-cpp from_google_discovery to_sdk_cli -i discovery.json -o ./google_cli --tests
```

### Documentation Snippet Generation (`to_docs_json`)

```sh
cdd-cpp to_docs_json -i spec.json -o docs.json [--no-imports] [--no-wrapping]
```

### JSON-RPC 2.0 & MCP Servers

- **JSON-RPC 2.0 HTTP Server**: Run `cdd-cpp serve_json_rpc --port 8080 --listen 127.0.0.1` to expose all CLI subcommands over HTTP JSON-RPC 2.0.
- **Model Context Protocol (MCP) Server**: Run `cdd-cpp mcp` to start a JSON-RPC 2.0 STDIO server implementing MCP `2024-11-05` (`from_openapi`, `to_openapi`, `sync`, and `cdd_generate`).

## Library Usage (`cdd_lib`)

To integrate `cdd_lib` into your CMake project:

```cmake
add_subdirectory(cdd-cpp)
target_link_libraries(your_target PRIVATE cdd_lib)
```

All public APIs return `cdd_cpp::utils::Result<T>` (`std::expected<T, cdd_cpp::utils::Error>`) and are marked `noexcept`.
