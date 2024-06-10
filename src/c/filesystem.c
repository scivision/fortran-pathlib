#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#include <string.h> // strerror
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
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
  const bool ok = fs_is_dir(path) ? RemoveDirectoryA(path) : DeleteFileA(path);
  if (!ok)
    fs_win32_print_error(path, "remove");

  return ok;
#else
  if(remove(path) == 0)
    return true;

  fprintf(stderr, "ERROR:Ffilesystem:remove: %s => %s\n", path, strerror(errno));
  return false;
#endif
}


bool fs_set_permissions(const char* path, int readable, int writable, int executable)
{
  // on POSIX, only sets permission for user, not group or others

  struct stat s;
  if(stat(path, &s) || (s.st_mode & S_IFCHR))
    return false;
  // special POSIX file character device like /dev/null

#ifdef _MSC_VER
  int m = s.st_mode;
  const int r = _S_IREAD;
  const int w = _S_IWRITE;
  const int x = _S_IEXEC;
#else
  mode_t m = s.st_mode;
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
