#include "ffilesystem.h"

#include <set>
#include <iostream>

#if __has_include(<ranges>)
#include <ranges>
#endif

#ifdef HAVE_CLOCK_CAST
// this include can break even when functions not used if C++ ABI problem e.g. Clang 14 with GCC 13
#include <chrono>
#else
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <filesystem>


bool fs_exists(const char* path)
{
  return Ffs::exists(std::string_view(path));
}

bool Ffs::exists(std::string_view path)
{
  // fs
  std::error_code ec;
  auto s = std::filesystem::status(path, ec);

  return !ec && std::filesystem::exists(s);
}


bool fs_is_char_device(const char* path)
{
  // special POSIX file character device like /dev/null
  return Ffs::is_char_device(std::string_view(path));
}

bool Ffs::is_char_device(std::string_view path)
{
  std::error_code ec;
  auto s = std::filesystem::status(path, ec);
  return !ec && std::filesystem::is_character_file(s);
}


bool fs_is_dir(const char* path)
{
  return Ffs::is_dir(std::string_view(path));
}

bool Ffs::is_dir(std::string_view path)
{
  std::filesystem::path p(path);

  if (fs_is_windows() && !path.empty() && p.root_name() == p)
    return true;

  std::error_code ec;
  auto s = std::filesystem::status(p, ec);
  return !ec && std::filesystem::is_directory(s);
}


bool fs_is_exe(const char* path)
{
  return Ffs::is_exe(std::string_view(path));
}

bool Ffs::is_exe(std::string_view path)
{
  std::error_code ec;

  auto s = std::filesystem::status(path, ec);
  // need reserved check for Windows
  if(ec || !std::filesystem::is_regular_file(s) || Ffs::is_reserved(path))
    return false;

if(fs_is_mingw()){
  // Windows MinGW bug with executable bit
  return Ffs::is_readable(path);
}

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms none = std::filesystem::perms::none;
  std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  auto i = s.permissions() & (owner_exec | group_exec | others_exec);
  return i != none;
}


bool fs_is_readable(const char* path)
{
  return Ffs::is_readable(std::string_view(path));
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

  auto i = s.permissions() & (owner_read | group_read | others_read);
  return i != none;
}


bool fs_is_writable(const char* path)
{
  return Ffs::is_writable(std::string_view(path));
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

  auto i = s.permissions() & (owner_write | group_write | others_write);
  return i != none;
}


bool fs_is_file(const char* path)
{
  return Ffs::is_file(std::string_view(path));
}

bool Ffs::is_file(std::string_view path)
{
  std::error_code ec;
  auto s = std::filesystem::status(path, ec);

  // disqualify reserved names
  return !ec && (std::filesystem::is_regular_file(s) && !Ffs::is_reserved(path));
}


bool fs_is_reserved(const char* path)
{
  return Ffs::is_reserved(std::string_view(path));
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

  std::time_t t_int = 0;

#ifdef HAVE_CLOCK_CAST
  auto t_fs = Ffs::get_modtime(std::string_view(path));
  auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
  t_int = std::chrono::system_clock::to_time_t(t_sys);
#else
  struct stat s;
  if (!stat(path, &s))
    return s.st_mtime;
#endif

  return t_int;
}

std::filesystem::file_time_type Ffs::get_modtime(std::string_view path)
{
  std::error_code ec;

  std::filesystem::file_time_type t_fs = std::filesystem::last_write_time(path, ec);
  if(ec) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:get_modtime: " << ec.message() << "\n";
    return {};
  }

  return t_fs;
}


uintmax_t fs_file_size(const char* path)
{
  return Ffs::file_size(std::string_view(path));
}

uintmax_t Ffs::file_size(std::string_view path)
{
  std::error_code ec;
  auto s = std::filesystem::file_size(path, ec);
  if(ec) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
    return 0;
  }
  return s;
}


uintmax_t fs_space_available(const char* path)
{
  return Ffs::space_available(std::string_view(path));
}

uintmax_t Ffs::space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;
  auto s = std::filesystem::space(path, ec);
  if(ec) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:space_available: " << ec.message() << "\n";
    return 0;
  }
  return s.available;
}


size_t fs_get_permissions(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::get_permissions(std::string_view(path)), result, buffer_size);
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