// functions from C++ filesystem

// NOTE: this segfaults: std::filesystem::path p(nullptr);


#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstdio> // remove()

#include <iostream>
#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool fs_remove(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;

  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:FFs:remove: " << ec.message() << "\n";
  return false;
#else

#ifdef _WIN32
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
  if(fs_is_dir(path) ? RemoveDirectoryA(path.data()) : DeleteFileA(path.data()))
    return true;
#else
  if(remove(path.data()) == 0)
    return true;
#endif
   fs_print_error(path, "remove");
   return false;

#endif
}
