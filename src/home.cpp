#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string>
#include <cstdlib>
#include <set>

#if __has_include(<format>)
#include <format>
#endif

#include <iostream>

// get_profile_dir
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <UserEnv.h> // GetUserProfileDirectoryA
#include <Windows.h>
#include <tlhelp32.h> // for CreateToolhelp32Snapshot
#include <psapi.h>  // for EnumProcessModules
#else
#include <sys/types.h>
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
#ifdef __cpp_lib_format
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

  if(std::string home = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME");
      !home.empty())  FFS_LIKELY
    return fs_as_posix(home);

  return fs_get_profile_dir();
}


std::string fs_get_profile_dir()
{
  #ifdef _WIN32
  // https://learn.microsoft.com/en-us/windows/win32/api/userenv/nf-userenv-getuserprofiledirectorya
  const std::string::size_type m = fs_get_max_path();
  std::string path(m, '\0');
  // works on MSYS2, MSVC, oneAPI
  auto N = (DWORD) m;
  HANDLE hToken = nullptr;

  bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0;

  if(!ok){
    CloseHandle(hToken);
    fs_print_error(path, "get_profile_dir:OpenProcessToken");
    return {};
  }

  ok = GetUserProfileDirectoryA(hToken, path.data(), &N);
  CloseHandle(hToken);

  if (!ok){
    fs_print_error(path, "get_profile_dir:GetUserProfileDirectory");
    return {};
  }

  path.resize(N-1);

  return fs_as_posix(path);
#else
  const struct passwd *pw = fs_getpwuid();
  if (!pw)
    return {};

  return pw->pw_dir;
#endif
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

  if (home.back() == '/')
    home.pop_back();

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
  const std::string::size_type m = fs_get_max_path();
  std::string name(m, '\0');
  auto L = (DWORD) m;
  // Windows.h
  if(GetUserNameA(name.data(), &L) == 0){
    fs_print_error("", "get_username:GetUserName");
    return {};
  }

  name.resize(L-1);

  return name;
#else
  const struct passwd *pw = fs_getpwuid();
  if (!pw){
    fs_print_error("", "get_username:getpwuid");
    return {};
  }

  return pw->pw_name;
#endif
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

    const std::string::size_type m = fs_get_max_path();

    std::string name(m, '\0');

    if( Process32First(h, &pe)) {
      const DWORD pid = GetCurrentProcessId();
      do {
        if (pe.th32ProcessID == pid) {
          HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                PROCESS_VM_READ,
                                FALSE, pe.th32ParentProcessID );
          if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) {
              const auto L = GetModuleBaseNameA( hProcess, hMod, name.data(), (DWORD) m );
              CloseHandle( hProcess );
              if(L == 0){
                fs_print_error(name, "get_shell:GetModuleBaseName");
                return {};
              }
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
    if (!pw)
      return {};

    return pw->pw_shell;
#endif
}
