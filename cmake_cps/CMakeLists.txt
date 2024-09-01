cmake_minimum_required(VERSION 3.15)
project(mypkg CXX)

set(CMAKE_EXPERIMENTAL_EXPORT_PACKAGE_INFO "b80be207-778e-46ba-8080-b23bba22639e")

add_library(mypkg src/mypkg.cpp)
target_include_directories(mypkg PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

set_target_properties(mypkg PROPERTIES PUBLIC_HEADER "include/mypkg.h")
install(TARGETS mypkg EXPORT mypkg)

install(PACKAGE_INFO mypkg EXPORT mypkg)
