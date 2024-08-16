#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <iostream>
#include <system_error>

#ifdef HAVE_CLOCK_CAST
// this include can break even when functions not used if C++ ABI problem e.g. Clang 14 with GCC 13
#include <chrono>
#endif
#include <ctime>                // for time_t

// preferred import order for stat() -- always include to avoid dev confusion
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>

#include <cstring> // std::strerror
#include <string_view>


std::time_t fs_get_modtime(const char* path)
{

#ifdef HAVE_CLOCK_CAST
  const auto t_fs = Ffs::get_modtime(path);
  const auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
  return std::chrono::system_clock::to_time_t(t_sys);
#else
  if (struct stat s; !stat(path, &s))
    return s.st_mtime;

  std::cerr << "ERROR:Ffs:fs_get_modtime: " << std::strerror(errno) << "\n";
  return {};
#endif

}

std::filesystem::file_time_type Ffs::get_modtime(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::file_time_type t_fs = std::filesystem::last_write_time(path, ec); !ec) FFS_LIKELY
    return t_fs;

  std::cerr << "ERROR:Ffs:get_modtime: " << ec.message() << "\n";
  return std::filesystem::file_time_type::min();
}


bool Ffs::set_modtime(std::string_view path)
{
  std::error_code ec;

  std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
  if(!ec) FFS_LIKELY
    return true;
  // techinically IWYU <chrono> but that can break some compilers, and it works without the include.

  std::cerr << "ERROR:ffilesystem:set_modtime: " << ec.message() << "\n";
  return false;
}
