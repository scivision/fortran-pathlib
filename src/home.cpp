#include <string>
#include <string_view>

#include <set>

#if __has_include(<format>) && defined(_WIN32)
#include <format>  // IWYU pragma: keep
#endif


// get_profile_dir
#ifdef _WIN32
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <UserEnv.h> // GetUserProfileDirectoryA
#include <Windows.h>
#include <tlhelp32.h> // for CreateToolhelp32Snapshot
#include <psapi.h>  // for EnumProcessModules
#else
#include <sys/types.h>  // IWYU pragma: keep
#include <pwd.h>      // for getpwuid, passwd
#include <unistd.h> // for mac too
#endif

#include "ffilesystem.h"


#ifndef _WIN32
static struct passwd* fs_getpwuid()
{
  const uid_t eff_uid = geteuid();

  if(struct passwd *pw = getpwuid(eff_uid); pw) FFS_LIKELY
    return pw;

  fs_print_error(
#ifdef __cpp_lib_format  // C++20
    std::format("uid: {}", eff_uid)
#else
    ""
#endif
    , "getpwuid");

  return {};
}
#endif


std::string fs_get_homedir()
{
  // has no trailing slash
  if(std::string home = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME");
      !home.empty())  FFS_LIKELY
    return fs_drop_slash(fs_as_posix(home));

  return fs_get_profile_dir();
}


std::string fs_get_profile_dir()
{
  // has no trailing slash
  #ifdef _WIN32
  // https://learn.microsoft.com/en-us/windows/win32/api/userenv/nf-userenv-getuserprofiledirectorya
  std::string path(fs_get_max_path(), '\0');
  // works on MSYS2, MSVC, oneAPI
  HANDLE hToken = nullptr;
  auto N = static_cast<DWORD>(path.size());

  const bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0 &&
    GetUserProfileDirectoryA(hToken, path.data(), &N);

  CloseHandle(hToken);

  if (ok){
    path.resize(N-1);
    return fs_drop_slash(fs_as_posix(path));
  }
#else
  const struct passwd *pw = fs_getpwuid();
  if (pw)
    return fs_drop_slash(pw->pw_dir);
#endif

  fs_print_error("", "get_profile_dir");
  return {};
}


std::string fs_expanduser(std::string_view path)
{
  if(path.empty()) FFS_UNLIKELY
    return {};

  if(path.front() != '~')
    return std::string(path);

  const std::set <char> filesep = {'/', fs_is_windows() ? '\\' : '/'};

  if(path.length() > 1 &&
     // second character is not a file separator
#if __cplusplus >= 202002L
  !filesep.contains(path[1])
#else
  filesep.find(path[1]) == filesep.end()
#endif
  )
    return std::string(path);

  std::string home = fs_get_homedir();
  if(home.empty()) FFS_UNLIKELY
    return {};

  if (path.length() < 3)
    return home;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
  std::string::size_type i = 2;
  while(i < path.length() &&
#if __cplusplus >= 202002L
  filesep.contains(path[i])
#else
  filesep.find(path[i]) != filesep.end()
#endif
  )
    i++;

  std::string e = home + "/" + std::string(path).substr(i);

  if (e.back() == '/')
    e.pop_back();

  return e;
}


std::string fs_get_username()
{
#ifdef _WIN32
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea
  std::string name(fs_get_max_path(), '\0');
  auto L = static_cast<DWORD>(name.size());
  // Windows.h
  if(GetUserNameA(name.data(), &L) != 0){
    name.resize(L-1);
    return name;
  }
#else
  const struct passwd *pw = fs_getpwuid();
  if (pw)
    return pw->pw_name;
#endif

  fs_print_error("", "get_username");
  return {};
}


std::string fs_get_shell()
{
#ifdef _WIN32
    const HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // 0: current process
    PROCESSENTRY32 pe;
    ZeroMemory(&pe, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);
    HMODULE hMod;
    DWORD cbNeeded;

    std::string name(fs_get_max_path(), '\0');

    if( Process32First(h, &pe)) {
      const DWORD pid = GetCurrentProcessId();
      do {
        if (pe.th32ProcessID == pid) {
          HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                PROCESS_VM_READ,
                                FALSE, pe.th32ParentProcessID );
          if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) {
              const auto L = GetModuleBaseNameA( hProcess, hMod, name.data(), static_cast<DWORD>(name.size()) );
              CloseHandle( hProcess );
              if(L == 0)
                goto err;

              name.resize(L);
              break;
          }
          CloseHandle( hProcess );
if(FS_TRACE) std::cout << "TRACE: get_shell: " << name << " PID: " << pid << " PPID: " << pe.th32ParentProcessID << "\n";
        }
      } while( Process32Next(h, &pe));
    }

    CloseHandle(h);
    return name;
#else
    const struct passwd *pw = fs_getpwuid();
    if (pw)
      return pw->pw_shell;
#endif

#if !defined(_MSC_VER) && __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
err:
  fs_print_error("", "get_shell");
  return {};
}
