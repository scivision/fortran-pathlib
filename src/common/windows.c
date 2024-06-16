#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>

#include "ffilesystem.h"


void fs_win32_print_error(FFS_MUNUSED const char* path, FFS_MUNUSED const char* fname)
{
#ifdef _WIN32
  DWORD error = GetLastError();
  char *message;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&message, 0, NULL);
  fprintf(stderr, "ERROR:ffilesystem:%s %s => %s\n", fname, path, message);
#endif
}


size_t fs_longname(const char* in, char* out, const size_t buffer_size)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist
#ifdef _WIN32
// size includes null terminator
  DWORD Lb = (DWORD) buffer_size;
  DWORD L = GetLongPathNameA(in, out, Lb);
  if(L > 0)
    return L;

  fs_win32_print_error(in, "longname");
  return 0;
#else
  return fs_strncpy(in, out, buffer_size);
#endif
}


size_t fs_shortname(const char* in, char* out, const size_t buffer_size)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist
#ifdef _WIN32
// size includes null terminator
  DWORD Lb = (DWORD) buffer_size;
  DWORD L = GetShortPathNameA(in, out, Lb);
  if(L > 0)
    return L;

  fs_win32_print_error(in, "shortname");
  return 0;
#else
  return fs_strncpy(in, out, buffer_size);
#endif
}
