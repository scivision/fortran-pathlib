// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <time.h>

#include <stdio.h>
#include <string.h>


time_t
fs_get_modtime(const char* path)
{
  struct stat s;
  if (!stat(path, &s))
    return s.st_mtime;

  fprintf(stderr, "ERROR:ffilesystem:fs_get_modtime: %s => %s\n", path, strerror(errno));
  return 0;
}
