#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#include <cstdlib> // for realpath
#endif

#include <string>
#include <string_view>


std::string fs_realpath(std::string_view path)
{
  // resolve real path
  // not well-defined for non-existing path--may return empty string.

#ifdef _WIN32
  return fs_exists(path) ? fs_win32_final_path(path) : fs_win32_full_name(path);
#else
  std::string r(fs_get_max_path(), '\0');
  const char* t = realpath(path.data(), r.data());
  if(!t && errno != ENOENT && errno != EACCES)  FFS_UNLIKELY
    fs_print_error(path, "realpath");

  return fs_trim(r);
#endif

}
