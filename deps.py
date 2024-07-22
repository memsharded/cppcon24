import os

def run(cmd):
    ret = os.system(cmd)
    if ret != 0:
        raise Exception(f"Command failed {cmd}")


run("conan create recipes/zlib/all --version=1.3.1 --build=missing")