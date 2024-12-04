#include "ffilesystem.h"

#include <cstdint> // uintmax_t
#include <system_error> // for error_code

#include <string_view>


#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetDiskFreeSpaceEx
#else
#include <unistd.h>
#if __has_include(<sys/statvfs.h>)
#include <sys/statvfs.h>
#endif
#endif
#endif

std::uintmax_t fs_space_available(std::string_view path)
{
  // filesystem space available for device holding path
#ifdef HAVE_CXX_FILESYSTEM

  std::error_code ec;
  if(auto s = std::filesystem::space(path, ec); !ec)  FFS_LIKELY
    return s.available;

  fs_print_error(path, "space_available", ec);

#else

  // Windows MinGW and oneAPI need root() or space is zero.
  // use root for Unix too for consistency e.g. macOS
  // need root(absolute()) to be consistent with <filesystem> backend on relative paths
  const std::string r = fs_root(fs_absolute(path, false));
  if (r.empty())
    return {};

#ifdef _WIN32
  ULARGE_INTEGER bytes_available;
  if(GetDiskFreeSpaceExA(r.data(), &bytes_available, nullptr, nullptr))
    return bytes_available.QuadPart;

  fs_print_error(path, "space_available:GetDiskSpaceEx");
#elif __has_include(<sys/statvfs.h>)
  // https://unix.stackexchange.com/a/703650
  struct statvfs stat;
  if (!statvfs(r.data(), &stat))
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_bavail;

  fs_print_error(path, "space_available:statvfs");
#else
  fs_print_error(path, "space_available: function not implemented on this platform");
#endif
#endif

  return {};
}
