#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#else
#include <cstdio> // for std::remove, std::rename
#endif

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool
fs_remove(std::string_view path)
{
  // remove a file or empty directory
  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  // https://en.cppreference.com/w/cpp/filesystem/remove
  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
#else
  // https://en.cppreference.com/w/cpp/io/c/remove
  if(std::remove(path.data()) == 0)
#endif
    return true;

  fs_print_error(path, "remove", ec);
  return false;
}


bool
fs_rename(std::string_view from, std::string_view to)
{
  // rename a file or directory
  // existing non-empty "to" is an error

  std::error_code ec;

  if(fs_exists(to) && !fs_is_empty(to)){
    fs_print_error(from, to, "rename: destination is not empty", ec);
    return false;
  }

#ifdef HAVE_CXX_FILESYSTEM
  // https://en.cppreference.com/w/cpp/filesystem/rename
  std::filesystem::rename(from, to, ec);
  if(!ec) FFS_LIKELY
#else
  // https://en.cppreference.com/w/cpp/io/c/rename
  if(std::rename(from.data(), to.data()) == 0)
#endif
    return true;

  fs_print_error(from, to, "rename", ec);
  return false;

}
