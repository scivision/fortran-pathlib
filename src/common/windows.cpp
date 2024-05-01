#include "ffilesystem.h"

#include <memory>
#include <string>


std::string Ffs::shortname(std::string_view in)
{

  [[maybe_unused]] const size_t MP = fs_get_max_path();

#ifndef __cpp_lib_make_unique
  std::cerr << "compiler lacks C++ 14 make_unique: Ffs::shortname\n";
  return {};
#else
  if(auto buf = std::make_unique<char[]>(MP); fs_shortname(in.data(), buf.get(), MP)) FFS_LIKELY
    return std::string(buf.get());
#endif

  return {};
}


std::string Ffs::longname(std::string_view in)
{
  [[maybe_unused]] const size_t MP = fs_get_max_path();

#ifndef __cpp_lib_make_unique
  std::cerr << "compiler lacks C++ 14 make_unique: Ffs::longname\n";
  return {};
#else
  if(auto buf = std::make_unique<char[]>(MP); fs_longname(in.data(), buf.get(), MP)) FFS_LIKELY
    return std::string(buf.get());
#endif

  return {};
}
