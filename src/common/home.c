#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h> // malloc, free

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

#include <string.h>

#include "ffilesystem.h"


#ifndef _WIN32
static struct passwd* fs_getpwuid()
{
  uid_t eff_uid = geteuid();

  struct passwd *pw = getpwuid(eff_uid);
  if (pw == NULL)
    fprintf(stderr, "ERROR:ffilesystem:fs_getpwuid: UID %u %s\n", eff_uid, strerror(errno));

  return pw;
}
#endif


size_t fs_get_homedir(char* path, const size_t buffer_size)
{
  const size_t L = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME", path, buffer_size);
  if (L){
    fs_as_posix(path);
    return L;
  }

  return fs_get_profile_dir(path, buffer_size);
}


size_t fs_get_profile_dir(char* path, const size_t buffer_size)
{
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
  const struct passwd *pw = fs_getpwuid();
  if (pw == NULL)
    return 0;

  return fs_strncpy(pw->pw_dir, path, buffer_size);
#endif
}


size_t fs_expanduser(const char* path, char* result, const size_t buffer_size)
{
  const size_t L = strlen(path);
  if(L < 1){
    result[0] = '\0';  // avoid problem with hanging old buffer info
    return 0;
  }
  if(path[0] != '~')
    return fs_strncpy(path, result, buffer_size);
  if(L > 1 && !(path[1] == '/' || (fs_is_windows() && path[1] == '\\')))
    return fs_strncpy(path, result, buffer_size);

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;

  const size_t L1 = fs_get_homedir(buf, buffer_size);
  if(L1 == 0){
    free(buf);
    return 0;
  }
  if (L < 3){
    fs_strncpy(buf, result, buffer_size);
    free(buf);
    return L1;
  }

  // handle initial duplicated file separators
  size_t i = 2;
  while (i < L && (path[i] == '/' || (fs_is_windows() && path[i] == '\\')))
    i++;

  const int N = snprintf(result, buffer_size, "%s/%s", buf, path+i);

  free(buf);

  if (N < 0 || N >= (int) buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_expanduser: buffer overflow %s\n", strerror(errno));
    return 0;
  }

  fs_drop_slash(result);

  return strlen(result);
}


size_t fs_get_username(char *name, const size_t buffer_size)
{
#ifdef _WIN32
    DWORD L = buffer_size;
    // Windows.h
    if(!GetUserNameA(name, &L))
    {
      fs_win32_print_error(name, "get_username");
      return 0;
    }

    if (L < buffer_size)
      return (size_t) L;

    fprintf(stderr, "ERROR:Ffilesystem:get_username: Buffer %zu too small for %lu\n", buffer_size, L);
    return 0;
#else
    const struct passwd *pw = fs_getpwuid();
    if (pw == NULL)
      return 0;

    return fs_strncpy(pw->pw_name, name, buffer_size);
#endif
}
