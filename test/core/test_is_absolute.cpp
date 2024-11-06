#include <iostream>
#include "ffilesystem.h"
#include <cstdlib>

int main() {

if (fs_is_absolute("")) {
  std::cerr << "blank is not absolute\n";
  return EXIT_FAILURE;
}

if(fs_is_windows()){
    if (!fs_is_absolute("J:/")) {
        std::cerr << "is_absolute(J:/) on Windows should be true\n";
        return EXIT_FAILURE;
    }
    if (!fs_is_absolute("j:/")) {
        std::cerr << "is_absolute(j:/) on Windows should be true\n";
        return EXIT_FAILURE;
    }
    if (fs_is_absolute("/")) {
        std::cerr << "is_absolute(/) on Windows should be false\n";
        return EXIT_FAILURE;
    }
} else {
    if (!fs_is_absolute("/")) {
        std::cerr << "is_absolute(/) on Unix should be true\n";
        return EXIT_FAILURE;
    }
    if (fs_is_absolute("j:/")) {
        std::cerr << "is_absolute(j:/) on Unix should be false\n";
        return EXIT_FAILURE;
    }
}

    return EXIT_SUCCESS;
}
