#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define MYPKG_EXPORT __declspec(dllexport)
#else
  #define MYPKG_EXPORT
#endif

MYPKG_EXPORT void mypkg();
MYPKG_EXPORT void mypkg_print_vector(const std::vector<std::string> &strings);
