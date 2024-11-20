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

#include <cstddef> // IWYU pragma: keep
#include <string>
#include <string_view>
#include <iostream> // IWYU pragma: keep

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
  return (m < 4096) ? m : 4096;
  // arbitrary absolute maximum
  // Ref: https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/include/ghc/filesystem.hpp#L244

}


std::string::size_type fs_max_component([[maybe_unused]] std::string_view path)
{
  // maximum length of each component of a path. That is, while the maximum
  // total length of a path may be thousands of character, each segment of the
  // path has a maximum length too--typically 255 characters.

#ifdef _WIN32

  if(DWORD L = 0;
      GetVolumeInformationA(fs_root(path).data(), nullptr, 0, nullptr, &L, nullptr, nullptr, 0) != 0)  FFS_LIKELY
    return L;

  fs_print_error(path, "max_component:GetVolumeInformationA");
  return {};
#elif defined(_PC_NAME_MAX)
  return pathconf(path.data(), _PC_NAME_MAX);
#elif defined(NAME_MAX)
  return NAME_MAX;
#else
  std::cerr << "ERROR:ffilesystem:max_component(" << path << ") => function not implemented on this platform\n";
  return {};
#endif
}
