#include "ffilesystem.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <iostream>
#include <string>
#include <system_error>


bool Ffs::is_symlink(std::string_view path)
{
#ifdef WIN32_SYMLINK
  const DWORD a = GetFileAttributes(path.data());
  return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_REPARSE_POINT);
#else
  std::error_code ec;
  return std::filesystem::is_symlink(path, ec) && !ec;
#endif
}


std::string Ffs::read_symlink(std::string_view path)
{
  std::error_code ec;
  if(auto p = std::filesystem::read_symlink(path, ec); !ec) FFS_LIKELY
    return p.generic_string();

  // std::filesystem::canonical fallback not helpful here -- link is still not resolved
  std::cerr << "ERROR:ffilesystem:read_symlink: " << ec.message() << "\n";
  return {};
}


bool Ffs::create_symlink(std::string_view target, std::string_view link)
{
  if(target.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: Ffs::create_symlink: target path must not be empty\n";
    // confusing program errors if target is "" -- we'd never make such a symlink in real use.
    return false;
  }
  if(link.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: Ffs::create_symlink: link path must not be empty\n";
    // macOS needs empty check to avoid SIGABRT
    return false;
  }

#ifdef WIN32_SYMLINK
  return fs_win32_create_symlink(target.data(), link.data());
#else
  std::error_code ec;

  Ffs::is_dir(target)
    ? std::filesystem::create_directory_symlink(target, link, ec)
    : std::filesystem::create_symlink(target, link, ec);

  if(ec) FFS_UNLIKELY
    std::cerr << "ERROR:ffilesystem:create_symlink: " << ec.message() << "\n";

  return !ec;
#endif
}
