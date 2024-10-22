#include <string>
#include <algorithm> // toupper, std::ranges::contains, std::find
#include <set>
#include <cctype> // std::isalnum
#include <vector>

#if __has_include(<ranges>)
#include <ranges>
#endif

#include <iostream>

#if defined(__unix__)
// https://github.com/cpredef/predef/blob/master/OperatingSystems.md#bsd-environment
#include <sys/param.h>
#endif

#if defined(__linux__) && __has_include(<linux/magic.h>)
#include <sys/vfs.h>
#include <linux/magic.h>
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/magic.h
#elif defined(__APPLE__) || defined(BSD)
#include <sys/mount.h>
#elif defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ffilesystem.h"


bool fs_is_reserved(std::string_view path)
{
  if(!fs_is_windows())
    return false;

  std::string s = fs_stem(path);
  auto L = s.length();
  if(L < 3 || L > 4)
    return false;

  // convert to upper case
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);

// check if the stem is a reserved device name
  const std::vector<std::string_view> r = {"CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

#ifdef _cpp_lib_ranges_contains  // C++23
  if (std::ranges::contains(r, s))
#elif __cpp_lib_ranges // C++20
  if (std::ranges::find(r, s) != r.end())
#else // C++17
  if (std::find(r.begin(), r.end(), s) != r.end())
#endif
    return true;

return false;
}


static bool fs_is_safe_char(const char c)
{
  // unordered_set<char>  8us
  // set<char, std::less<>>  6us
  // vector<char> 0.3us so much faster!
  const std::vector<char> safe {'_', '-', '.', '~', '@', '#', '$', '%', '^', '&', '(', ')', '[', ']', '{', '}', '+', '=', ',', '!'};

  return std::isalnum(c) ||
#ifdef __cpp_lib_ranges // C++20
    std::ranges::find(safe, c)
#else // C++17
    std::find(safe.begin(), safe.end(), c)
#endif
    != safe.end();
}


bool fs_is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  // empty check for MSVC
  if(fs_is_windows() && !filename.empty() && filename.back() == '.') FFS_UNLIKELY
    return false;

#ifdef __cpp_lib_ranges // C++20
  return std::ranges::all_of(filename, fs_is_safe_char);
#else // C++11
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}

#if defined(__linux__) && __has_include(<linux/magic.h>)
static inline std::string fs_type_linux(std::string_view path)
{
  struct statfs s;

  if(statfs(path.data(), &s)) {
    fs_print_error(path, "filesystem_type:statfs");
    return {};
  }

  switch (s.f_type) {
    case BTRFS_SUPER_MAGIC: return "btrfs";
    case EXT4_SUPER_MAGIC: return "ext4";
#ifdef EXFAT_SUPER_MAGIC
    case EXFAT_SUPER_MAGIC: return "exfat";
#endif
#ifdef F2FS_SUPER_MAGIC
    case F2FS_SUPER_MAGIC: return "f2fs";
#endif
#ifdef FUSE_SUPER_MAGIC
    case FUSE_SUPER_MAGIC: return "fuse";
#endif
    case NFS_SUPER_MAGIC: return "nfs";
    case SQUASHFS_MAGIC: return "squashfs";
    case TMPFS_MAGIC: return "tmpfs";
#ifdef TRACEFS_MAGIC
    case TRACEFS_MAGIC: return "tracefs";
#endif
#ifdef UDF_SUPER_MAGIC
    case UDF_SUPER_MAGIC: return "udf";
#endif
    case V9FS_MAGIC: return "v9fs";
    // used for WSL
    // https://devblogs.microsoft.com/commandline/whats-new-for-wsl-in-windows-10-version-1903/
#ifdef XFS_SUPER_MAGIC
    case XFS_SUPER_MAGIC: return "xfs";
#endif
    default:
      std::cerr << "ERROR:fs_filesystem_type " << path << " unknown type ID: " << s.f_type << "\n";
      return {};
  }
}
#endif


std::string fs_filesystem_type(std::string_view path)
{
  // return name of filesystem type if known

#if defined(_WIN32) || defined(__CYGWIN__)

  std::string r(path);

  // Cygwin: assume user input Cygwin path root directly.
  if(!fs_is_cygwin()){
    if (r.length() == 2 && std::isalpha(r[0]) && r[1] == ':')
        r += "\\";

    r = fs_root(r);

    if (r.length() < 3)
      return {};

    // GetVolumeInformationA requires a trailing backslash
    if (r.length() == 3 && r[2] == '/')
      r[2] = '\\';
  }

  if(FS_TRACE) std::cout << "TRACE:filesystem_type(" << path << ") root: " << r << "\n";

  std::string name(MAX_PATH+1, '\0');
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationa
  if(GetVolumeInformationA(r.data(), nullptr, 0, nullptr, nullptr, nullptr, name.data(), name.size()))
    return fs_trim(name);

  fs_print_error(path, "filesystem_type");
  return {};
#elif defined(__linux__)
#if __has_include(<linux/magic.h>)
  return fs_type_linux(path);
#else
  fs_print_error(path, "filesystem_type: linux/magic.h not found");
#endif
#elif defined(__APPLE__) || defined(BSD)
  struct statfs s;
  if(!statfs(path.data(), &s))
    return s.f_fstypename;
#else
  fs_print_error(path, "filesystem_type: Unknown operating system");
#endif
  return {};
}
