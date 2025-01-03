#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <string_view>

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{

const std::string p = (argc > 1) ? argv[1] : argv[0];

if (fs_is_fifo(p))
  err("is_fifo(" + p + ") should be false");

return EXIT_SUCCESS;

}
