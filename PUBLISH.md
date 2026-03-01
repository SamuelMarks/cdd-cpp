# Publishing cdd-cpp

## C++ Package Managers (Conan/vcpkg)
C++ doesn't have a single dominant package registry, but `Conan` and `vcpkg` are the most popular. 
To publish `cdd-cpp`:
1. **Conan**: Create a `conanfile.py`, define the build requirements and CMake build step, and run `conan create .` then `conan upload`.
2. **vcpkg**: Submit a port to the `microsoft/vcpkg` repository via PR containing a `vcpkg.json` and `portfile.cmake`.

## Documentation Publishing
1. Generate the local HTML documentation folder:
   ```bash
   make build_docs docs/
   ```
   This generates a `docs/html/` directory using Doxygen.
2. **Your Own Server**: Sync the `docs/html/` folder to your server (e.g., using `rsync` or AWS S3).
3. **Most Popular Location (GitHub Pages)**: Commit the `docs/html/` folder to a `gh-pages` branch, or use a GitHub Action (`peaceiris/actions-gh-pages`) to automatically deploy the generated Doxygen folder to GitHub Pages.
