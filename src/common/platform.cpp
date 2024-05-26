#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif


#include "ffilesystem.h"

#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#endif

#include <cstdlib>

#include <cstring> // std::strerror
#include <string>
#include <iostream>
#include <system_error>

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
#include <pwd.h>  // getpwuid
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
    return -1;

  std::string_view sysname(buf.sysname);
  std::string_view release(buf.release);

  if (sysname != "Linux")
    return 0;

#ifdef __cpp_lib_starts_ends_with
  if (release.ends_with("microsoft-standard-WSL2"))
    return 2;
  if (release.ends_with("-Microsoft"))
    return 1;
#else
  if (release.find("microsoft-standard-WSL2") != std::string::npos)
    return 2;
  if (release.find("-Microsoft") != std::string::npos)
    return 1;
#endif

  return 0;
#else
  return -1;
#endif
}


std::string Ffs::get_env(std::string_view name)
{
  if(auto r = std::getenv(name.data()); r)
    return r;

  return {};
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


std::string Ffs::get_tempdir()
{
  std::error_code ec;
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();

  std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
  return {};
}


std::string Ffs::get_cwd()
{
  std::error_code ec;
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();

  std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
  return {};
}


bool Ffs::chdir(std::string_view path)
{
  std::error_code ec;
  if(std::filesystem::current_path(path, ec); !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:chdir: " << ec.message() << "\n";
  return false;
}


std::string Ffs::get_homedir()
{
  // must be std::string to avoid dangling pointer -- GCC doesn't detect this but Clang does.

  if(std::string h = Ffs::get_env(fs_is_windows() ? "USERPROFILE" : "HOME"); !h.empty()) FFS_LIKELY
    return Ffs::as_posix(h);

#if defined(_WIN32)
  // works on MSYS2, MSVC, oneAPI.
  auto L = static_cast<DWORD>(fs_get_max_path());
  std::string buf(L, '\0');
  // process with query permission
  HANDLE hToken = nullptr;
  const bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0 &&
            GetUserProfileDirectoryA(hToken, buf.data(), &L);

  CloseHandle(hToken);

  if (ok) FFS_LIKELY
    return Ffs::as_posix(buf.c_str());

  std::cerr << "ERROR: Ffilesystem::homedir " << std::system_category().message(GetLastError()) << "\n";
  return {};
#elif !defined(_WIN32)
  if (const char *h = getpwuid(geteuid())->pw_dir; h)
    return h;
#endif

  std::cerr << "ERROR:ffilesystem:homedir: could not get home directory: " << std::strerror(errno) << "\n";
  return {};
}


std::string Ffs::expanduser(std::string_view path)
{
  if(path.empty()) FFS_UNLIKELY
    return {};
  // cannot call .front() on empty string_view() (MSVC)

  if(path.front() != '~')
    return std::string(path);

  const std::string filesep = {'/', std::filesystem::path::preferred_separator};

  // second character is not a file separator
  if(path.length() > 1 && filesep.find(path[1]) == std::string::npos)
    return std::string(path);

  const std::string h = Ffs::get_homedir();
  if (h.empty()) FFS_UNLIKELY
    return {};

  const std::string p = fs_drop_slash(path);
  if (p.length() < 3)
    return h;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
  std::string::size_type i = 2;
  while(i < p.length() && filesep.find(p[i]) != std::string::npos)
    i++;

  return (std::filesystem::path(h) / p.substr(i)).generic_string();
}
