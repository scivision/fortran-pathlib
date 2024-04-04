#include "ffilesystem.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstddef> // size_t
#include <iostream>
#include <string>

#include <filesystem>



bool fs_is_symlink(const char* path)
{
  return Ffs::is_symlink(std::string_view(path));
}

bool Ffs::is_symlink(std::string_view path)
{
  if (path.empty()) FFS_UNLIKELY
    return false;

#ifdef WIN32_SYMLINK
  DWORD a = GetFileAttributes(path.data());
  return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_REPARSE_POINT);
#else
  std::error_code ec;
  auto s = std::filesystem::symlink_status(path, ec);
  // NOTE: use of symlink_status here like lstat(), else logic is wrong with std::filesystem::status()

  return !ec && std::filesystem::is_symlink(s);
#endif
}


size_t fs_read_symlink(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::read_symlink(std::string_view(path)), result, buffer_size);
}

std::string Ffs::read_symlink(std::string_view path)
{
  std::error_code ec;
  auto p = std::filesystem::read_symlink(path, ec);
  if(ec) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:read_symlink: " << ec.message() << "\n";
    return {};
  }

  return p.generic_string();
  // std::filesystem::canonical fallback not helpful here
}


bool fs_create_symlink(const char* target, const char* link)
{
  try{
    return Ffs::create_symlink(std::string_view(target), std::string_view(link));
  } catch(std::filesystem::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:create_symlink: " << e.what() << "\n";
    return false;
  }
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

  auto s = std::filesystem::status(target);

#ifdef WIN32_SYMLINK
// https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#iso.2017.specific
// 30.10.2.1 [fs.conform.9945] The behavior of the filesystem library implementation
// will depend on the target operating system. Some features will not be supported
// on some targets.
// Symbolic links and file permissions are not supported on Windows.

  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(std::filesystem::is_directory(s))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if (CreateSymbolicLinkA(link.data(), target.data(), p)) FFS_LIKELY
    return true;

  DWORD err = GetLastError();
  std::string msg = "ERROR:Ffs:CreateSymbolicLink";
  if(err == ERROR_PRIVILEGE_NOT_HELD)
    msg += "Enable Windows developer mode to use symbolic links: https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging";

  std::cerr << msg << ": " << link << " => " << target << "\n";
  return false;
#else
  std::filesystem::is_directory(s)
    ? std::filesystem::create_directory_symlink(target, link)
    : std::filesystem::create_symlink(target, link);

  return true;
#endif
}
