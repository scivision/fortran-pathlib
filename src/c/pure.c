#include "ffilesystem.h"
#include <cwalk.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h> // isalnum


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


size_t fs_strncpy(const char* path, char* result, size_t buffer_size)
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


void fs_as_posix(char* path)
{
// force posix file seperator on Windows
  if(!fs_is_windows())
    return;

  char s = '\\';
  char *p = strchr(path, s);
  while (p) {
    *p = '/';
    p = strchr(p+1, s);
  }
}


size_t fs_normal(const char* path, char* result, size_t buffer_size)
{
// normalize path
  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  size_t L = cwk_path_normalize(path, result, buffer_size);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:normal: output buffer %zu too small\n", buffer_size);
    return 0;
  }

  if(L)
    fs_as_posix(result);

  return L;
}


size_t fs_file_name(const char* path, char* result, size_t buffer_size)
{
  size_t L = strlen(path);
  if(L == 0)
    return 0;

  // same as C++17 std::filesystem::path::filename()
  if (path[L-1] == '/' || (fs_is_windows() && path[L-1] == '\\'))
    return 0;

  const char *base;

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  cwk_path_get_basename(path, &base, &L);

  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_file_name: buffer_size %zu too small\n", buffer_size);
    return 0;
  }

  strncpy(result, base, buffer_size);
  return L;
}


size_t fs_stem(const char* path, char* result, size_t buffer_size)
{
  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  if(!fs_file_name(path, buf, buffer_size)){
    free(buf);
    return 0;
  }

  // no need to recheck buffer_size as it's already checked in fs_file_name()

  char* pos = strrchr(buf, '.');
  if (pos && pos != buf){
    strncpy(result, buf, pos-buf);
    result[pos-buf] = '\0';  // need this to truncate suffix
  } else
    strncpy(result, buf, buffer_size);

  free(buf);
  return strlen(result);
}


size_t fs_join(const char* path, const char* other, char* result, size_t buffer_size)
{
  size_t L1 = strlen(path);
  size_t L2 = strlen(other);

  if(L1 == 0 && L2 == 0)
    return 0;

  if(L1 == 0)
    return fs_strncpy(other, result, buffer_size);

  if(L2 == 0)
    return fs_strncpy(path, result, buffer_size);

  cwk_path_set_style(CWK_STYLE_UNIX);

  return cwk_path_join(path, other, result, buffer_size);
}


size_t fs_parent(const char* path, char* result, size_t buffer_size)
{

  size_t L = strlen(path);
  if(L == 0 || buffer_size < 2)
    return 0;

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  cwk_path_get_dirname(path, &L);

  if (L >= buffer_size){
    fprintf(stderr, "ERROR:Ffilesystem:fs_parent: buffer_size %zu too small\n", buffer_size);
    return 0;
  }

  // handle "/" and other no parent cases
  if(L == 0){
    if (path[0] == '/' || (fs_is_windows() && path[0] == '\\'))
      result[0] = '/';
    else if (fs_is_windows() && isalpha(path[0]) && path[1] == ':')
      return fs_root(path, result, buffer_size);
    else
      result[0] = '.';

    result[1] = '\0';
    return 1;
  }

  // drop trailing slashes
  while(L > 1 && (path[L-1] == '/' || (fs_is_windows() && path[L-1] == '\\')))
    L--;

  if(FS_TRACE) printf("TRACE: parent %s L=%zu\n", path, L);

  strncpy(result, path, L);

  fs_as_posix(result);

  return L;
}


size_t fs_suffix(const char* path, char* result, size_t buffer_size)
{
  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  if(!fs_file_name(path, buf, buffer_size)){
    free(buf);
    return 0;
  }

  char* pos = strrchr(buf, '.');
  if (pos && pos != buf)
    strncpy(result, pos, buffer_size);
  else
    result[0] = '\0';

  free(buf);

  return strlen(result);
}


size_t fs_with_suffix(const char* path, const char* suffix,
                      char* result, size_t buffer_size)
{
  if(strlen(suffix) == 0)
    return fs_stem(path, result, buffer_size);

  if(path[0] == '.'){
    size_t L = strlen(path) + strlen(suffix);
    if (L >= buffer_size){
      fprintf(stderr, "ERROR:ffilesystem:fs_with_suffix: buffer_size too small for string\n");
      return 0;
    }
    // workaround for leading dot filename
    strncpy(result, path, buffer_size);
    strncat(result, suffix, buffer_size);
    return L;
  }

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  return cwk_path_change_extension(path, suffix, result, buffer_size);
}


size_t fs_root(const char* path, char* result, size_t buffer_size)
{
  size_t L;

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  cwk_path_get_root(path, &L);

  if(L >= buffer_size){
     fprintf(stderr, "ERROR:ffilesystem:fs_root: buffer_size %zu too small\n", buffer_size);
     return 0;
  }

  strncpy(result, path, L);

  return L;
}


bool fs_is_absolute(const char* path)
{
  if(fs_is_windows() && path[0] == '/')
    return false;

  cwk_path_set_style(fs_is_windows() ? CWK_STYLE_WINDOWS : CWK_STYLE_UNIX);

  return cwk_path_is_absolute(path);
}


bool fs_is_safe_name(const char* filename)
{

size_t L = strlen(filename);

if(L == 0)
  return false;

if(fs_is_windows() && filename[L-1] == '.')
  return false;

for (size_t i = 0; i < L; i++) {
  if(isalnum(filename[i]))
    continue;

  switch (filename[i]) {
    case '_': case '-': case '.': case '~': case '@': case '#': case '$': case '%': case '^': case '&':
    case '(': case ')': case '[': case ']': case '{': case '}': case '+': case '=': case ',': case '!':
      continue;
    default:
      return false;
  }
}

return true;

}
