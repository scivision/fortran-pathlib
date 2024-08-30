#include "ffilesystem.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <cstring> // std::streerror
#include <system_error>  // for error_code
#include <cstdint> // for uintmax_t


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
// NOTE: Windows top-level drive "C:" needs a trailing slash "C:/"
  std::error_code ec;
  return std::filesystem::is_directory(path, ec) && !ec;
}


bool Ffs::is_exe(std::string_view path)
{
  std::error_code ec;

  const auto s = std::filesystem::status(path, ec);
  // need reserved check for Windows
  if(ec || !std::filesystem::is_regular_file(s) || Ffs::is_reserved(path))
    return false;

  // Windows MinGW bug with executable bit
  if(fs_is_mingw())
    return Ffs::is_readable(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  return (s.permissions() & (owner_exec | group_exec | others_exec)) != none;
}


bool Ffs::is_readable(std::string_view path)
{
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
#endif

  return (s.permissions() & (owner_read | group_read | others_read)) != none;
}


bool Ffs::is_writable(std::string_view path)
{
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
#endif

  return (s.permissions() & (owner_write | group_write | others_write)) != none;
}


bool Ffs::is_file(std::string_view path)
{
  std::error_code ec;
  // disqualify reserved names
  return std::filesystem::is_regular_file(path, ec) && !ec && !Ffs::is_reserved(path);
}


std::optional<uintmax_t> Ffs::file_size(std::string_view path)
{
  std::error_code ec;
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;

  std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
  return {};
}


std::optional<std::string> Ffs::get_permissions(std::string_view path)
{

  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s)) FFS_UNLIKELY
    return {};

  const std::filesystem::perms p = s.permissions();

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
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
