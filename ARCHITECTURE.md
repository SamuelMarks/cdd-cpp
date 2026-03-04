# Architecture

`cdd-cpp` is designed using a modular, layer-based architecture heavily influenced by parsing and emission strategies (AST/Static analysis).

## Directory Structure

- `src/functions/`: Functional block parsers and emitters.
- `src/classes/`: Class models parsers and emitters (SDK models, logic).
- `src/docstrings/`: Parsing and emitting inline and block docstrings.
- `src/routes/`: Handling route parsing and emission (Web frameworks like Drogon, or similar).
- `src/mocks/`: Generating and parsing mocked client responses and test stubs.
- `src/openapi/`: Core OpenAPI logic (parsing spec, emitting spec).
  - `upgraders/`: Upgrades legacy Swagger/OpenAPI formats to the target OpenAPI specification.
- `src/google_discovery/`: Parses Google Discovery JSON formats and maps them into internal OpenAPI ASTs.
- `src/tests/`: Exhaustive testing suite.

## Control Flow

1. **Parsing Input**:
   - For C++ to OpenAPI, source code is read, AST structures are parsed, classes and functions are resolved, and documentation is merged.
   - For OpenAPI to C++, the JSON/YAML spec is digested, structures are created, and emission templates/generators are triggered.

2. **Bidirectional Synchronization**:
   - `sync` processes match the models internally and apply changes to both targets simultaneously.

## Tooling Integration
- JSON handling via `simdjson` for performance.
- HTTP clients via `libcurl`.
- PostgreSQL handlers for ORM logic generation.