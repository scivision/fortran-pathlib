#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <stdbool.h>
#include <stdint.h>  // uintmax_t
#include <string.h>

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <io.h> // _access_s
#else
#include <unistd.h>
#endif

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <stdio.h>


bool
fs_is_exe(const char* path)
{
  return (fs_is_file(path) &&
#ifdef _WIN32
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  // in Windows, all readable files are executable. Do not use _S_IEXEC, it is not reliable.
  fs_is_readable(path)
#else
  !access(path, X_OK)
#endif
  );
}


bool
fs_is_readable(const char* path)
{
/* directory or file readable */

#ifdef _MSC_VER
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return !_access_s(path, 4);
#else
  return !access(path, R_OK);
#endif
}


bool
fs_is_writable(const char* path)
{
/* directory or file writable */

#ifdef _MSC_VER
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return !_access_s(path, 2);
#else
  return !access(path, W_OK);
#endif
}


uintmax_t
fs_file_size(const char* path)
{
  struct stat s;
  if (!stat(path, &s))
    return s.st_size;

  fprintf(stderr, "ERROR:ffilesystem:file_size: %s => %s\n", path, strerror(errno));
  return 0;
}
