import os

def run(cmd):
    ret = os.system(cmd)
    if ret != 0:
        raise Exception(f"Command failed {cmd}")

# CMake
# run("conan install --requires=openssl/[*] -of=myapp/cmake/build -g CMakeToolchain -g CMakeDeps")
# run("cmake -S myapp/cmake -B myapp/cmake/build -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake")
# run("cmake --build myapp/cmake/build --config Release")
# run(r".\myapp\cmake\build\Release\myapp.exe")

# # Meson
# run("conan install --requires=openssl/[*] --tool-requires=pkgconf/[*] -of=myapp/meson/build -g MesonToolchain -g PkgConfigDeps")
# run(r".\myapp\meson\build\conanbuild.bat && meson setup --native-file=myapp/meson/build/conan_meson_native.ini myapp/meson/build myapp/meson")
# run(r".\myapp\meson\build\conanbuild.bat && meson compile -C myapp/meson/build")
# run(r".\myapp\meson\build\myapp.exe")

# Visual Studio
run("conan install --requires=openssl/[*] -of=myapp/visual/build -g MSBuildDeps -g MSBuildToolchain")
run(r".\myapp\visual\build\conanbuild.bat && msbuild myapp/visual/myapp.sln /p:Configuration=Release /p:Platform=x64")
run(r".\myapp\visual\x64\Release\myapp.exe")