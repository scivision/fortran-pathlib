#include "ffilesystem.h"

#include <string>
#include <iostream>

#include <optional>
#include <system_error> // for std::error_code


std::optional<std::string> Ffs::get_tempdir()
{
  std::error_code ec;
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();

  std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
  return {};
}


std::optional<std::string> Ffs::get_cwd()
{
  std::error_code ec;
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();

  std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
  return {};
}


bool Ffs::chdir(std::string_view path)
{
  std::error_code ec;
  if(std::filesystem::current_path(path, ec); !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:chdir: " << ec.message() << "\n";
  return false;
}
