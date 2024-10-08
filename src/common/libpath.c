// must be before ANY other include
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <stdio.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#elif defined(HAVE_DLADDR)
#include <dlfcn.h> // dladdr
static void dl_dummy_func() {}
#endif

#include "ffilesystem.h"


size_t fs_lib_path(FFS_MUNUSED char* path, FFS_MUNUSED const size_t buffer_size)
{

  size_t L=0;

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  L = GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), path, (DWORD) buffer_size);
  if(L == 0 || L >= buffer_size){
    fs_print_error(path, "lib_path");
    return 0;
  }
#elif defined(HAVE_DLADDR)
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
