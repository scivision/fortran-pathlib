#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <stdbool.h>
#include <stdio.h>
#include <limits.h> // realpath()
#include <stdlib.h> // malloc(), free(), realpath(), _fullpath()

#include <string.h>

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


size_t fs_canonical(const char* path, const bool strict, const bool expand_tilde,
                    char* result, const size_t buffer_size)
{
  // distinct from resolve()

  // distinct from resolve
  const size_t L = strlen(path);
  if(L == 0)
    return 0;

  if(L == 1 && path[0] == '.')
    return fs_get_cwd(result, buffer_size);

  if(expand_tilde){
    if(!fs_expanduser(path, result, buffer_size))
      return 0;
  } else
    fs_strncpy(path, result, buffer_size);

  if(strict){
    if(!fs_exists(result)){
      fprintf(stderr, "ERROR:ffilesystem:canonical: \"%s\" => does not exist and strict=true\n", result);
      return 0;
    }
  } else if (!fs_exists(result))
    return fs_normal(result, result, buffer_size);

  return fs_realpath(result, result, buffer_size);
}


size_t fs_resolve(const char* path, const bool strict, const bool expand_tilde,
                  char* result, const size_t buffer_size)
{
  // works like canonical(absolute(path)).
  // Inspired by Python pathlib.Path.resolve()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.resolve

  const size_t L = strlen(path);
  if(L == 0 || (L == 1 && path[0] == '.'))
    return fs_get_cwd(result, buffer_size);

  if(expand_tilde){
    if(!fs_expanduser(path, result, buffer_size))
      return 0;
  } else
    fs_strncpy(path, result, buffer_size);

  if(strict && !fs_exists(result)){
    fprintf(stderr, "ERROR:ffilesystem:resolve: %s => does not exist and strict=true\n", result);
    return 0;
  }

  return fs_realpath(result, result, buffer_size);
}


bool fs_equivalent(const char* path1, const char* path2)
{
// both paths must exist, or they are not equivalent -- return false
// note how much more sophisticated Boost.Filesystem is
// https://github.com/boostorg/filesystem/blob/a0c8edba38a4d31b449fcf7b7ada455977342596/src/operations.cpp#L3813

  struct stat s1;
  struct stat s2;

  if(stat(path1, &s1) != 0 || stat(path2, &s2) != 0)
    return false;
// https://www.boost.org/doc/libs/1_85_0/libs/filesystem/doc/reference.html#equivalent
  return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
}


size_t fs_absolute(const char* path, const char* base, const bool expand_tilde,
                        char* result, const size_t buffer_size)
{
  size_t L;

  // path might be empty, no return checks
  if(expand_tilde)
    L = fs_expanduser(path, result, buffer_size);
  else
    L = fs_strncpy(path, result, buffer_size);

  if (fs_is_absolute(result)){
    if(FS_TRACE) printf("TRACE: fs_absolute: %s is already absolute\n", result);
    return L;
  }

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;

  // base might be empty, no return checks
  if(expand_tilde){
    fs_expanduser(base, buf, buffer_size);
  } else
    fs_strncpy(base, buf, buffer_size);

  if(FS_TRACE) printf("TRACE: fs_absolute: base=%s path=%s\n", buf, result);

  char* buf2 = (char*) malloc(buffer_size);
  if(!buf2){
    free(buf);
    return 0;
  }
  L = fs_join(buf, result, buf2, buffer_size);
  free(buf);
  if(L == 0){
    free(buf2);
    return 0;
  }

  if(FS_TRACE) printf("TRACE: fs_absolute: joined=%s\n", buf2);

  L = fs_strncpy(buf2, result, buffer_size);
  free(buf2);
  return L;
}
