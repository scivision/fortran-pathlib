#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <stdio.h>  // snprintf
#include <string.h>

// maximum path length
#if defined (__APPLE__)
#include <sys/syslimits.h>
#elif !defined (_MSC_VER)
#include <limits.h>
#endif
// end maximum path length


size_t fs_get_max_path(){

  size_t m = 256;
#if defined(PATH_MAX)
  m = PATH_MAX;
#elif defined (_MAX_PATH)
  m = _MAX_PATH;
#elif defined (_POSIX_PATH_MAX)
  m = _POSIX_PATH_MAX;
#endif
  return (m < 4096) ? m : 4096; // arbitrary absolute maximum

}

char fs_pathsep(){
  return fs_is_windows() ? ';' : ':';
}

const char* fs_devnull(){
  return fs_is_windows() ? "nul" : "/dev/null";
}


void fs_as_posix(char* path)
{
// force posix file seperator on Windows
  if(!(fs_is_windows() || fs_is_cygwin()))
    return;

  for (char* p = path; *p != '\0'; p++) {
    if (*p == '\\')
      *p = '/';
  }
}

void fs_drop_slash(char* path)
{
  // drop single final "/" if present
  const size_t L = strlen(path);
  if (L > 1 && path[L - 1] == '/')
    path[L - 1] = '\0';
}


bool fs_is_subdir(const char* subdir, const char* dir)
{
  // is subdir a subdirectory of dir
  const size_t m = fs_get_max_path();

  char* buf = (char*) malloc(m);
  if(!buf) return false;

  const size_t L = fs_relative_to(dir, subdir, buf, m);
  const bool yes = L > 0 && !(L==1 && buf[0] == '.') && strncmp(buf, "..", 2) != 0;

  free(buf);

  return yes;
}


size_t fs_strncpy(const char* path, char* result, const size_t buffer_size)
{
// check size before copy
  size_t L = strlen(path);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:Ffilesystem:strncpy: output buffer %zu too small\n", buffer_size);
    return 0;
  }

  if(L)
    strncpy(result, path, buffer_size);

  return L;
}
