#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <iostream>
#include <optional>
#include <system_error>

#ifdef HAVE_CLOCK_CAST
// this include can break even when functions not used if C++ ABI problem e.g. Clang 14 with GCC 13
#include <chrono>
#endif
#include <ctime>                // for time_t

// preferred import order for stat() -- always include to avoid dev confusion
#include <sys/types.h>
#include <sys/stat.h>

#include <cstring> // std::strerror
#include <string_view>

#ifndef HAVE_CXX_FILESYSTEM
#if defined(_WIN32)
#include <sys/utime.h> // _utime
#else
#include <sys/time.h> // utimes
#endif
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif


std::time_t fs_get_modtime(const char* path)
{

#if defined(HAVE_CLOCK_CAST) && defined(HAVE_CXX_FILESYSTEM)
  if(const auto &t_fs = fs_get_modtime_fs(path); t_fs){
    const auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs.value());
    return std::chrono::system_clock::to_time_t(t_sys);
  }
#elif defined(STATX_MTIME) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (FS_TRACE) std::cout << "TRACE: statx() get_modtime " << path << "\n";
  struct statx s;

  if( statx(AT_FDCWD, path, AT_NO_AUTOMOUNT, STATX_MTIME, &s) == 0 ) FFS_LIKELY
    return s.stx_mtime.tv_sec;
#else
  if (struct stat s; !stat(path, &s))
    return s.st_mtime;
#endif

  fs_print_error(path, "fs_get_modtime");
  return {};
}

#ifdef HAVE_CXX_FILESYSTEM
std::optional<std::filesystem::file_time_type> fs_get_modtime_fs(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::file_time_type t_fs = std::filesystem::last_write_time(path, ec); !ec) FFS_LIKELY
    return t_fs;

  fs_print_error(path, "fs_get_modtime_fs", ec);
  return {};
}
#endif


bool fs_set_modtime(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM

  std::error_code ec;

  std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
  if(!ec) FFS_LIKELY
    return true;
  // techinically IWYU <chrono> but that can break some compilers, and it works without the include.

  fs_print_error(path, "fs_set_modtime", ec);
  return false;

#else

  if (
#ifdef _WIN32
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64
    _utime(path.data(), nullptr)
#else
    utimes(path.data(), nullptr)
#endif
    ){
      fs_print_error(path, "fs_set_modtime");
      return false;
    }

  return true;

#endif
}
