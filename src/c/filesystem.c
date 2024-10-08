#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#include <string.h> // strerror
#include <stdbool.h>
#include <stdio.h>  // remove()

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>  // chmod, stat
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ffilesystem.h"


bool fs_remove(const char* path)
{

#ifdef _WIN32
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
  if(fs_is_dir(path) ? RemoveDirectoryA(path) : DeleteFileA(path))
    return true;
#else
  if(remove(path) == 0)
    return true;
#endif
   fs_print_error(path, "remove");
   return false;
}


bool fs_set_permissions(const char* path, int readable, int writable, int executable)
{
  // on POSIX, only sets permission for user, not group or others

#ifdef _MSC_VER
  int m = fs_st_mode(path);
  const int r = _S_IREAD;
  const int w = _S_IWRITE;
  const int x = _S_IEXEC;
#else
  mode_t m = fs_st_mode(path);
  const mode_t r = S_IRUSR;
  const mode_t w = S_IWUSR;
  const mode_t x = S_IXUSR;
#endif

  if(readable > 0)
    m |= r;
  else if (readable < 0)
    m &= ~r;

  if(writable > 0)
    m |= w;
  else if (writable < 0)
    m &= ~w;

  if(executable > 0)
    m |= x;
  else if (executable < 0)
    m &= ~x;

  return chmod(path, m) == 0;
}
