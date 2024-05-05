#include "ffilesystem.h"

#include <string>


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
