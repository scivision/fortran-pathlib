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
#define HAVE_STATVFS
#include <sys/statvfs.h>
#endif
#endif
#endif

std::uintmax_t fs_space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(auto s = std::filesystem::space(path, ec); !ec)  FFS_LIKELY
    return s.available;
#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdiskfreespaceexa

  ULARGE_INTEGER b;

  if(GetDiskFreeSpaceExA(path.data(), &b, nullptr, nullptr) != 0)  FFS_LIKELY
    return b.QuadPart;
#elif defined(HAVE_STATVFS)
  // https://www.man7.org/linux/man-pages/man3/statvfs.3.html
  // https://unix.stackexchange.com/a/703650
  struct statvfs stat;
  if (!statvfs(path.data(), &stat))  FFS_LIKELY
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_bavail;
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, "space_available", ec);

  return {};
}


std::uintmax_t fs_space_capacity(std::string_view path)
{
  // filesystem space capacity for device holding path
  // total size of the filesystem, in bytes
  // This is the quantity available to the non-priviliged user,
  // not the total physical disk size.
#ifdef HAVE_CXX_FILESYSTEM

  std::error_code ec;
  if(auto s = std::filesystem::space(path, ec); !ec)  FFS_LIKELY
    return s.capacity;

  fs_print_error(path, "space_capacity", ec);

#else

  const std::string r = fs_root(fs_absolute(path, false));
  if (r.empty())
    return {};

#ifdef _WIN32
  ULARGE_INTEGER b;
  if(GetDiskFreeSpaceExA(r.data(), nullptr, &b, nullptr) != 0)
    return b.QuadPart;

  fs_print_error(path, "space_capacity:GetDiskSpaceEx");
#elif defined(HAVE_STATVFS)
  struct statvfs stat;
  if (!statvfs(r.data(), &stat))
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_blocks;

  fs_print_error(path, "space_capacity:statvfs");
#else
  fs_print_error(path, "space_capacity: function not implemented on this platform");
#endif
#endif

  return {};
}
