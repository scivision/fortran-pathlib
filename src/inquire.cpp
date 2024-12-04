// IWYU pragma: no_include <bits/statx-generic.h>
// IWYU pragma: no_include <linux/stat.h>

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <iostream>  // IWYU pragma: keep
#include <cstdint> // uintmax_t

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <io.h> // _access_s
#else
#include <unistd.h>
#endif
#endif

#include <sys/types.h>  // IWYU pragma: keep
#include <sys/stat.h>   // IWYU pragma: keep

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif


bool fs_has_statx()
{
// https://www.man7.org/linux/man-pages/man2/statx.2.html
#if defined(STATX_MODE)
  return true;
#else
  return false;
#endif
}


int
fs_st_mode(std::string_view path)
{
#if defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  if(struct statx s;
      statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MODE, &s) == 0)
    return s.stx_mode;
#else

  if(struct stat s;
      !stat(path.data(), &s))
    return s.st_mode;
#endif
  // std::cerr << "ERROR:ffilesystem:fs_st_mode(" << path << ") " << strerror(errno)) << "\n";
  return 0;
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
#elif defined(_MSC_VER)
  // kwSys:SystemTools:FileExists is much more elaborate with Reparse point checks etc.
  // This way seems to work fine on Windows anyway.
  // io.h
  return !_access_s(path.data(), 0);
#else
  // unistd.h
  return !access(path.data(), F_OK);
#endif
}


bool
fs_is_dir(std::string_view path)
{
#if defined(HAVE_CXX_FILESYSTEM)
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
  return std::filesystem::is_regular_file(path, ec) && !ec;
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


bool fs_is_exe(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM

  std::error_code ec;

  const auto s = std::filesystem::status(path, ec);
  // need reserved check for Windows
  if(ec || !std::filesystem::is_regular_file(s) || fs_is_reserved(path))
    return false;

  // Windows MinGW bug with executable bit
  if(fs_is_mingw())
    return fs_is_readable(path);

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  return (s.permissions() & (owner_exec | group_exec | others_exec)) != none;

#else

  return (fs_is_file(path) &&
#ifdef _WIN32
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  // in Windows, all readable files are executable. Do not use _S_IEXEC, it is not reliable.
  fs_is_readable(path)
#else
  !access(path.data(), X_OK)
#endif
  );

#endif
}


bool fs_is_readable(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
// directory or file readable
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
#endif

  return (s.permissions() & (owner_read | group_read | others_read)) != none;

#elif defined(_MSC_VER)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return !_access_s(path.data(), 4);
#else
  return !access(path.data(), R_OK);
#endif
}


bool fs_is_writable(std::string_view path)
{
  // directory or file writable
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
#endif

  return (s.permissions() & (owner_write | group_write | others_write)) != none;
#elif defined(_MSC_VER)
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return !_access_s(path.data(), 2);
#else
  return !access(path.data(), W_OK);
#endif
}


std::uintmax_t fs_hard_link_count(std::string_view path)
{
  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM

  if(auto s = std::filesystem::hard_link_count(path, ec); !ec)  FFS_LIKELY
    return s;

#elif defined(STATX_BASIC_STATS) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (fs_trace) std::cout << "TRACE: statx() hard_link " << path << "\n";
  struct statx s;

  if( statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_BASIC_STATS, &s) == 0 ) FFS_LIKELY
    return s.stx_nlink;

#else
  if (struct stat s;
        !stat(path.data(), &s))
    return s.st_nlink;

#endif

  fs_print_error(path, "hard_link_count", ec);
  return {};
}
