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
  // canonicalize path, i.e. resolve all symbolic links, remove ".", ".." and extra slashes
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

  if(!fs_absolute(path, "", expand_tilde, result, buffer_size))
    return 0;

  return fs_canonical(result, strict, false, result, buffer_size);
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
