#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cstdlib> // for realpath
#endif
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
  // resolve real path, which need not exist

#ifdef _WIN32
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnameA
  const DWORD L = GetFullPathNameA(path.data(), 0, nullptr, nullptr);
  if(L == 0){  FFS_UNLIKELY
    fs_print_error(path, "realpath:GetFullPathName");
    return {};
  }
  // this form includes the null terminator
  std::string r(L, '\0');
  // weak detection of race condition (cwd change)
  if(GetFullPathNameA(path.data(), L, r.data(), nullptr) == L-1){  FFS_LIKELY
    r.resize(L-1);
    return fs_as_posix(r);
  }

  fs_print_error(path, "realpath:GetFullPathName");
  return {};
#else
  std::string r(fs_get_max_path(), '\0');
  const char* t = realpath(path.data(), r.data());
  if(!t && fs_exists(path))  FFS_UNLIKELY
    fs_print_error(path, "realpath");

  return fs_trim(r);
#endif

}
