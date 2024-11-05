#include "ffilesystem.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <iostream>
#include <string>

#include <system_error>


#ifndef HAVE_CXX_FILESYSTEM
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h> // readlink(), symlink()
#endif
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif


bool fs_is_symlink(std::string_view path)
{
#if defined(__MINGW32__)
  if (const DWORD a = GetFileAttributesA(path.data());
        a != INVALID_FILE_ATTRIBUTES)  FFS_LIKELY
    return a & FILE_ATTRIBUTE_REPARSE_POINT;
#elif defined(HAVE_CXX_FILESYSTEM)
// std::filesystem::symlink_status doesn't detect symlinks on MinGW
  std::error_code ec;
  const auto s = std::filesystem::symlink_status(path, ec);
  if(!ec) FFS_LIKELY
    return std::filesystem::is_symlink(s);

  std::cerr << "ERROR:ffilesystem:is_symlink(" << path << "): " << ec.message() << "\n";
  return false;
#elif defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (FS_TRACE) std::cout << "TRACE: statx() is_symlink " << path << "\n";
  struct statx s;
  if(statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_MODE, &s) == 0) FFS_LIKELY
    return S_ISLNK(s.stx_mode);
#else
  struct stat s;

  if(!lstat(path.data(), &s)) FFS_LIKELY
    return S_ISLNK(s.st_mode);
  // return (s.st_mode & S_IFMT) == S_IFLNK; // equivalent
#endif

  fs_print_error(path, "is_symlink");
  return false;
}


std::string fs_read_symlink(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  if(auto p = std::filesystem::read_symlink(path, ec); !ec) FFS_LIKELY
    return p.generic_string();

  // std::filesystem::canonical fallback not helpful here -- link is still not resolved
  std::cerr << "ERROR:ffilesystem:read_symlink: " << ec.message() << "\n";
  return {};
#elif defined(_WIN32)
  // does not work for MinGW, oneAPI Windows, ...
  // fs_canonical(path, false, result, buffer_size) just returns the unresolved symlink as in C++
  // GetFinalPathNameByHandle or GetFullPathName also returns the unresolved symlink
  std::cerr << "ERROR:Ffilesystem:read_symlink: not implemented for non-C++: " << path << "\n";
  return {};
#else
  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  const auto m = fs_get_max_path();
  std::string r(m, '\0');
  const ssize_t Lr = readlink(path.data(), r.data(), m);
  if (Lr < 0){
    fs_print_error(path, "read_symlink");
    return {};
  }
  // readlink() does not null-terminate the result
  r.resize(Lr);
  return r;
#endif
}


bool fs_create_symlink(std::string_view target, std::string_view link)
{

  if(target.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: create_symlink: target path must not be empty\n";
    // confusing program errors if target is "" -- we'd never make such a symlink in real use.
    return false;
  }

  if(link.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: create_symlink: link path must not be empty\n";
    // macOS needs empty check to avoid SIGABRT
    return false;
  }

#if defined(_WIN32)

  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target.data()))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if(CreateSymbolicLinkA(link.data(), target.data(), p))  FFS_LIKELY
    return true;

#elif defined(HAVE_CXX_FILESYSTEM)

  std::error_code ec;

  fs_is_dir(target)
    ? std::filesystem::create_directory_symlink(target, link, ec)
    : std::filesystem::create_symlink(target, link, ec);

  if(ec) FFS_UNLIKELY
    std::cerr << "ERROR:ffilesystem:create_symlink: " << ec.message() << "\n";

  return !ec;

#else

    // necessary to avoid logic problems on macOS
  if(!fs_exists(target)) {  FFS_UNLIKELY
    std::cerr << "ERROR:filesystem:create_symlink: target " << target << "does not exist\n";
    return false;
  }

  // https://linux.die.net/man/3/symlink
  if(symlink(target.data(), link.data()) == 0)  FFS_LIKELY
    return true;

#endif

  fs_print_error(std::string(target) + " => " + std::string(link), "create_symlink");
  return false;
}
