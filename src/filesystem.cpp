#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstdio> // remove()

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool fs_remove(std::string_view path)
{

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;
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
#endif

  fs_print_error(path, "remove", ec);
  return false;
}
