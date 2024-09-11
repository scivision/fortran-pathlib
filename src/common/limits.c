#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetVolumeInformationA
#else
#include <limits.h>   // NAME_MAX
#include <unistd.h>  // pathconf
#endif

#include <stddef.h> // size_t

#include "ffilesystem.h"


size_t fs_max_component(const char* path)
{
  // maximum length of each component of a path. That is, while the maximum
  // total length of a path may be thousands of character, each segment of the
  // path has a maximum length too--typically 255 characters.

#ifdef _WIN32
  DWORD lpMaximumComponentLength = 0;

  if(!GetVolumeInformationA(drive.c_str(), 0, 0, 0, &lpMaximumComponentLength, 0, 0, 0))
    fs_win32_print_error(path, "max_name");

  return (size_t) lpMaximumComponentLength;

#elif defined(_PC_NAME_MAX)
  return (size_t) pathconf(path, _PC_NAME_MAX);
#elif defined(NAME_MAX)
  return (size_t) NAME_MAX;
#else
  fprintf(stderr, "ERROR:ffilesystem:max_component(%s) => function not implemented on this platform\n", path);
  return 0;
#endif
}
