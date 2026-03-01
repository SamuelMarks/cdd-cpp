# Publishing cdd-cpp

For C++ packages, the most common distribution methods are system package managers, **vcpkg**, or **Conan**. Since cdd-cpp provides a standalone CLI, it can also be distributed via GitHub Releases.

## Binary Releases
We use GitHub Actions to build binaries for Windows, Linux, and macOS. Upon a new tag, binaries are attached to the release.

## Publishing to Conan (ConanCenter)
To upload this library to ConanCenter (the most popular location for C++ packages):
1. Create a conanfile.py defining the package build steps.
2. Submit a Pull Request to the conan-center-index repository on GitHub.
3. Once merged, users can install it via `conan install cdd-cpp/1.0.0@`.

## Publishing to vcpkg
To upload this library to the vcpkg registry:
1. Create a vcpkg.json manifest and a portfile.cmake.
2. Submit a Pull Request to the microsoft/vcpkg repository.
3. Once merged, users can install it via `vcpkg install cdd-cpp`.

## Publishing Documentation
To generate and serve docs locally (producing a local folder for static serving):
```bash
make build_docs docs_out
cd docs_out/html
python3 -m http.server
```

To upload docs to the most popular location (GitHub Pages or ReadTheDocs):
- **GitHub Pages**: Set up a GitHub Action to deploy the generated `docs_out/html` folder to the `gh-pages` branch.
- **ReadTheDocs**: Link your repository to ReadTheDocs and configure it to run Doxygen and Breathe/Sphinx.
