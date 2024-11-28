#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "myfs.h"


int main(int argc, char* argv[]) {

  const bool has = (argc < 2) ? has_filename(".") : has_filename(argv[1]);

  printf("%d\n", has);

  return EXIT_SUCCESS;
}
