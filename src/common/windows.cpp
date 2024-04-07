#include "ffilesystem.h"

#include <memory>
#include <string>


std::string Ffs::shortname(std::string_view in)
{
  const size_t MP = fs_get_max_path();

  if(auto buf = std::make_unique<char[]>(MP); fs_shortname(in.data(), buf.get(), MP)) FFS_LIKELY
    return std::string(buf.get());

  return {};
}


std::string Ffs::longname(std::string_view in)
{
  const size_t MP = fs_get_max_path();

  if(auto buf = std::make_unique<char[]>(MP); fs_longname(in.data(), buf.get(), MP)) FFS_LIKELY
    return std::string(buf.get());

  return {};
}
