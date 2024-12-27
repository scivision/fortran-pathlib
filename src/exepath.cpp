#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string>

#if defined(__unix__)
// https://github.com/cpredef/predef/blob/master/OperatingSystems.md#bsd-environment
#include <sys/param.h> // IWYU pragma: keep
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#include <cstddef> // for size_t
#elif defined(__APPLE__) && defined(__MACH__)
#include <cstdint> // for uint32_t
#include <mach-o/dyld.h> // _NSGetExecutablePath
#elif defined(__linux__) || defined(__CYGWIN__)
#include <unistd.h> // for readlink, size_t
#elif defined(BSD)
#include <sys/sysctl.h> // sysctl
#include <cstddef> // for size_t
#endif


std::string fs_exe_path()
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

  std::string path(fs_get_max_path(), '\0');

#if defined(_WIN32) || defined(__CYGWIN__)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  const size_t L = GetModuleFileNameA(nullptr, path.data(), static_cast<DWORD>(path.size()));
  if(L > 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER){  FFS_LIKELY
    path.resize(L);
    return fs_as_posix(path);
  }
#elif defined(__linux__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  const size_t L = readlink("/proc/self/exe", path.data(), path.size());
  if(L){  FFS_LIKELY
    path.resize(L);
    return path;
  }
#elif defined(__APPLE__) && defined(__MACH__)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
  uint32_t mp = static_cast<uint32_t>(path.size());

  if(_NSGetExecutablePath(path.data(), &mp) == 0)  FFS_LIKELY
    return fs_trim(path);
  // "mp" doesn't have the string length of the path
#elif defined(BSD)
  // https://man.freebsd.org/cgi/man.cgi?sysctl(3)
  size_t L = path.size();
  const int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
  if(sysctl(mib, 4, path.data(), &L, nullptr, 0) == 0){  FFS_LIKELY
    path.resize(L);
    return path;
  }
#endif

  fs_print_error("", "exepath");
  return {};
}
