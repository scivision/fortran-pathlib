#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdbool.h>
#include <stdio.h>

#include "ffilesystem.h"



#if defined(_WIN32) || defined(__CYGWIN__)
void fs_win32_print_error(const char* path, const char* fname)
{
  const DWORD error = GetLastError();

  char *message;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&message, 0, NULL);

  fprintf(stderr, "ERROR:ffilesystem:%s %s => %s\n", fname, path, message);

  if(error == ERROR_PRIVILEGE_NOT_HELD)
    fprintf(stderr, "Enable Windows developer mode to use symbolic links: https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging \n");

}


bool fs_win32_create_symlink(const char* target, const char* link)
{
// https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#iso.2017.specific
// 30.10.2.1 [fs.conform.9945] The behavior of the filesystem library implementation
// will depend on the target operating system. Some features will not be supported
// on some targets.
// Symbolic links and file permissions are not supported on Windows.

  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  const bool ok = CreateSymbolicLinkA(link, target, p);
  if(!ok)
    fs_win32_print_error(target, "create_symlink");

  return ok;
}
#endif


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
