#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <cstdlib> // for realpath, _fullpath
#include <string>


std::string fs_trim(std::string r)
{
  // trim trailing nulls
  if(const auto i = r.find('\0');
      i != std::string::npos)
    r.resize(i);

  return r;
}


std::string fs_realpath(std::string_view path)
{
  std::string r(fs_get_max_path(), '\0');


  [[maybe_unused]] char* t =
#ifdef _WIN32
  _fullpath(r.data(), path.data(), r.size());
#else
  realpath(path.data(), r.data());
#endif

  return fs_as_posix(fs_trim(r));
}
