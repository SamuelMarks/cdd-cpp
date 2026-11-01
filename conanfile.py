from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class CddCppConan(ConanFile):
    name = "cdd-cpp"
    version = "0.0.5"
    license = "Apache-2.0 OR MIT"
    url = "https://github.com/offscale/cdd-cpp"
    description = "Bidirectional C++23 <-> OpenAPI 3.2.0, Arazzo 1.1.0, Google Discovery, and MCP compiler and code generator."
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("simdjson/3.9.5")
        self.requires("libcurl/8.7.1")
        self.requires("libpq/15.5")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["cdd_lib"]
