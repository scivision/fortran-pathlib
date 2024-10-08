#include <stdbool.h>
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>  // GetTokenInformation
#else
#include <unistd.h>  // geteuid, getpid
#include <sys/types.h>  // geteuid, pid_t
#endif

#include "ffilesystem.h"


bool fs_is_admin(){
  // running as admin / root / superuser
#ifdef _WIN32
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

  bool ok = (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) &&
     GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize));

  CloseHandle(hToken);
  if(ok)
    return elevation.TokenIsElevated;

  return false;
#else
  return geteuid() == 0;
#endif
}


int fs_getpid()
{
#ifdef _WIN32
  return (int) GetCurrentProcessId();
#else
  return (int) getpid();
#endif
}


size_t fs_get_terminal(char* name, const size_t buffer_size)
{
#ifdef _WIN32
  // inspired by https://gitlab.kitware.com/utils/kwsys/-/commit/0d6eac1feb8615fe59e8f972d41d1eaa8bc9baf8
  int const L = GetClassNameA(GetConsoleWindow(), name, (int) buffer_size);
  // Windows Console Host: ConsoleWindowClass
  // Windows Terminal / ConPTY: PseudoConsoleWindow (undocumented)
  if(L > 0)
    return (size_t) L;

  fs_print_error(name, "get_terminal");
  return 0;
#else
  return fs_getenv("TERM", name, buffer_size);
#endif

}
