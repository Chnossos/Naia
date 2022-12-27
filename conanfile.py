from conan             import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class NaiaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "boost/1.69.0",
        "libmysqlclient/8.0.30",
        ("zlib/1.2.12", "override"),
    ]
    default_options = {
        "*:shared": False,
    }
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
