#include "ffilesystem.h"

#include <stdio.h>
#include <stdlib.h> // for realpath, _fullpath
#include <string.h> // for strlen


size_t fs_realpath(const char* path, char* result, const size_t buffer_size)
{

  const char* t =
#ifdef _WIN32
  _fullpath(result, path, buffer_size);
#else
  realpath(path, result);
#endif

  (void) t;

  size_t L = strlen(result);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:realpath: buffer overflow\n");
    return 0;
  }

  fs_as_posix(result);

  return L;

}
