#include "ffilesystem.h"

#include <string_view>
#include <system_error>

// inquire.cpp
// ------------

#ifndef HAVE_CXX_FILESYSTEM
#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <io.h> // _access_s
#else
#include <unistd.h>
#endif
#endif


bool fs_exists(std::string_view path)
{
  // fs_exists() is true even if path is non-readable
  // this is like Python pathlib.Path.exists()
  // unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  return std::filesystem::exists(path, ec) && !ec;
#else

#ifdef _MSC_VER
  // kwSys:SystemTools:FileExists is much more elaborate with Reparse point checks etc.
  // This way seems to work fine on Windows anyway.
  // io.h
  return !_access_s(path.data(), 0);
#else
  // unistd.h
  return !access(path.data(), F_OK);
#endif

#endif
}
