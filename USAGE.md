# Usage
The `cdd-cpp` CLI allows parsing and emitting OpenAPI specifications and C++ models/clients.
## CLI Options
- `cdd-cpp --help` : Show help message.
- `cdd-cpp --version` : Print version.
- `cdd-cpp from_openapi -i <spec.json>` : Parse OpenAPI spec and generate C++ client.
- `cdd-cpp to_openapi -f <path/to/code>` : Parse C++ code and emit OpenAPI spec.
- `cdd-cpp to_docs_json [--no-imports] [--no-wrapping] -i <spec.json>` : Extract documentation.
## Library
Integrate into your C++ codebase by including the library headers and linking against `cdd_lib`.
