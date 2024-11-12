#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if defined(HAVE_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#elif !defined(_WIN32)
#include <cstdio> // remove()
#endif

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool fs_remove(std::string_view path)
{

  Fserr::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)
  if(Fs::remove(path, ec) && !ec) FFS_LIKELY
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
