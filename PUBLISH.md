# Publish Guide

To publish `cdd-cpp` to C++ package registries, we recommend utilizing **vcpkg** or **Conan**.

## Vcpkg

1. Generate a `vcpkg.json` file pointing to the repository and correct versions.
2. Open a Pull Request on the main [vcpkg ports repository](https://github.com/microsoft/vcpkg).

## GitHub Releases

We publish pre-built binaries (including WASM) directly to GitHub Releases. The CI/CD pipeline does this automatically on tag creation.

## Documentation

To generate and serve local documentation:

```bash
make build_docs
python -m http.server -d docs
```

To publish docs online, push them to the `gh-pages` branch using a GitHub Action.
