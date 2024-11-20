#include "ffilesystem.h"

#include <iostream>

#include <cstdlib>
#include <cstdint>

int main() {

  uintmax_t c = fs_hard_link_count(".");

  if(c < 1){
    std::cerr << "ERROR: hard_link_count: " << c << "\n";
    return EXIT_FAILURE;
  }



  std::cout << "OK: hard_link_count(.) " << c << "\n";

  return EXIT_SUCCESS;
}
