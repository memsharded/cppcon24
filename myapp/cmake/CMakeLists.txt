cmake_minimum_required(VERSION 3.15)

project(MyApp CXX)

find_package(OpenSSL CONFIG REQUIRED)
add_executable(myapp ../myapp.cpp)
target_link_libraries(myapp PRIVATE OpenSSL::Crypto)
