#include "ffilesystem.h"

#include <memory>
#include <cstddef> // size_t

// for lib_path, exe_path
#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(HAVE_DLADDR)
#include <dlfcn.h>
static void dl_dummy_func() {}
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#elif defined(__linux__) || defined(__CYGWIN__)
#include <unistd.h>
#endif
// --- end of lib_path, exe_path


size_t fs_exe_path(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::exe_path(), path, buffer_size);
}

std::string Ffs::exe_path()
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

  auto buf = std::make_unique<char[]>(fs_get_max_path());

#ifdef _WIN32
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  if (!GetModuleFileNameA(nullptr, buf.get(), static_cast<DWORD>(fs_get_max_path()))) FFS_UNLIKELY
    return {};
#elif defined(__linux__) || defined(__CYGWIN__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  size_t L = readlink("/proc/self/exe", buf.get(), fs_get_max_path());
  if (L < 1 || L >= fs_get_max_path()) FFS_UNLIKELY
    return {};
#elif defined(__APPLE__)
  uint32_t mp = fs_get_max_path();
  if(_NSGetExecutablePath(buf.get(), &mp)) FFS_UNLIKELY
    return {};
#else
  std::cerr << "ERROR:ffilesystem:exe_path: not implemented for this platform\n";
  return {};
#endif

  std::string s(buf.get());
  return Ffs::canonical(s, true);
}


size_t fs_lib_path(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::lib_path(), path, buffer_size);
}

std::string Ffs::lib_path()
{
#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
  auto buf = std::make_unique<char[]>(fs_get_max_path());

 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  if(!GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), buf.get(), fs_get_max_path())) FFS_UNLIKELY
    return {};

  std::string s(buf.get());
  return s;
#elif defined(HAVE_DLADDR)
  Dl_info info;

  return dladdr( (void*)&dl_dummy_func, &info)
    ? std::string(info.dli_fname)
    : std::string();
#else
  return {};
#endif
}
