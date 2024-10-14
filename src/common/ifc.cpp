#include "ffilesystem.h"

#include <string>


bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  return fs_is_subdir(subdir.data(), dir.data());
}
