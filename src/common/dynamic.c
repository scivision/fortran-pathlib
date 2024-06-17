#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include "ffilesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

// for lib_path, exe_path
#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#elif __has_include(<dlfcn.h>)
#include <dlfcn.h> // dladdr
static void dl_dummy_func() {}
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#elif defined(__linux__) || defined(__CYGWIN__)
#include <unistd.h> // readlink
#endif
// --- end of lib_path, exe_path


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
  if(!L)
    return 0;
  path[L] = '\0';
#elif defined(__APPLE__)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
  uint32_t mp = (uint32_t) buffer_size;

  if(_NSGetExecutablePath(path, &mp) != 0)
    return 0;
  L = strlen(path);
#else
  fprintf(stderr, "ERROR:ffilesystem:exe_path: not implemented for this platform\n");
  return L;
#endif
  fs_as_posix(path);
  return L;
}

size_t fs_lib_path(FFS_MUNUSED char* path, FFS_MUNUSED const size_t buffer_size)
{

  size_t L=0;

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  L = GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), path, (DWORD) buffer_size);
  if(L == 0 || L >= buffer_size){
    fs_win32_print_error(path, "lib_path");
    return 0;
  }
#elif __has_include(<dlfcn.h>)
  Dl_info info;
  if(!dladdr( (void*)&dl_dummy_func, &info))
    return 0;

  L = fs_strncpy(info.dli_fname, path, buffer_size);
#else
  fprintf(stderr, "ERROR:ffilesystem:lib_path: not implemented for this platform\n");
  return L;
#endif
  fs_as_posix(path);
  return L;
}
