# Publish Guide

To publish `cdd-cpp` to C++ package registries, we recommend utilizing **vcpkg** or **Conan**, or distributing pre-built native and WebAssembly binaries via **GitHub Releases**.

## vcpkg

1. Update the port manifest (`vcpkg.json` / `portfile.cmake`) pointing to the release tag and commit SHA512.
2. Open a Pull Request on the main [vcpkg ports repository](https://github.com/microsoft/vcpkg).

## GitHub Releases

Pre-built native binaries (`cdd-cpp`, `cdd-cpp.exe`) and standalone WebAssembly modules (`cdd-cpp.wasm`) are published directly to GitHub Releases. The `.github/workflows/ci.yml` pipeline builds and uploads these artifacts automatically on pushes to `master`/`main` and tag creation.

## Coverage & Badges

To verify 100% test and documentation coverage and refresh `README.md` badges locally:

```sh
python3 scripts/update_badges.py
```
