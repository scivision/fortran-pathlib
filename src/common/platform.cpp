#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif


#include "ffilesystem.h"

#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#endif

#include <memory>
#include <set>
#include <cstddef> // size_t
#include <cstdlib>

#include <cstring> // std::strlen
#include <string>
#include <iostream>

#if __has_include(<format>)
#include <format>
#endif

// for get_homedir backup method
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <UserEnv.h>
#include <Windows.h>
#else
#include <sys/types.h>
#include <pwd.h>
#include <cerrno>
#include <unistd.h> // for mac too
#endif
// end get_homedir backup method


std::string Ffs::compiler()
{
#ifdef __cpp_lib_format

#if defined(__INTEL_LLVM_COMPILER)
  return std::format("Intel LLVM {} {}", __INTEL_LLVM_COMPILER,  __VERSION__);
#elif defined(__NVCOMPILER_LLVM__)
  return std::format("NVIDIA nvc {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#elif defined(__clang__)
  #ifdef __VERSION__
    return std::format("Clang {}", __VERSION__);
  #else
    return std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#elif defined(__GNUC__)
  return std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
  return std::format("MSVC {}", _MSC_FULL_VER);
#else
  return {};
#endif

#else
  return {};
#endif
}


int fs_is_wsl() {
#if __has_include(<sys/utsname.h>)
  struct utsname buf;
  if (uname(&buf) != 0) FFS_UNLIKELY
    return false;

  std::string_view release(buf.release);

  if (std::string_view(buf.sysname) != "Linux") FFS_UNLIKELY
    return 0;

#ifdef __cpp_lib_starts_ends_with
    if (release.ends_with("microsoft-standard-WSL2"))
      return 2;
    if (release.ends_with("-Microsoft"))
      return 1;
#endif
    return -1;
#else
  return 0;
#endif
}


size_t fs_getenv(const char* name, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::get_env(std::string_view(name)), result, buffer_size);
}

std::string Ffs::get_env(std::string_view name)
{
  if(auto r = std::getenv(name.data()); r && std::strlen(r) > 0)
    return r;

  return {};
}

bool fs_setenv(const char* name, const char* value)
{
  return Ffs::set_env(std::string_view(name), std::string_view(value));
}


bool Ffs::set_env(std::string_view name, std::string_view value)
{

  if(
#ifdef _WIN32
  // SetEnvironmentVariable returned OK but set blank values
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv
  std::string v = std::string(name) + "=" + std::string(value); putenv(v.data()) == 0
#else
  // https://www.man7.org/linux/man-pages/man3/setenv.3.html
  setenv(name.data(), value.data(), 1) == 0
#endif
   ) return true;

  std::cerr << "Ffs:set_env: could not set environment variable " << name << "\n";
  return false;
}


size_t fs_get_tempdir(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_tempdir(), path, buffer_size);
}

std::string Ffs::get_tempdir()
{
  std::error_code ec;
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();

  std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
  return {};
}


size_t fs_get_cwd(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_cwd(), path, buffer_size);
}

std::string Ffs::get_cwd()
{
  std::error_code ec;
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();

  std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
  return {};
}


bool fs_set_cwd(const char *path)
{
  return Ffs::chdir(path);
}

bool Ffs::chdir(std::string_view path)
{
  std::error_code ec;
  if(std::filesystem::current_path(path, ec); !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:chdir: " << ec.message() << "\n";
  return false;
}


size_t fs_get_homedir(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_homedir(), path, buffer_size);
}

std::string Ffs::get_homedir()
{
  // must be std::string to avoid dangling pointer -- GCC doesn't detect this but Clang does.
  std::string homedir = Ffs::get_env(fs_is_windows() ? "USERPROFILE" : "HOME");
  if(!homedir.empty()) FFS_LIKELY
    return Ffs::as_posix(homedir);

#ifdef _WIN32
  // works on MSYS2, MSVC, oneAPI.
  auto L = static_cast<DWORD>(fs_get_max_path());
  auto buf = std::make_unique<char[]>(L);
  // process with query permission
  HANDLE hToken = nullptr;
  if(!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken)) FFS_UNLIKELY
  {
		CloseHandle(hToken);
    return {};
  }

  bool ok = GetUserProfileDirectoryA(hToken, buf.get(), &L);
  CloseHandle(hToken);
  if (!ok) FFS_UNLIKELY
    return {};

  homedir = std::string(buf.get());
#else
  const char *h = getpwuid(geteuid())->pw_dir;
  if (!h) FFS_UNLIKELY
    return {};
  homedir = std::string(h);
#endif

  return Ffs::as_posix(homedir);
}


size_t fs_expanduser(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::expanduser(std::string_view(path)), result, buffer_size);
}

std::string Ffs::expanduser(std::string_view path)
{
  if(path.empty()) FFS_UNLIKELY
    return {};
  // cannot call .front() on empty string_view() (MSVC)

  if(path.front() != '~')
    return std::string(path);

  std::set <char> filesep = {'/', std::filesystem::path::preferred_separator};

  if(path.length() > 1 &&
     // second character is not a file separator
#if __cplusplus >= 202002L
  !filesep.contains(path[1])
#else
  filesep.find(path[1]) == filesep.end()
#endif
  )
    return std::string(path);


  std::string h = Ffs::get_homedir();
  if (h.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:expanduser: could not get home directory\n";
    return {};
  }

  std::string p = fs_drop_slash(path);

  if (p.length() < 3)
    return h;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
  size_t i = 2;
  while(i < p.length() &&
#if __cplusplus >= 202002L
  filesep.contains(p[i])
#else
  filesep.find(p[i]) != filesep.end()
#endif
  )
    i++;

  return (std::filesystem::path(h) / p.substr(i)).generic_string();
}
