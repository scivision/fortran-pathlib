#include "ffilesystem.h"

#include <vector>
#include <string>


std::string Ffs::shortname(std::string_view in)
{

  if(std::vector<char> buf(fs_get_max_path());
      fs_shortname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return std::string(buf.data());

  return {};
}


std::string Ffs::longname(std::string_view in)
{
  if(std::vector<char> buf(fs_get_max_path());
      fs_longname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return std::string(buf.data());

  return {};
}
