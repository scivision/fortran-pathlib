#include <stdio.h>
#include <stddef.h>


size_t fs_exe_path(char* path, size_t buffer_size)
{
  (void) path; (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:fs_exe_path: not implemented for non-C++\n");
  return 0;
}

size_t fs_lib_path(char* path, size_t buffer_size)
{
  (void) path; (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:fs_lib_path: not implemented for non-C++\n");
  return 0;
}
