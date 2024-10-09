// must be before ANY other include
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <iostream>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#elif defined(HAVE_DLADDR)
#include <dlfcn.h> // dladdr
static void dl_dummy_func() {}
#endif

#include "ffilesystem.h"


std::string fs_lib_path()
{

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea

  std::string path(fs_get_max_path(), '\0');
  const size_t buffer_size = path.size();
  size_t L=0;

  L = GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), path.data(), (DWORD) buffer_size);
  if(L == 0 || L >= buffer_size){
    fs_print_error(path, "lib_path");
    return {};
  }

  path.resize(L);

  return fs_as_posix(path);
#elif defined(HAVE_DLADDR)
  Dl_info info;
  if(!dladdr( (void*)&dl_dummy_func, &info)){
    fs_print_error("dladdr", "lib_path");
    return {};
  }

  return info.dli_fname;
#else
  std::cerr << "Ffilesystem:lib_path: not implemented for this platform\n";
  return {};
#endif

}
