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
#include <io.h> // chmod
#endif

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>  // chmod, stat

#include <cstdio> // remove()

#include <iostream>
#include <string>
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


bool fs_set_permissions(std::string_view path, int readable, int writable, int executable)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::filesystem::path pth(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  std::error_code ec;
  // need to error if path doesn't exist and no operations are requested
  if(!std::filesystem::exists(pth, ec))
    ec = std::make_error_code(std::errc::no_such_file_or_directory);

  if (readable != 0)
    std::filesystem::permissions(pth, owner_read,
      (readable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if (!ec && writable != 0)
    std::filesystem::permissions(pth, owner_write,
      (writable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if (!ec && executable != 0)
    std::filesystem::permissions(pth, owner_exec,
      (executable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if(!ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:set_permissions: " << ec.message() << "\n";
  return false;
#else

  // on POSIX, only sets permission for user, not group or others

  int m = fs_st_mode(path);
#ifdef _MSC_VER
  const int r = _S_IREAD;
  const int w = _S_IWRITE;
  const int x = _S_IEXEC;
#else
  const int r = S_IRUSR;
  const int w = S_IWUSR;
  const int x = S_IXUSR;
#endif

  if(readable > 0)
    m |= r;
  else if (readable < 0)
    m &= ~r;

  if(writable > 0)
    m |= w;
  else if (writable < 0)
    m &= ~w;

  if(executable > 0)
    m |= x;
  else if (executable < 0)
    m &= ~x;
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chmod-wchmod
  return chmod(path.data(), m) == 0;
#endif
}
