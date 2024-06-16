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
  if(!fid){
    fprintf(stderr, "ERROR:ffilesystem:touch:fopen %s => %s\n", path, strerror(errno));
    return false;
  }

  if(fclose(fid) != 0){
    fprintf(stderr, "ERROR:ffilesystem:touch:fclose %s => %s\n", path, strerror(errno));
    return false;
  }

  if(fs_is_file(path))
    return true;
  // is_file check because sometimes fclose() doesn't fail, but the file is not created

  fprintf(stderr, "ERROR:Ffilesystem:touch: %s was not created\n", path);
  return false;
}
