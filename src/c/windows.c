#include "ffilesystem.h"

#include <stdio.h>
#include <stddef.h> // size_t


size_t fs_longname(const char* in, char* out, size_t buffer_size){
  if(fs_is_windows())
    fprintf(stderr, "ERROR:Filesystem:fs_longname: not implemented for non-C++\n");

  return fs_strncpy(in, out, buffer_size);
}


size_t fs_shortname(const char* in, char* out, size_t buffer_size)
{
  if(fs_is_windows())
    fprintf(stderr, "ERROR:Ffilesystem:fs_shortname: not implemented for non-C++\n");

  return fs_strncpy(in, out, buffer_size);
}
