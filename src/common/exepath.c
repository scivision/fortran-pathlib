#include "ffilesystem.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h> // uint32_t

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


size_t fs_exe_path(FFS_MUNUSED char* path, FFS_MUNUSED const size_t buffer_size)
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

  size_t L=0;

#if defined(_WIN32) || defined(__CYGWIN__)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  L = GetModuleFileNameA(NULL, path, (DWORD) buffer_size);
  if(L == 0 || L >= buffer_size){
    fs_win32_print_error(path, "exe_path");
    return 0;
  }
  path[L] = '\0';
#elif defined(__linux__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  L = readlink("/proc/self/exe", path, buffer_size);
  if(!L){
    fprintf(stderr, "ERROR:ffilesystem:exe_path:readlink: %s\n", strerror(errno));
    return 0;
  }
  path[L] = '\0';
#elif defined(__APPLE__)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
  uint32_t mp = (uint32_t) buffer_size;

  if(_NSGetExecutablePath(path, &mp) != 0)
    return 0;
  L = strlen(path);
#elif defined(BSD)
  const int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
  if(sysctl(mib, 4, NULL, &L, NULL, 0) != 0){
    fprintf(stderr, "ERROR:ffilesystem:exe_path:sysctl: get size %s\n", strerror(errno));
    return 0;
  }
  if(sysctl(mib, 4, path, &L, NULL, 0) != 0){
    fprintf(stderr, "ERROR:ffilesystem:exe_path:sysctl: get path %s\n", strerror(errno));
    return 0;
  }
#else
  fprintf(stderr, "ERROR:ffilesystem:exe_path: not implemented for this platform\n");
  return L;
#endif
  fs_as_posix(path);
  return L;
}
