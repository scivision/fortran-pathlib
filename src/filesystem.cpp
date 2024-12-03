#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#elif !defined(_WIN32)
#include <cstdio> // remove()
#endif

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


#if defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM)

static bool
fs_win32_remove(std::string_view path)
{

  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea

  if(fs_is_dir(path)){
    if(RemoveDirectoryA(path.data()))
      return true;
    fs_print_error(path, "remove: directory faileed");
    return false;
  } else if(fs_is_file(path)){
    if(DeleteFileA(path.data()))
      return true;
    fs_print_error(path, "remove: file failed");
    return false;
  } else {
    fs_print_error(path, "remove: is not a file or directory");
    return false;
  }

  fs_print_error(path, "remove");
  return false;
}

#endif


bool
fs_remove(std::string_view path)
{
  // remove a file or empty directory
  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM

  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;

#elif defined(_WIN32)
  return fs_win32_remove(path);
#else
  if(std::remove(path.data()) == 0)
    return true;
#endif

  fs_print_error(path, "remove", ec);
  return false;
}
