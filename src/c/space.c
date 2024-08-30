#include "ffilesystem.h"

#include <stdint.h>  // uintmax_t
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetDiskFreeSpaceEx
#else
#include <unistd.h>
#if __has_include(<sys/statvfs.h>)
#include <sys/statvfs.h>
#endif
#endif

uintmax_t
fs_space_available(const char* path)
{

#ifdef _WIN32
  // Windows MinGW and oneAPI need root() or space is zero.
  char r[4];
  if(!fs_root(path, r, 4)){
    fprintf(stderr, "ERROR:ffilesystem:space_available: %s => root() failed\n", path);
    return 0;
  }
  ULARGE_INTEGER bytes_available;
  const BOOL ok = GetDiskFreeSpaceExA(r, &bytes_available, NULL, NULL);
  if(ok)
    return bytes_available.QuadPart;

  fs_win32_print_error(path, "space_available");
#elif __has_include(<sys/statvfs.h>)
  // https://unix.stackexchange.com/a/703650
  struct statvfs stat;
  if (!statvfs(path, &stat))
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_bavail;

  fprintf(stderr, "ERROR:ffilesystem:space_available(%s) => %s\n", path, strerror(errno));
#else
  fprintf(stderr, "ERROR:ffilesystem:space_available(%s) => function not implemented on this platform\n", path);
#endif

  return 0;
}
