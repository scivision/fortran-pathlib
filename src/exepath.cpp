#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <iostream>
#include <string>
#include <cstdint> // uint32_t

#if defined(__unix__)
// https://github.com/cpredef/predef/blob/master/OperatingSystems.md#bsd-environment
#include <sys/param.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#elif defined(__linux__) || defined(__CYGWIN__)
#include <unistd.h> // readlink
#elif defined(BSD)
#include <sys/sysctl.h> // sysctl
#endif


std::string fs_exe_path()
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

  std::string path(fs_get_max_path(), '\0');
  const size_t buffer_size = path.size();

#if defined(_WIN32) || defined(__CYGWIN__)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  const size_t L = GetModuleFileNameA(nullptr, path.data(), (DWORD) buffer_size);
  if(L == 0 || L >= buffer_size){
    fs_print_error(path, "exe_path");
    return {};
  }
  path.resize(L);
  return fs_as_posix(path);
#elif defined(__linux__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  const size_t L = readlink("/proc/self/exe", path.data(), buffer_size);
  if(!L){
    fs_print_error("", "exe_path:readlink");
    return {};
  }
  path.resize(L);
  return path;
#elif defined(__APPLE__)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
  uint32_t mp = (uint32_t) buffer_size;

  if(_NSGetExecutablePath(path.data(), &mp) != 0){
    fs_print_error("", "exe_path:_NSGetExecutablePath");
    return {};
  }
  // "mp" doesn't have the string length of the path
  return fs_trim(path);
#elif defined(BSD)
  // https://man.freebsd.org/cgi/man.cgi?sysctl(3)
  size_t L=0;
  const int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
  if(sysctl(mib, 4, nullptr, &L, nullptr, 0) != 0){
    fs_print_error("sysctl: get size", "exe_path");
    return {};
  }
  if(sysctl(mib, 4, path.data(), &L, nullptr, 0) != 0){
    fs_print_error("sysctl: get path", "exe_path");
    return {};
  }
  path.resize(L);
  return path;
#else
  fs_print_error("ERROR:ffilesystem:exe_path: not implemented for this platform", "exepath");
  return {};
#endif

}
