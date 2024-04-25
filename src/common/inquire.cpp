#include "ffilesystem.h"

#include <set>
#include <iostream>
#include <string>
#include <cstring> // std::streerror

#if __has_include(<ranges>)
#include <ranges>
#endif

// preferred import order for stat() -- always include to avoid dev confusion
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_CLOCK_CAST
// this include can break even when functions not used if C++ ABI problem e.g. Clang 14 with GCC 13
#include <chrono>
#endif

#include <cerrno>


bool Ffs::exists(std::string_view path)
{
  std::error_code ec;
  return std::filesystem::exists(path, ec) && !ec;
}


bool Ffs::is_char_device(std::string_view path)
{
  std::error_code ec;
  return std::filesystem::is_character_file(path, ec) && !ec;
}


bool Ffs::is_dir(std::string_view path)
{
  std::filesystem::path p(path);

  if (fs_is_windows() && !path.empty() && p.root_name() == p)
    return true;

  std::error_code ec;
  return std::filesystem::is_directory(p, ec) && !ec;
}


bool Ffs::is_exe(std::string_view path)
{
  std::error_code ec;

  auto s = std::filesystem::status(path, ec);
  // need reserved check for Windows
  if(ec || !std::filesystem::is_regular_file(s) || Ffs::is_reserved(path))
    return false;

  // Windows MinGW bug with executable bit
  if(fs_is_mingw())
    return Ffs::is_readable(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms none = std::filesystem::perms::none;
  std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  return (s.permissions() & (owner_exec | group_exec | others_exec)) != none;
}


bool Ffs::is_readable(std::string_view path)
{
  std::error_code ec;
  auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms none = std::filesystem::perms::none;
  std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  std::filesystem::perms group_read = std::filesystem::perms::group_read;
  std::filesystem::perms others_read = std::filesystem::perms::others_read;
#endif

  return (s.permissions() & (owner_read | group_read | others_read)) != none;
}


bool Ffs::is_writable(std::string_view path)
{
  std::error_code ec;
  auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  std::filesystem::perms group_write = std::filesystem::perms::group_write;
  std::filesystem::perms others_write = std::filesystem::perms::others_write;
  std::filesystem::perms none = std::filesystem::perms::none;
#endif

  return (s.permissions() & (owner_write | group_write | others_write)) != none;
}


bool Ffs::is_file(std::string_view path)
{
  std::error_code ec;
  // disqualify reserved names
  return std::filesystem::is_regular_file(path, ec) && !ec && !Ffs::is_reserved(path);
}


bool Ffs::is_reserved(std::string_view path)
// https://learn.microsoft.com/en-gb/windows/win32/fileio/naming-a-file#naming-conventions
{
  if (path.empty()) FFS_UNLIKELY
    return false;

#ifndef _WIN32
  return false;
#elif defined(__cpp_lib_ranges)

  std::set<std::string_view, std::less<>> reserved {
      "CON", "PRN", "AUX", "NUL",
      "COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
      "LPT0", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

  std::string s = Ffs::stem(path);

  std::ranges::transform(s.begin(), s.end(), s.begin(), ::toupper);

  return reserved.contains(s);

#else
  std::cerr << "ERROR:ffilesystem:is_reserved: C++20 required for reserved names check\n";
  return false;
#endif
}


std::time_t fs_get_modtime(const char* path)
{

#ifdef HAVE_CLOCK_CAST
  auto t_fs = Ffs::get_modtime(std::string_view(path));
  auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
  return std::chrono::system_clock::to_time_t(t_sys);
#else
  struct stat s;
  if (!stat(path, &s))
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


uintmax_t Ffs::file_size(std::string_view path)
{
  std::error_code ec;
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;

  std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
  return 0;
}


uintmax_t Ffs::space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;
  if(auto s = std::filesystem::space(path, ec); !ec)  FFS_LIKELY
    return s.available;

  std::cerr << "ERROR:ffilesystem:space_available: " << ec.message() << "\n";
  return 0;
}


std::string Ffs::get_permissions(std::string_view path)
{

  std::error_code ec;
  auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s)) FFS_UNLIKELY
    return {};

  std::filesystem::perms p = s.permissions();

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms none = std::filesystem::perms::none;
  std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
  std::filesystem::perms group_read = std::filesystem::perms::group_read;
  std::filesystem::perms group_write = std::filesystem::perms::group_write;
  std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  std::filesystem::perms others_read = std::filesystem::perms::others_read;
  std::filesystem::perms others_write = std::filesystem::perms::others_write;
  std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
#endif

  std::string r = "---------";
  if ((p & owner_read) != none)
    r[0] = 'r';
  if ((p & owner_write) != none)
    r[1] = 'w';
  if ((p & owner_exec) != none)
    r[2] = 'x';
  if ((p & group_read) != none)
    r[3] = 'r';
  if ((p & group_write) != none)
    r[4] = 'w';
  if ((p & group_exec) != none)
    r[5] = 'x';
  if ((p & others_read) != none)
    r[6] = 'r';
  if ((p & others_write) != none)
    r[7] = 'w';
  if ((p & others_exec) != none)
    r[8] = 'x';

  return r;
}
