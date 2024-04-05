#include "ffilesystem.h"

#include <stdio.h>
#include <stddef.h>


size_t fs_exe_path(FFS_MUNUSED_C char* path, FFS_MUNUSED_C size_t buffer_size)
{
  fprintf(stderr, "ERROR:ffilesystem:fs_exe_path: not implemented for non-C++\n");
  return 0;
}

size_t fs_lib_path(FFS_MUNUSED_C char* path, FFS_MUNUSED_C size_t buffer_size)
{
  fprintf(stderr, "ERROR:ffilesystem:fs_lib_path: not implemented for non-C++\n");
  return 0;
}
