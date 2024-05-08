#include "ffilesystem.h"

#include <string>


std::string Ffs::exe_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_exe_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}

std::string Ffs::filesystem_type(std::string_view path)
{
  if(std::string name(fs_get_max_path(), '\0');
      fs_filesystem_type(path.data(), name.data(), name.size()))
    return name.c_str();

  return {};
}

std::string Ffs::lib_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_lib_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}

std::string Ffs::shortname(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_shortname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::longname(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_longname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}
