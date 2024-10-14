#include "ffilesystem.h"

#include <string>


std::string Ffs::filesystem_type(std::string_view path)
{
  if(std::string name(80, '\0');
      fs_filesystem_type(path.data(), name.data(), name.size()))
    return name.c_str();

  return {};
}


bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  return fs_is_subdir(subdir.data(), dir.data());
}
