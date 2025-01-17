#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h> // for OpenProcess, GetModuleBaseNameA
#include <tlhelp32.h> // for CreateToolhelp32Snapshot, Process32First
#include <psapi.h>  // for EnumProcessModules
#else
#include <pwd.h>     // forpasswd
#endif

#include "ffilesystem.h"


std::string
fs_get_shell()
{

#if defined(_WIN32)
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
            if(L == 0){
              fs_print_error("", "get_shell");
              return {};
            }

            name.resize(L);
            break;
        }
        CloseHandle( hProcess );
if(fs_trace) std::cout << "TRACE: get_shell: " << name << " PID: " << pid << " PPID: " << pe.th32ParentProcessID << "\n";
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

  fs_print_error("", "get_shell");
  return {};
}
