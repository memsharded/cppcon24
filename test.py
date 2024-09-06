import os
import platform


def run(cmd):
    print("*"*50)
    print("**** RUNNING: ", cmd)
    print("*"*50)
    ret = os.system(cmd)
    if ret != 0:
        raise Exception(f"Command failed {cmd}")

if platform.system() == "Windows":
    # CMake
    os.chdir("myapp/cmake")
    run("conan install .")
    run("cmake --preset conan-default")
    run("cmake --build --preset conan-release")
    run(r".\build\Release\myapp.exe")

    # Meson
    os.chdir("../meson")
    run("conan install . -of=build")
    run(r".\build\conanbuild.bat && meson setup --native-file=build/conan_meson_native.ini build .")
    run(r".\build\conanbuild.bat && meson compile -C build")
    run(r".\build\myapp.exe")

    # Visual Studio
    os.chdir("../visual")
    run("conan install .")
    # Just open VS IDE
    run(r".\conan\conanbuild.bat && msbuild myapp.sln /p:Configuration=Release /p:Platform=x64")
    run(r".\x64\Release\myapp.exe")
else:
    # Makefiles (only Linux)
    os.chdir("myapp/make")
    # Use ping "$(hostname).local" for remote WSL and add "conan remote add" for server in host
    run("conan install .")
    run("make")
    run("./myapp")
