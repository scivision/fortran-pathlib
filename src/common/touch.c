#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "ffilesystem.h"


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
