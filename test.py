import os

def run(cmd):
    ret = os.system(cmd)
    if ret != 0:
        raise Exception(f"Command failed {cmd}")

# CMake
run("conan install myapp -of=myapp/cmake/build -g CMakeToolchain -g CMakeDeps")
run("cmake -S myapp/cmake -B myapp/cmake/build -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake")
run("cmake --build myapp/cmake/build --config Release")
run(r".\myapp\cmake\build\Release\myapp.exe")