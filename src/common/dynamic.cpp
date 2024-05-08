#include "ffilesystem.h"

#include <string>


std::string Ffs::exe_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_exe_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::lib_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_lib_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}
