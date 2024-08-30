#include "ffilesystem.h"

#include <string_view>
#include <iostream>
#include <system_error>         // for error_code


bool Ffs::mkdir(std::string_view path)
{
  std::error_code ec;
  // old MacOS return false even if directory was created
  if (std::filesystem::create_directories(path, ec) || (!ec && Ffs::is_dir(path))) FFS_LIKELY
    return true;

  std::cerr << "ERROR:Ffs:mkdir(" << path << ") " << ec.message() << "\n";
  return false;
}
