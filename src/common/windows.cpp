#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <system_error>
#include <cerrno>
#include <algorithm> // std::replace

#include "ffilesystem.h"


std::string fs_as_posix(std::string_view path)
{
  std::string s(path);

  if(fs_is_windows())
    std::replace( s.begin(), s.end(), '\\', '/');

  return s;
}


void fs_print_error(std::string_view path, std::string_view fname)
{

  std::cerr << "ERROR:ffilesystem:" << fname <<"(" << path << ")  ";
#if defined(_WIN32) || defined(__CYGWIN__)
  const DWORD error = GetLastError();

  std::cerr << std::system_category().message(GetLastError()) << "\n";

  if(error == ERROR_PRIVILEGE_NOT_HELD)
    std::cerr << "Enable Windows developer mode to use symbolic links: https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging \n";
#else
  auto econd = std::generic_category().default_error_condition(errno);
  std::cerr << econd.message() << "\n";
#endif
}


bool fs_win32_create_symlink(std::string_view target, std::string_view link)
{

#if defined(_WIN32) || defined(__CYGWIN__)
  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target.data()))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if(CreateSymbolicLinkA(link.data(), target.data(), p))
    return true;
#endif

  fs_print_error(std::string(target) + " => " + std::string(link), "create_symlink");
  return false;
}


std::string fs_longname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist
#ifdef _WIN32
  std::string out(fs_get_max_path(), '\0');
// size does not include null terminator

  if(auto L = GetLongPathNameA(in.data(), out.data(), (DWORD) out.size());
      L > 0 && L < out.length()){
    out.resize(L);
    return out;
  }

  fs_print_error(in, "longname");
  return {};
#else
  return std::string(in);
#endif
}


std::string fs_shortname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist
#ifdef _WIN32
  std::string out(fs_get_max_path(), '\0');
// size does not include null terminator
  if(auto L = GetShortPathNameA(in.data(), out.data(), (DWORD) out.size());
      L > 0 && L < out.length()){
    out.resize(L);
    return out;
  }

  fs_print_error(in, "shortname");
  return {};
#else
  return std::string(in);
#endif
}
