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
#include <iterator> // std::begin, std::end
#include <string>
#include <fstream>
#include <system_error>         // for error_code

#include <sys/types.h>

#if __has_include(<ranges>)
#include <ranges>
#endif

#include <ctime> // std::time_t

#include "ffilesystem.h"


bool Ffs::remove(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:FFs:remove: " << ec.message() << "\n";
  return false;
}


bool Ffs::copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
  auto opt = std::filesystem::copy_options::none;
  if (overwrite)
    opt = std::filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && Ffs::is_file(dest) && !Ffs::remove(dest)) FFS_UNLIKELY
    std::cerr << "ERROR:Ffs::copy_file: could not remove existing destination file: " << dest << "\n";

  std::error_code ec;
  if(std::filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:copy_file: " << ec.message() << "\n";
  return false;
}


bool Ffs::set_modtime(std::string_view path)
{
  std::error_code ec;

  std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
  if(!ec) FFS_LIKELY
    return true;
  // techinically IWYU <chrono> but that can break some compilers, and it works without the include.

  std::cerr << "ERROR:ffilesystem:set_modtime: " << ec.message() << "\n";
  return false;
}


bool Ffs::touch(std::string_view path)
{

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
#endif

  std::error_code ec;

  if(Ffs::exists(path)) {
    std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
    // techinically IWYU <chrono> but that can break some compilers, and it works without the include.
    return !ec;
  }

  std::ofstream ost;
  ost.open(path.data(), std::ios_base::out | std::ios_base::binary);
  if(!ost.is_open()){ FFS_UNLIKELY
    std::cerr << "ERROR:ffilesystem:touch: could not open file " << path << "\n";
    return false;
  }

  ost.close();

  // ensure user can access new file, as default permissions may be mode 600 or such
  std::filesystem::permissions(path, owner_read | owner_write, std::filesystem::perm_options::add, ec);
  if(!ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:touch: " << ec.message() << "\n";
  return false;
}


bool Ffs::set_permissions(std::string_view path, int readable, int writable, int executable)
{
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
}
