#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>  // GetTokenInformation
#include <io.h> // _isatty
#else
#include <unistd.h>  // geteuid, getpid, isatty
#include <sys/types.h>  // IWYU pragma: keep
// geteuid, pid_t
#endif

 #include <cstdio> // fileno

#include <string>

#include "ffilesystem.h"


bool fs_is_admin(){
  // running as admin / root / superuser
#ifdef _WIN32
	HANDLE hToken = nullptr;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

  const bool ok = (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) &&
     GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize));

  CloseHandle(hToken);
  if(ok)  FFS_LIKELY
    return elevation.TokenIsElevated;

  return false;
#else
  return geteuid() == 0;
#endif
}


int fs_getpid()
{
#ifdef _WIN32
  return static_cast<int>(GetCurrentProcessId());
#else
  return static_cast<int>(getpid());
#endif
}


bool fs_stdin_tty()
{
// detect if stdin is connected to a terminal
  return
#ifdef _WIN32
    _isatty(_fileno(stdin));
#else
    isatty(fileno(stdin));
#endif
}


std::string fs_get_terminal()
{
#ifdef _WIN32
  std::string name(fs_get_max_path(), '\0');
  // inspired by https://gitlab.kitware.com/utils/kwsys/-/commit/0d6eac1feb8615fe59e8f972d41d1eaa8bc9baf8
  // Windows Console Host: ConsoleWindowClass
  // Windows Terminal / ConPTY: PseudoConsoleWindow (undocumented)

  // https://learn.microsoft.com/en-us/windows/console/getconsolewindow
  // encourages Virtual Terminal Sequences
  if (HWND h = GetConsoleWindow(); h) {
    if (int L = GetClassNameA(h, name.data(), static_cast<int>(name.size())); L > 0) {
      name.resize(L);
      return name;
    }
  }

  fs_print_error("", "get_terminal");
  return {};
#else
  return fs_getenv("TERM");
#endif

}
