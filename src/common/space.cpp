#include "ffilesystem.h"

#include <optional>
#include <cstdlib> // uintmax_t
#include <system_error> // for error_code
#include <iostream>
#include <string_view>

std::optional<uintmax_t> Ffs::space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;
  if(auto s = std::filesystem::space(path, ec); !ec)  FFS_LIKELY
    return s.available;

  std::cerr << "ERROR:ffilesystem:space_available: " << ec.message() << "\n";
  return {};
}
