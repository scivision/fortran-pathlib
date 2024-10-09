#include "ffilesystem.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <cstring> // std::streerror
#include <system_error>  // for error_code
#include <cstdint> // for uintmax_t


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


std::optional<uintmax_t> Ffs::file_size(std::string_view path)
{
  std::error_code ec;
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;

  std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
  return {};
}
