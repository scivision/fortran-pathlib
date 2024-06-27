#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ffilesystem.h"


size_t fs_which(const char* name, char* result, const size_t buffer_size)
{

  size_t L;
  if(fs_is_exe(name)){
    L = fs_strncpy(name, result, buffer_size);
    fs_as_posix(result);
    return L;
  }

  // relative directory component, but path was not a file
  if(fs_file_name(name, result, buffer_size) != strlen(name))
    return 0;

  // Windows gives priority to cwd, so check that first
  if(fs_is_windows()){
    L = fs_get_cwd(result, buffer_size);
    if(L)
      L = fs_join(result, name, result, buffer_size);
    if(L && fs_is_exe(result)){
      fs_as_posix(result);
      return L;
    }
  }

  char* path = getenv("PATH");
  if(!path){
    fprintf(stderr, "ERROR:ffilesystem:which: PATH environment variable not set\n");
    return 0;
  }

  if(FS_TRACE) printf("TRACE:which: PATH: %s\n", path);

  const char sep[2] = {fs_pathsep(), '\0'};

// strtok_r, strtok_s not necessarily available
  char* p = strtok(path, sep);  // NOSONAR

  while (p) {
    const int N = snprintf(result, buffer_size, "%s/%s", p, name);
    if(N < 0 || N >= (int) buffer_size){
      fprintf(stderr, "ERROR:ffilesystem:which: buffer overflow\n");
      return 0;
    }

    if(FS_TRACE) printf("TRACE:which: is_file(%s) %d is_exe(%s) %d\n", result, fs_is_file(result), result, fs_is_exe(result));

    if(fs_is_exe(result)){
      fs_as_posix(result);
      return (size_t) N;
    }
    p = strtok(NULL, sep);  // NOSONAR
  }

  return 0;
}
