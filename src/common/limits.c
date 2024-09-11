#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetVolumeInformationA
#else
#include <limits.h>   // NAME_MAX
#include <unistd.h>  // pathconf
#endif

#if defined (__APPLE__)
#include <sys/syslimits.h>
#endif

#include <stddef.h> // size_t

#include "ffilesystem.h"


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


size_t fs_max_component(const char* path)
{
  // maximum length of each component of a path. That is, while the maximum
  // total length of a path may be thousands of character, each segment of the
  // path has a maximum length too--typically 255 characters.

#ifdef _WIN32
  DWORD lpMaximumComponentLength = 0;

  if(!GetVolumeInformationA(path, 0, 0, 0, &lpMaximumComponentLength, 0, 0, 0))
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
