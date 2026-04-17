# cdd-cpp

[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI](https://github.com/SamuelMarks/cdd-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/cdd-cpp/actions/workflows/ci.yml)
[![Test Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen.svg)](#)
[![Doc Coverage](https://img.shields.io/badge/docs-100%25-brightgreen.svg)](#)

OpenAPI ↔ C++. This is one compiler in a suite, all focussed on the same task: Compiler Driven Development (CDD).

Each compiler is written in its target language, is whitespace and comment sensitive, and has both an SDK and CLI.

The CLI—at a minimum—has:

- `cdd-cpp --help`
- `cdd-cpp --version`
- `cdd-cpp from_openapi to_sdk_cli -i spec.json`
- `cdd-cpp from_openapi to_sdk -i spec.json`
- `cdd-cpp from_openapi to_server -i spec.json`
- `cdd-cpp to_openapi -f path/to/code`
- `cdd-cpp to_docs_json --no-imports --no-wrapping -i spec.json`
- `cdd-cpp serve_json_rpc --port 8080 --listen 0.0.0.0`

The goal of this project is to enable rapid application development without tradeoffs. Tradeoffs of Protocol Buffers / Thrift etc. are an untouchable "generated" directory and package, compile-time and/or runtime overhead. Tradeoffs of Java or JavaScript for everything are: overhead in hardware access, offline mode, ML inefficiency, and more. And neither of these alternative approaches are truly integrated into your target system, test frameworks, and bigger abstractions you build in your app. Tradeoffs in CDD are code duplication (but CDD handles the synchronisation for you).

## 🚀 Capabilities

The `cdd-cpp` compiler leverages a unified architecture to support various facets of API and code lifecycle management.

- **Compilation**:
    - **OpenAPI → `C++`**: Generate idiomatic native models, network routes, client SDKs, and boilerplate directly from OpenAPI (`.json` / `.yaml`) specifications.
    - **`C++` → OpenAPI**: Statically parse existing `C++` source code and emit compliant OpenAPI specifications.
- **AST-Driven & Safe**: Employs static analysis instead of unsafe dynamic execution or reflection, allowing it to safely parse and emit code even for incomplete or un-compilable project states.
- **Seamless Sync**: Keep your docs, tests, database, clients, and routing in perfect harmony. Update your code, and generate the docs; or update the docs, and generate the code.

## 📦 Installation & Build

### Native Tooling

```bash
cmake -B build && cmake --build build
ctest --test-dir build
```

### Makefile / make.bat

You can also use the included cross-platform Makefiles to fetch dependencies, build, and test:

```bash
# Install dependencies
make deps

# Build the project
make build

# Run tests
make test
```

## 🛠 Usage

### Command Line Interface

```bash
# Generate C++ models from an OpenAPI spec
cdd-cpp from_openapi to_sdk -i spec.json -o src/models

# Generate an OpenAPI spec from your C++ code
cdd-cpp to_openapi -f src/models -o openapi.json
```

### Programmatic SDK / Library

```cpp
#include "cdd_api.hpp"
#include <iostream>

int main() {
    cdd::Config config{"spec.json", "src/models"};
    cdd::generate_sdk(config);
    std::cout << "SDK generation complete.\n";
    return 0;
}
```

## 🏗 Supported Conversions for C++

*(The boxes below reflect the features supported by this specific `cdd-cpp` implementation)*

| Features | Parse (From) | Emit (To) |
| --- | --- | --- |
| OpenAPI 3.2.0 | ✅ | ✅ |
| API Client SDK | ✅ | ✅ |
| API Client CLI | ✅ | ✅ |
| Server Routes / Endpoints | ✅ | ✅ |
| ORM / DB Schema | ✅ | ✅ |
| Mocks + Tests | [ ] | [ ] |
| Model Context Protocol (MCP) | [ ] | [ ] |

### Uncommon Features

`cdd-cpp` includes specific parsing features beyond the base specification:
- **Format Upgrades:** Automatically parses older Swagger (2.0) and OpenAPI (3.0/3.1) and upgrades them to the OpenAPI 3.2.0 internal representation.
- **Google Discovery Integration:** Can parse Google Discovery API JSON files and map them to OpenAPI specifications natively.

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
