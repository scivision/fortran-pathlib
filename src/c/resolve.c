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


size_t fs_canonical(const char* path, const bool strict, char* result, const size_t buffer_size)
{
  // also expands ~
  // distinct from resolve()

  // distinct from resolve
  size_t L = strlen(path);
  if(L == 0)
    return 0;

  if(L == 1 && path[0] == '.')
    return fs_get_cwd(result, buffer_size);

  L = fs_expanduser(path, result, buffer_size);
  if(!L)
    return 0;

  if(strict){
    if(!fs_exists(result)){
      fprintf(stderr, "ERROR:ffilesystem:canonical: \"%s\" => does not exist and strict=true\n", result);
      return 0;
    }
  } else if (!fs_exists(result))
    return fs_normal(result, result, buffer_size);

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  strcpy(buf, result);

  const char* t =
#ifdef _WIN32
  _fullpath(result, buf, buffer_size);
#else
  realpath(buf, result);
#endif
  free(buf);

  L = strlen(result);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:resolve: buffer overflow\n");
    L = 0;
  } else if (!t) {
    fprintf(stderr, "ERROR:ffilesystem:canonical: %s   %s\n", result, strerror(errno));
    L = 0;
  }

  if(L)
    fs_as_posix(result);

  return L;
}


size_t fs_resolve(const char* path, const bool strict, char* result, const size_t buffer_size)
{
  // also expands ~
  // distinct from canonical()

  size_t L = strlen(path);
  if(L == 0 || (L == 1 && path[0] == '.'))
    return fs_get_cwd(result, buffer_size);

  if(!fs_expanduser(path, result, buffer_size))
    return 0;

  if(strict && !fs_exists(result)){
    fprintf(stderr, "ERROR:ffilesystem:resolve: %s => does not exist and strict=true\n", result);
    return 0;
  }

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  strcpy(buf, result);

  const char* t =
#ifdef _WIN32
    _fullpath(result, buf, buffer_size);
#else
    realpath(buf, result);
#endif
  free(buf);

  L = strlen(result);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:resolve: buffer overflow\n");
    L = 0;
  } else if (!t && strict) {  // needs && strict unlike canonical()
    fprintf(stderr, "ERROR:ffilesystem:resolve: %s   %s\n", result, strerror(errno));
    L = 0;
  }

  if(L)
    fs_as_posix(result);

  return L;
}


bool fs_equivalent(const char* path1, const char* path2)
{
// both paths must exist, or they are not equivalent -- return false
// note how much more sophisticated Boost.Filesystem is
// https://github.com/boostorg/filesystem/blob/a0c8edba38a4d31b449fcf7b7ada455977342596/src/operations.cpp#L3813

  struct stat s1, s2;

  if(stat(path1, &s1) != 0 || stat(path2, &s2) != 0)
    return false;
  // printf("TRACE: st_dev: %d %d\n", s1.st_dev, s2.st_dev);
  // printf("TRACE: st_ino: %d %d\n", s1.st_ino, s2.st_ino);
  // https://www.boost.org/doc/libs/1_85_0/libs/filesystem/doc/reference.html#equivalent
  return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
}


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

  const char sep[2] = {fs_pathsep(), '\0'};

// strtok_r, strtok_s not necessarily available, and non-C++ is fallback
  char* p = strtok(path, sep);  // NOSONAR

  while (p) {
    const int N = snprintf(result, buffer_size, "%s/%s", p, name);
    if(N < 0 || N >= (int) buffer_size){
      fprintf(stderr, "ERROR:ffilesystem:which: buffer overflow\n");
      return 0;
    }

    if(fs_is_exe(result)){
      fs_as_posix(result);
      return (size_t) N;
    }
    p = strtok(NULL, sep);  // NOSONAR
  }

  return 0;
}


size_t fs_make_absolute(const char* path, const char* base,
                        char* result, const size_t buffer_size)
{
  size_t L = fs_expanduser(path, result, buffer_size);

  if (fs_is_absolute(result))
    return L;

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;

  if(!fs_expanduser(base, buf, buffer_size)){
    free(buf);
    return L;
  }

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

  L = fs_strncpy(buf2, result, buffer_size);
  free(buf2);
  return L;
}
