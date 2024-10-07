#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"
#include <cwalk.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h>
#include <ctype.h> // isalpha


bool fs_cpp(){
// tell if fs core is C or C++
  return false;
}


long fs_lang(){
#ifdef __STDC_VERSION__
  return __STDC_VERSION__;
#else
  return 0L;
#endif
}


size_t fs_normal(const char* path, char* result, const size_t buffer_size)
{
// normalize path
  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  const size_t L = cwk_path_normalize(path, result, buffer_size);

  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:normal: output buffer %zu too small\n", buffer_size);
    return 0;
  }

  if(L)
    fs_as_posix(result);

  return L;
}


size_t fs_file_name(const char* path, char* result, const size_t buffer_size)
{
  size_t L = strlen(path);
  // need this check to avoid cwalk segfault
  if(L == 0)
    return 0;

  // same as C++17 std::filesystem::path::filename()
  if (path[L-1] == '/' || (fs_is_windows() && path[L-1] == '\\'))
    return 0;

  const char *base;

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  cwk_path_get_basename(path, &base, &L);

  return fs_strncpy(base, result, buffer_size);
}


size_t fs_stem(const char* path, char* result, const size_t buffer_size)
{
  if(!fs_file_name(path, result, buffer_size))
    return 0;

  // handle special case a/..
  if (strcmp(result, "..") == 0)
    return 2;

  char* pos = strrchr(result, '.');
  if (pos && pos != result)
    *pos = '\0';

  return strlen(result);
}


size_t fs_join(const char* path, const char* other, char* result, const size_t buffer_size)
{
  const size_t L1 = strlen(path);
  const size_t L2 = strlen(other);

  if(L1 == 0 && L2 == 0)
    return 0;

  if(L1 == 0)
    return fs_strncpy(other, result, buffer_size);

  if(L2 == 0)
    return fs_strncpy(path, result, buffer_size);

  cwk_path_set_style(CWK_STYLE_UNIX);

  return cwk_path_join(path, other, result, buffer_size);
}


size_t fs_parent(const char* path, char* result, const size_t buffer_size)
{

  // drop repeated slashes
  size_t L = strlen(path);
  size_t j = 0;
  for (size_t i = 0; i <= L; i++) {
    if (path[i] == '/' || (fs_is_windows() && path[i] == '\\')) {
      if (j > 0 && result[j-1] == '/')
        continue;
    }
    if(j >= buffer_size){
      fprintf(stderr, "ERROR:ffilesystem:fs_parent: buffer_size %zu too small for string\n", buffer_size);
      return 0;
    }
    result[j++] = (fs_is_windows() && path[i] == '\\') ? '/' : path[i];
  }
  j--;
  result[j] = '\0';

  if(j == 1 && result[0] == '/')
    return fs_strncpy("/", result, buffer_size);

  if (fs_is_windows() && (j==2 || j==3) && isalpha(result[0]) && result[1] == ':')
    return fs_root(path, result, buffer_size);

  if(j > 0 && result[j-1] == '/')
    result[j-1] = '\0';

  char *pos = strrchr(result, '/');
  if(!pos)
    return fs_strncpy(".", result, buffer_size);
  if (pos != result)
    *pos = '\0';

  L = strlen(result);
  if(L==0)
    return fs_strncpy(".", result, buffer_size);

  return L;

}


size_t fs_suffix(const char* path, char* result, const size_t buffer_size)
{
  // leave malloc() in -- eliminating it by getting clever with pointer
  // failed on some systems in the path_t Fortran type jumbling the string
  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;

  if(!fs_file_name(path, buf, buffer_size)){
    free(buf);
    return 0;
  }

  size_t L = 0;
  char* pos = strrchr(buf, '.');
  if (pos && pos != buf)
    L = fs_strncpy(pos, result, buffer_size);

  free(buf);
  return L;
}


size_t fs_with_suffix(const char* path, const char* suffix,
                      char* result, const size_t buffer_size)
{
  if(strlen(suffix) == 0)
    return fs_stem(path, result, buffer_size);

  if(path[0] == '.'){
    // workaround for leading dot filename
    const int L = snprintf(result, buffer_size, "%s%s", path, suffix);
    if (L >= (int) buffer_size){
      fprintf(stderr, "ERROR:ffilesystem:fs_with_suffix: buffer_size too small for string\n");
      return 0;
    }

    return (size_t) L;
  }

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  return cwk_path_change_extension(path, suffix, result, buffer_size);
}


size_t fs_root(const char* path, char* result, const size_t buffer_size)
{
  // assumes a POSIX path separated input i.e. '/'

  if(buffer_size < 1)
    return 0;

  result[0] = '\0';
  if(!fs_is_absolute(path))
    return 0;
  // empty path is not absolute

  int N;
  if(fs_is_windows())
    N = snprintf(result, buffer_size, "%c:/", path[0]);
  else
    N = snprintf(result, buffer_size, "/");

  if (N < 0 || N >= (int) buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_root: buffer overflow\n");
    return 0;
  }

  return (size_t) N;
}


bool fs_is_absolute(const char* path)
{
  const size_t L = strlen(path);

  if(fs_is_windows())
    return L > 2 && isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\');
  else
    return L > 0 && path[0] == '/';
}


size_t fs_relative_to(const char* base, const char* other, char* result, const size_t buffer_size)
{
  // need this or separators are not handled correctly
  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  const size_t L = cwk_path_get_relative(base, other, result, buffer_size);

  if(L)
    fs_as_posix(result);

  return L;
}


size_t fs_proximate_to(const char* base, const char* other, char* result, const size_t buffer_size)
{
  const size_t L = fs_relative_to(base, other, result, buffer_size);
  if(L)
    return L;

  return fs_strncpy(other, result, buffer_size);
}
