#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <string_view>

#include <iostream>
#include <cstdlib>

int main() {
// make a test with std::filesystem::is_fifo()

std::string_view p;

#ifdef _WIN32
    p = "CONIN$";
#else
    p = "/dev/stdin";
#endif


if (fs_is_fifo(p))
    std::cout << ": FIFO\n";

if (fs_is_char_device(p))
    std::cout << ": character file\n";

return EXIT_SUCCESS;

}
