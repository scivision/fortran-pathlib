#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif


#include "ffilesystem.h"

#include <optional>
#include <string_view>
#include <system_error>
#include <iostream>

// inquire.cpp
// ------------

#ifndef HAVE_CXX_FILESYSTEM
#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <io.h> // _access_s
#else
#include <unistd.h>
#endif
#endif

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>


#if defined(_MSC_VER)
int
#else
mode_t
#endif
fs_st_mode(std::string_view path)
{
  struct stat s;
  if(stat(path.data(), &s)){
    // std::cerr << "ERROR:ffilesystem:fs_st_mode(" << path << ") " << strerror(errno)) << "\n";
    return 0;
  }

  return s.st_mode;
}


bool
fs_exists(std::string_view path)
{
  // fs_exists() is true even if path is non-readable
  // this is like Python pathlib.Path.exists()
  // unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  return std::filesystem::exists(path, ec) && !ec;
#else

#ifdef _MSC_VER
  // kwSys:SystemTools:FileExists is much more elaborate with Reparse point checks etc.
  // This way seems to work fine on Windows anyway.
  // io.h
  return !_access_s(path.data(), 0);
#else
  // unistd.h
  return !access(path.data(), F_OK);
#endif

#endif
}

bool
fs_is_dir(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
// NOTE: Windows top-level drive "C:" needs a trailing slash "C:/"
  std::error_code ec;
  return std::filesystem::is_directory(path, ec) && !ec;
#else
  return fs_st_mode(path) & S_IFDIR;
  // S_ISDIR not available with MSVC
#endif
}


bool
fs_is_file(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  // disqualify reserved names
  return std::filesystem::is_regular_file(path, ec) && !ec && !Ffs::is_reserved(path);
#else
    return fs_st_mode(path) & S_IFREG;
  // S_ISREG not available with MSVC
#endif
}


bool fs_is_char_device(std::string_view path)
{
// special character device like /dev/null
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  return std::filesystem::is_character_file(path, ec) && !ec;
#else
  // Windows: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32
  return fs_st_mode(path) & S_IFCHR;
  // S_ISCHR not available with MSVC
#endif
}


std::optional<uintmax_t> fs_file_size(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;

  std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
  return {};
#else
  if (struct stat s;
        !stat(path.data(), &s))
    return s.st_size;

  fs_print_error(path, "file_size");
  return {};
#endif
}


std::optional<std::string> fs_get_permissions(std::string_view path)
{
  if (!fs_exists(path)) {
    fs_print_error(path, "get_permissions: path does not exist");
    return {};
  }

  std::string r = "---------";

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec) FFS_UNLIKELY
    return {};

  const std::filesystem::perms p = s.permissions();

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
#endif

  if ((p & owner_read) != none)
    r[0] = 'r';
  if ((p & owner_write) != none)
    r[1] = 'w';
  if ((p & owner_exec) != none)
    r[2] = 'x';
  if ((p & group_read) != none)
    r[3] = 'r';
  if ((p & group_write) != none)
    r[4] = 'w';
  if ((p & group_exec) != none)
    r[5] = 'x';
  if ((p & others_read) != none)
    r[6] = 'r';
  if ((p & others_write) != none)
    r[7] = 'w';
  if ((p & others_exec) != none)
    r[8] = 'x';

  return r;
#else

#ifdef _MSC_VER
  const int m = fs_st_mode(path);
  r[0] = (m & _S_IREAD) ? 'r' : '-';
  r[1] = (m & _S_IWRITE) ? 'w' : '-';
  r[2] = (m & _S_IEXEC) ? 'x' : '-';
  r[3] = (m & _S_IREAD) ? 'r' : '-';
  r[4] = (m & _S_IWRITE) ? 'w' : '-';
  r[5] = (m & _S_IEXEC) ? 'x' : '-';
  r[6] = (m & _S_IREAD) ? 'r' : '-';
  r[7] = (m & _S_IWRITE) ? 'w' : '-';
  r[8] = (m & _S_IEXEC) ? 'x' : '-';
#else
  const mode_t m = fs_st_mode(path);
  r[0] = (m & S_IRUSR) ? 'r' : '-';
  r[1] = (m & S_IWUSR) ? 'w' : '-';
  r[2] = (m & S_IXUSR) ? 'x' : '-';
  r[3] = (m & S_IRGRP) ? 'r' : '-';
  r[4] = (m & S_IWGRP) ? 'w' : '-';
  r[5] = (m & S_IXGRP) ? 'x' : '-';
  r[6] = (m & S_IROTH) ? 'r' : '-';
  r[7] = (m & S_IWOTH) ? 'w' : '-';
  r[8] = (m & S_IXOTH) ? 'x' : '-';
#endif
  return r;

#endif
}
