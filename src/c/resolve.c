#include "ffilesystem.h"

#include <stdio.h>
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), free()


bool fs_is_subdir(const char* subdir, const char* dir)
{
  // is subdir a subdirectory of dir
  const size_t m = fs_get_max_path();

  char* buf = (char*) malloc(m);
  if(!buf) return false;

  size_t L = fs_relative_to(dir, subdir, buf, m);
  bool yes = L > 0 && buf[0] != '.';

  free(buf);

  return yes;

}


size_t fs_make_absolute(const char* path, const char* base,
                        char* result, size_t buffer_size)
{
  size_t L1 = fs_expanduser(path, result, buffer_size);

  if (fs_is_absolute(result))
    return L1;

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  size_t L2 = fs_expanduser(base, buf, buffer_size);
  if(L2 == 0){
    free(buf);
    return L1;
  }

  char* buf2 = (char*) malloc(buffer_size);
  if(!buf2){
    free(buf);
    return 0;
  }
  L1 = fs_join(buf, result, buf2, buffer_size);
  free(buf);
  if(L1 == 0){
    free(buf2);
    return 0;
  }

  L1 = fs_strncpy(buf2, result, buffer_size);
  free(buf2);
  return L1;
}
