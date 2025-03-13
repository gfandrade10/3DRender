from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake

class OpenGLProject(ConanFile):
    name = "OpenGLPhysics"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("imgui/1.91.8")
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("glad/0.1.36")
        self.requires("opengl/system")
        self.requires("opengl-registry/20240721")
        self.requires("stb/cci.20240531")

    def configure(self):
        self.output.info("No specifig configuration")

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
        self.folders.generators = "build"

    def generate(self):
        Toolchain = CMakeToolchain(self)
        Toolchain.variables["CMAKE_RUNTIME_OUTPUT_DIRECTORY"] = "${CMAKE_SOURCE_DIR}"
        Toolchain.variables["CMAKE_LIBRARY_OUTPUT_DIRECTORY"] = "${CMAKE_SOURCE_DIR}/libs"
        Toolchain.variables["CMAKE_MAKE_PROGRAM"] = "C:/mingw64/bin/mingw32-make.exe"
        Toolchain.variables["CMAKE_CXX_COMPILER"] = "C:/mingw64/bin/g++.exe"
        Toolchain.variables["CMAKE_C_COMPILER"] = "C:/mingw64/bin/gcc.exe"
        Toolchain.variables["CMAKE_CXX_STANDARD"] = "20"
        Toolchain.variables["CMAKE_CXX_FLAGS"] = "-O2 -Wall -Werror -Wpedantic"
        Toolchain.generate()

        Deps = CMakeDeps(self)
        Deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.output.info("No package export")

    def package_info(self):
        self.cpp_info.includedirs = ["includes"]
        self.cpp_info.libdirs = ["libs"]
