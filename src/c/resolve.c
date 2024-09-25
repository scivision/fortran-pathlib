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

#include <cwalk.h>


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


size_t fs_absolute(const char* path, const char* base, const bool expand_tilde,
                        char* result, const size_t buffer_size)
{
  // works mostly like Python pathlib.Path.absolute()

  char* ex = (char*) malloc(buffer_size);
  if(!ex) return 0;

  size_t Lp;
  // path might be empty, no return checks
  if(expand_tilde)
    Lp = fs_expanduser(path, ex, buffer_size);
  else
    Lp = fs_strncpy(path, ex, buffer_size);

  if(fs_is_absolute(ex)){
    Lp = fs_strncpy(ex, result, buffer_size);
    free(ex);
    return Lp;
  }

  char* bx = (char*) malloc(buffer_size);
  if(!bx){
    free(ex);
    return 0;
  }


  size_t Lb;
  // base might be empty, no return checks
  if(expand_tilde){
    Lb = fs_expanduser(base, bx, buffer_size);
  } else
    Lb = fs_strncpy(base, bx, buffer_size);

  if(Lb == 0 && !fs_get_cwd(bx, buffer_size)){
    free(ex);
    free(bx);
    return 0;
  }

  if(!fs_is_absolute(bx)){
    char* cwd = (char*) malloc(buffer_size);
    if(!cwd){
      free(ex);
      free(bx);
      return 0;
    }
    if(!fs_get_cwd(cwd, buffer_size) || !snprintf(result, buffer_size, "%s/%s", cwd, bx)){
      free(ex);
      free(bx);
      free(cwd);
      return 0;
    }
    free(cwd);
    fs_strncpy(result, bx, buffer_size);
  }

  if(Lp == 0){
    Lp = fs_strncpy(bx, result, buffer_size);
    free(ex);
    free(bx);
    return Lp;
  }

  if(FS_TRACE) printf("TRACE: fs_absolute: base=%s path=%s\n", bx, ex);

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  Lp = cwk_path_get_absolute(bx, ex, result, buffer_size);
  free(ex);
  free(bx);

  fs_as_posix(result);
  return Lp;
}
