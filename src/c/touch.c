#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <sys/utime.h> // _utime
#else
#include <sys/time.h> // utimes
#endif

#include "ffilesystem.h"


bool fs_set_modtime(const char* path)
{
  if (
#ifdef _WIN32
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64
    _utime(path, NULL)
#else
    utimes(path, NULL)
#endif
    ){
      fprintf(stderr, "ERROR:Ffilesystem:set_modtime: %s => %s\n", path, strerror(errno));
      return false;
    }

  return true;
}


bool fs_touch(const char* path)
{
  if(fs_exists(path))
    return fs_set_modtime(path);

  FILE* fid = fopen(path, "a+b");
  if(fid && fclose(fid) == 0)
    return true;

  fprintf(stderr, "ERROR:Ffilesystem:touch: %s => %s\n", path, strerror(errno));
  return false;
}
