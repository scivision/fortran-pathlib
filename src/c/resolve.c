#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"
#include <cwalk.h>

#include <stdio.h>
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), free()
#include <string.h>
#include <errno.h>


size_t fs_canonical(const char* path, bool strict, char* result, size_t buffer_size)
{
  // also expands ~
  // distinct from resolve()

  if(strlen(path) == 0)
    return 0;

  if(strlen(path) == 1 && path[0] == '.')
    return fs_get_cwd(result, buffer_size);

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  size_t L = fs_expanduser(path, buf, buffer_size);
  if(L == 0){
    free(buf);
    return 0;
  }

  bool e = fs_exists(buf);

  if(!e) {
    if(strict){
      fprintf(stderr, "ERROR:ffilesystem:canonical: \"%s\" => does not exist and strict=true\n", buf);
    } else {
      L = fs_normal(buf, result, buffer_size);
    }
    free(buf);
    return L;
  }

  char* buf2 = (char*) malloc(buffer_size);
  if(!buf2) {
    free(buf);
    return 0;
  }

#ifdef _WIN32
  const char* t = _fullpath(buf2, buf, buffer_size);
#else
  const char* t = realpath(buf, buf2);
#endif

  if (!t) {
    fprintf(stderr, "ERROR:ffilesystem:canonical: %s   %s\n", buf, strerror(errno));
    free(buf);
    free(buf2);
    return 0;
  }
  free(buf);

  L = fs_strncpy(buf2, result, buffer_size);
  free(buf2);

  if(L)
    fs_as_posix(result);

  return L;
}


size_t fs_resolve(const char* path, bool strict, char* result, size_t buffer_size)
{
  // also expands ~
  // distinct from canonical()

  if(strlen(path) == 0 || (strlen(path) == 1 && path[0] == '.'))
    return fs_get_cwd(result, buffer_size);

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  size_t L = fs_expanduser(path, buf, buffer_size);
  if(L == 0){
    free(buf);
    return 0;
  }

  bool e = fs_exists(buf);

  if(!e) {
    if(strict){
      fprintf(stderr, "ERROR:ffilesystem:resolve: %s => does not exist and strict=true\n", buf);
      free(buf);
      return 0;
    } else if (fs_is_absolute(buf)){
      L = fs_normal(buf, result, buffer_size);
      free(buf);
      return L;
    } else if (!fs_join(".", buf, buf, buffer_size)){
      free(buf);
      return 0;
    }
  }

  char* buf2 = (char*) malloc(buffer_size);
  if(!buf2) {
    free(buf);
    return 0;
  }

#ifdef _WIN32
  const char* t = _fullpath(buf2, buf, buffer_size);
#else
  const char* t = realpath(buf, buf2);
#endif

  if (!t && strict) {
    fprintf(stderr, "ERROR:ffilesystem:resolve: %s   %s\n", buf, strerror(errno));
    free(buf);
    free(buf2);
    return 0;
  }
  free(buf);

  L = fs_strncpy(buf2, result, buffer_size);
  free(buf2);

  if(L)
    fs_as_posix(result);

  return L;
}


bool fs_equivalent(const char* path1, const char* path2)
{
// both paths must exist, or they are not equivalent -- return false

  const size_t m = fs_get_max_path();

  char* buf1 = (char*) malloc(m);
  if(!buf1) return false;
  char* buf2 = (char*) malloc(m);
  if(!buf2) {
    free(buf1);
    return false;
  }

  if(!fs_canonical(path1, true, buf1, m) || !fs_canonical(path2, true, buf2, m) ||
    !(fs_is_dir(buf1) || fs_is_dir(buf2) || fs_is_file(buf1) || fs_is_file(buf2))){

      free(buf1);
      free(buf2);
      return false;
  }

  bool eqv = strcmp(buf1, buf2) == 0;
  free(buf1);
  free(buf2);

  return eqv;
}


size_t fs_which(const char* name, char* result, size_t buffer_size)
{

  if(fs_is_absolute(name)){
    if(fs_is_exe(name))
      return fs_strncpy(name, result, buffer_size);

    return 0;
  }

  char* path = getenv("PATH");
  if(!path){
    fprintf(stderr, "ERROR:ffilesystem:which: PATH environment variable not set\n");
    return 0;
  }

  const char sep[2] = {fs_pathsep(), '\0'};

// strtok_r, strtok_s not necessarily available, and non-C++ is fallback
  char* p = strtok(path, sep);  // NOSONAR
  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;

  while (p) {
    fs_join(p, name, buf, buffer_size);

    if(fs_is_exe(buf)){
      size_t L = fs_strncpy(buf, result, buffer_size);
      free(buf);
      return L;
    }
    p = strtok(NULL, sep);  // NOSONAR
  }

  free(buf);
  return 0;
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
