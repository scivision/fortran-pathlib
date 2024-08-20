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

#include <iostream>
#include <string>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool Ffs::remove(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:FFs:remove: " << ec.message() << "\n";
  return false;
}


bool Ffs::set_permissions(std::string_view path, int readable, int writable, int executable)
{

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#elif defined(__cpp_lib_filesystem)
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

#if defined(__cpp_lib_filesystem)
  std::filesystem::path pth(path);

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
  std::cerr << "ERROR:ffilesystem:set_permissions: filesystem not available\n";
  return false;
#endif
}
