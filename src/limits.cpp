#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetVolumeInformationA
#else
#include <limits.h>   // NAME_MAX
#include <unistd.h>  // pathconf
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <sys/syslimits.h>
#endif

#include <cerrno>
#include <cstddef> // IWYU pragma: keep
#include <string>
#include <string_view>

#include "ffilesystem.h"


constexpr size_t DEFAULT_MAX_PATH = 256;

size_t
fs_get_max_path()
{
// Returns the maximum path length supported by the file system.
  size_t m = DEFAULT_MAX_PATH;
#if defined(PATH_MAX)
  m = PATH_MAX;
#elif defined (_MAX_PATH)
  m = _MAX_PATH;
#elif defined (_POSIX_PATH_MAX)
  m = _POSIX_PATH_MAX;
#endif
  // arbitrary absolute maximum
  // Ref: https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/include/ghc/filesystem.hpp#L244
  return (m < 4096) ? m : 4096;
}

// This function returns the maximum length of a single component in the given path.
std::string::size_type fs_max_component(
#if defined(__has_cpp_attribute) && __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
std::string_view path)
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
  errno = 0;
  auto const r = pathconf(path.data(), _PC_NAME_MAX);
  if(r != -1)
    return r;
  if(errno == 0)
    return DEFAULT_MAX_PATH;

  fs_print_error("max_component:pathconf", path);
  return {};
#elif defined(NAME_MAX)
  return NAME_MAX;
#else
  fs_print_error("max_component:function not implemented on this platform", path);
  return {};
#endif
}
