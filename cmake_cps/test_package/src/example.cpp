#include "mypkg.h"
#include <vector>
#include <string>

int main() {
    mypkg();

    std::vector<std::string> vec;
    vec.push_back("test_package");

    mypkg_print_vector(vec);
}
