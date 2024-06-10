#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// get_homedir backup method
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <UserEnv.h> // GetUserProfileDirectoryA
#include <Windows.h>
#else
#include <sys/types.h>
#include <pwd.h>      // for getpwuid, passwd
#include <unistd.h> // for mac too
#endif
// end get_homedir backup method

#include "ffilesystem.h"


size_t fs_get_homedir(char* path, const size_t buffer_size)
{
  const size_t L = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME", path, buffer_size);
  if (L){
    fs_as_posix(path);
    return L;
  }

#ifdef _WIN32
  // works on MSYS2, MSVC, oneAPI
  DWORD N = (DWORD) buffer_size;
  HANDLE hToken = NULL;
  const bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0 &&
                                    GetUserProfileDirectoryA(hToken, path, &N);

  CloseHandle(hToken);

  if (!ok){
    fs_win32_print_error(path, "get_homedir");
    return 0;
  }

  fs_as_posix(path);
  return strlen(path);
#else
  const char *h = getpwuid(geteuid())->pw_dir;
  if (!h)
    return 0;

  return fs_strncpy(h, path, buffer_size);
#endif
}
