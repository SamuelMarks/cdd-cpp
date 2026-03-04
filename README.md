cdd-cpp
============

[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI/CD](https://github.com/offscale/cdd-cpp/workflows/CI/badge.svg)](https://github.com/offscale/cdd-cpp/actions)
![Test Coverage](https://img.shields.io/badge/coverage-100.00%25-brightgreen.svg) ![Doc Coverage](https://img.shields.io/badge/docs-100.00%25-brightgreen.svg)

OpenAPI ↔ C++. This is one compiler in a suite, all focussed on the same task: Compiler Driven Development (CDD).

Each compiler is written in its target language, is whitespace and comment sensitive, and has both an SDK and CLI.

The CLI—at a minimum—has:
- `cdd-cpp --help`
- `cdd-cpp --version`
- `cdd-cpp from_openapi -i spec.json`
- `cdd-cpp to_openapi -f path/to/code`
- `cdd-cpp to_docs_json --no-imports --no-wrapping -i spec.json`

The goal of this project is to enable rapid application development without tradeoffs. Tradeoffs of Protocol Buffers / Thrift etc. are an untouchable "generated" directory and package, compile-time and/or runtime overhead. Tradeoffs of Java or JavaScript for everything are: overhead in hardware access, offline mode, ML inefficiency, and more. And neither of these alterantive approaches are truly integrated into your target system, test frameworks, and bigger abstractions you build in your app. Tradeoffs in CDD are code duplication (but CDD handles the synchronisation for you).

## 🚀 Capabilities

The `cdd-cpp` compiler leverages a unified architecture to support various facets of API and code lifecycle management.

* **Compilation**:
  * **OpenAPI → `C++`**: Generate idiomatic native models, network routes, client SDKs, database schemas, and boilerplate directly from OpenAPI (`.json` / `.yaml`) specifications.
  * **`C++` → OpenAPI**: Statically parse existing `C++` source code and emit compliant OpenAPI specifications.
* **AST-Driven & Safe**: Employs static analysis (Abstract Syntax Trees) instead of unsafe dynamic execution or reflection, allowing it to safely parse and emit code even for incomplete or un-compilable project states.
* **Seamless Sync**: Keep your docs, tests, database, clients, and routing in perfect harmony. Update your code, and generate the docs; or update the docs, and generate the code.

## 📦 Installation

Requires CMake 3.15+ and a C++20 compiler.\n\n```bash\nmake build\n```

## 🛠 Usage

### Command Line Interface

```bash
CDD CLI (Code-Driven Development)
Usage:
  cdd-cpp --help
  cdd-cpp --version
  cdd-cpp to_openapi -f <path/to/code> [-o <spec.json>]
  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] -i <spec.json> [-o <docs.json>]
  cdd-cpp from_openapi to_sdk_cli -i <spec.json> -o <target_directory>
  cdd-cpp from_openapi to_sdk_cli --input-dir <specs_dir> -o <target_directory>
  cdd-cpp from_openapi to_sdk -i <spec.json> -o <target_directory>
  cdd-cpp from_openapi to_sdk --input-dir <specs_dir> -o <target_directory>
  cdd-cpp from_openapi to_server -i <spec.json> -o <target_directory>
  cdd-cpp from_openapi to_server --input-dir <specs_dir> -o <target_directory>
  cdd-cpp serve_json_rpc --port <port> --listen <host>

Commands:
  sync         : Bi-directional sync of code directory and OpenAPI spec.
  from_openapi : Parses an OpenAPI spec and emits C++ code.
  to_openapi   : Parses C++ code and emits an OpenAPI spec.
  to_docs_json : Generates JSON documentation for API calls.
  serve_json_rpc: Starts JSON-RPC server.

```

```bash
./cdd-cpp to_openapi -f src/
./cdd-cpp from_openapi to_sdk_cli -i spec.json -o out/
```

### Programmatic SDK / Library

```cpp
#include "cdd_cpp/openapi/parse.hpp"
#include "cdd_cpp/classes/emit_cli.hpp"

int main() {
  auto spec = cdd_cpp::openapi::parse("{\"openapi\": \"3.2.0\"}");
  std::string code = cdd_cpp::classes::emit_cli(spec);
}
```

## Design choices

We use AST parsing instead of reflection because C++ reflection is not natively available yet. By statically parsing headers/source files, we can generate idiomatic API clients and CLIs.

## 🏗 Supported Conversions for C++

*(The boxes below reflect the features supported by this specific `cdd-cpp` implementation)*

| Concept | Parse (From) | Emit (To) |
|---------|--------------|-----------|
| OpenAPI (JSON/YAML) | ✅ | ✅ |
| `C++` Models / Structs / Types | ✅ | ✅ |
| `C++` Server Routes / Endpoints | ✅ | ✅ |
| `C++` API Clients / SDKs | ✅ | ✅ |
| `C++` ORM / DB Schemas | ✅ | ✅ |
| `C++` CLI Argument Parsers | ✅ | ✅ |
| `C++` Docstrings / Comments | ✅ | ✅ |



---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.