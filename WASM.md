# WASM Build Support
`cdd-cpp` supports compiling to WebAssembly using Emscripten. This allows running the parser and emitter logic within a web browser or a unified CLI tool without requiring native installations.
## Prerequisites
- Emscripten SDK (`emsdk`). The `make` tasks assume it is available at `../emsdk` or via your system`s `emcc`.
## Building
```bash
make build_wasm
```
This will produce `cdd-cpp.js` and `cdd-cpp.wasm` in the `custom_bin/` directory.
