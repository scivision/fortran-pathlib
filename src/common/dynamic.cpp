#include "ffilesystem.h"

#include <memory>
#include <cstddef> // size_t

// for lib_path, exe_path
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

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


std::string Ffs::exe_path()
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

#ifndef __cpp_lib_make_unique
#if __cplusplus >= 202302L
  #warning "compiler lacks C++ 14 make_unique: Ffs::exe_path"
#endif
  return {};
#else
  const size_t MP = fs_get_max_path();

  [[maybe_unused]] auto buf = std::make_unique<char[]>(fs_get_max_path());

#ifdef _WIN32
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  if(size_t L = GetModuleFileNameA(nullptr, buf.get(), static_cast<DWORD>(MP));
      L == 0 || L >= MP)  FFS_UNLIKELY
    return {};
#elif defined(__linux__) || defined(__CYGWIN__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  if (size_t L = readlink("/proc/self/exe", buf.get(), MP);
      L == 0 || L >= MP)  FFS_UNLIKELY
    return {};
#elif defined(__APPLE__)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
  if(uint32_t mp = static_cast<uint32_t>(MP);
      _NSGetExecutablePath(buf.get(), &mp) != 0) FFS_UNLIKELY
    return {};
#else
  std::cerr << "ERROR:ffilesystem:exe_path: not implemented for this platform\n";
  return {};
#endif

  return Ffs::as_posix(buf.get());
#endif
}


std::string Ffs::lib_path()
{

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)

  const size_t MP = fs_get_max_path();
  auto buf = std::make_unique<char[]>(MP);
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea

  if (size_t L = GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), buf.get(), MP);
        L > 0 && L < MP)  FFS_LIKELY
    return Ffs::as_posix(buf.get());
#elif __has_include(<dlfcn.h>)
  Dl_info info;
  if(dladdr( (void*)&dl_dummy_func, &info))  FFS_LIKELY
    return std::string(info.dli_fname);
#endif

  return {};
}
