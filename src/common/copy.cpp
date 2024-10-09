#include "ffilesystem.h"

#include <iostream>
#include <system_error>


bool Ffs::copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
  auto opt = std::filesystem::copy_options::none;
  if (overwrite)
    opt = std::filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs_is_file(dest) && !fs_remove(dest)) FFS_UNLIKELY
    std::cerr << "ERROR:Ffs::copy_file: could not remove existing destination file: " << dest << "\n";

  std::error_code ec;
  if(std::filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:copy_file: " << ec.message() << "\n";
  return false;
}
