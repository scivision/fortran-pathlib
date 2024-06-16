// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <stdbool.h>

#if defined(_WIN32)
#include <sys/utime.h> // _utime
#else
#include <sys/time.h> // utimes
#endif

#include <time.h> // time_t

#include <stdio.h>
#include <string.h>


time_t fs_get_modtime(const char* path)
{
  struct stat s;
  if (!stat(path, &s))
    return s.st_mtime;

  fprintf(stderr, "ERROR:ffilesystem:fs_get_modtime: %s => %s\n", path, strerror(errno));
  return 0;
}


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
