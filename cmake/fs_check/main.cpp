#include <cstdlib>
#include <iostream>

#include "myfs.h"


int main(int argc, char* argv[]) {

  const bool has = (argc < 2) ? has_filename(".") : has_filename(argv[1]);

  std::cout << has << "\n";

  return EXIT_SUCCESS;
}
