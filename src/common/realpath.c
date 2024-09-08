#include "ffilesystem.h"

#include <stdio.h>
#include <stdlib.h> // for realpath, _fullpath
#include <string.h> // for strlen


size_t fs_realpath(const char* path, char* result, const size_t buffer_size)
{

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  strcpy(buf, path);
  // macOS must have this malloc/strcpy else the output is a single character
  // when fs_realpath is called from fs_canonical or fs_resolve.

  const char* t =
#ifdef _WIN32
  _fullpath(result, buf, buffer_size);
#else
  realpath(buf, result);
#endif
  free(buf);

  (void) t;

  size_t L = strlen(result);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:realpath: buffer overflow\n");
    return 0;
  }

  fs_as_posix(result);

  return L;

}
