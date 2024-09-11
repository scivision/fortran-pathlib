#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include <limits.h>
#include <stdlib.h> // for realpath, malloc, free
#include <string.h> // for strlen


int main(void){

  const char* in = "./";
  char out[PATH_MAX];

  const char* t = realpath(in, out);

  size_t L = strlen(out);
  if(L >= PATH_MAX)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
