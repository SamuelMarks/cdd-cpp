# Architecture

`cdd-cpp` is designed using a modular, layer-based C++23 architecture centered on strongly typed Intermediate Representations (IR) and trivia-preserving Abstract Syntax Tree (AST) transformations.

## Directory Structure

- `src/openapi/`: OpenAPI 3.2.0 and JSON Schema Draft 2020-12 data models (`models.hpp`), `simdjson` parser (`parse.cpp`), `JsonWriter` emitter (`emit.cpp`), and multi-version upgrader (`upgraders/upgrader.cpp`) for Swagger 1.2, Swagger 2.0, and OpenAPI 3.0/3.1.
- `src/arazzo/`: Arazzo 1.1.0 workflow models (`models.hpp`), ABNF runtime expression parser/evaluator and Criterion engine (`expressions.cpp`), `simdjson` parser (`parse.cpp`), and `JsonWriter` emitter (`emit.cpp`).
- `src/mcp/`: Model Context Protocol (`2024-11-05`) models (`models.hpp`), JSON-RPC 2.0 parsers (`parse.cpp`), and emitters (`emit.cpp`).
- `src/google_discovery/`: Google Discovery document and directory manifest parser (`parse.cpp`) targeting OpenAPI 3.2.0 models.
- `src/client_sdk/`: Bidirectional C++ HTTP client (`libcurl` + `simdjson`) and Arazzo `Workflows` orchestrator emitters (`emit.cpp`) and parsers (`parse.cpp`).
- `src/client_sdk_cli/`: Bidirectional C++ CLI application emitters (`emit.cpp`) and parsers (`parse.cpp`) for OpenAPI and Arazzo specs.
- `src/server/`: Bidirectional C++ HTTP server (`cpp-httplib`), Arazzo `WorkflowEngine`, and JSON-RPC 2.0 server emitters (`emit.cpp`) and route parsers (`parse.cpp`).
- `src/models/`: C++ data model (`struct`) and JSON serialization emitter (`emit.cpp`) and AST schema extractor (`parse.cpp`).
- `src/orm/`: PostgreSQL (`libpq`) DAO, Stub/Faker DAO, `RepositoryFactory`, and SQL DDL migration emitter (`emit.cpp`).
- `src/mocks/`: Bidirectional GoogleMock (`IClient` / `MockClient` and `IWorkflows` / `MockWorkflows`) emitters (`emit.cpp`) and parsers (`parse.cpp`).
- `src/functions/`: Bidirectional standalone C++ function signature emitters (`emit.cpp`) and parsers (`parse.cpp`) for OpenAPI and Arazzo.
- `src/docstrings/`: Doxygen `@tag` docstring parsers (`parse.cpp`) and emitters (`emit.cpp`) for lossless OpenAPI and Arazzo metadata round-tripping.
- `src/cli/`: Top-level CLI entrypoint (`main.cpp`), configuration API (`api.hpp`), subcommand dispatcher, in-place AST `sync`, `to_docs_json`, `serve_json_rpc`, and `mcp` STDIO server (`sync.cpp`).
- `src/utils/`: Core C++ lexer and trivia-preserving AST (`cpp_ast.cpp`), recursive project directory scanner (`cpp_parser.cpp`), unified error handling (`error.hpp` using `std::expected<T, Error>`), `JsonWriter` (`json_writer.hpp`), and HTTP client wrapper (`http.cpp`).
- `src/tests/`: Comprehensive unit and integration test suites achieving 100% line, function, and branch coverage.

## Design Principles

1. **Zero Exceptions / `std::expected` Error Model**: All fallible functions are marked `noexcept` and return `cdd_cpp::utils::Result<T>` (`std::expected<T, cdd_cpp::utils::Error>`) backed by a single `ErrorCode` enum.
2. **Trivia-Preserving Bidirectional AST**: `cdd_cpp::utils::parse_cpp` and `emit_cpp` preserve leading and trailing comments/whitespace (`Trivia`) and function bodies so `cdd-cpp sync` updates signatures and schemas in-place without clobbering user implementations.
3. **High-Performance Serialization**: Uses `simdjson` On-Demand for zero-copy JSON parsing and a deterministic RFC 8259 `JsonWriter` for emission.
