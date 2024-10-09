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
    return fs_is_readable(path);

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
