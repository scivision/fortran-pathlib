#include <iostream>

#include <cstdlib>
#include <cstdint>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

  std::uintmax_t c = fs_hard_link_count(".");

  if(c < 1)
    err("hard_link_count: " + std::to_string(c));

  ok_msg("hard_link_count C++");

  return EXIT_SUCCESS;
}
