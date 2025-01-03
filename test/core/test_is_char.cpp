#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <string_view>

#include <iostream>
#include <cstdlib>

int main() {

// /dev/stdin may not be available on CI systems

const std::string p = fs_is_windows() ? "NUL" : "/dev/null";

if (!fs_is_char_device(p))
  err("is_char_device(" + p + ") should be true");

if (fs_is_file(p))
  err("is_file(" + p + ") should be false");

ok_msg("is_char_device(" + p + ")");

return EXIT_SUCCESS;

}
