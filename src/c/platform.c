#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#ifndef __has_include
#define __has_include(x) 0
#endif

#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#endif

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // getenv, setenv, putenv
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h>  // getcwd, chdir
#include <Windows.h>
#include <UserEnv.h>
#else
#include <pwd.h>  /* getpwuid */
#include <unistd.h>
#endif


#ifndef _WIN32
static inline bool str_ends_with(const char *s, const char *suffix) {
  /* https://stackoverflow.com/a/41652727 */
    const size_t slen = strlen(s);
    const size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}
#endif


int fs_is_wsl()
{
#if __has_include(<sys/utsname.h>)
  struct utsname buf;
  if (uname(&buf) != 0)
    return -1;

  if (strcmp(buf.sysname, "Linux") != 0)
    return 0;
  if (str_ends_with(buf.release, "microsoft-standard-WSL2"))
    return 2;
  if (str_ends_with(buf.release, "-Microsoft"))
    return 1;

  return 0;
#endif

  return -1;
}


size_t fs_getenv(const char* name, char* path, const size_t buffer_size)
{
  const char* buf = getenv(name);
  if(!buf) // not error because sometimes we just check if envvar is defined
    return 0;
  // need strncpy otherwise garbage output and/or segfault
  return fs_strncpy(buf, path, buffer_size);
}


bool fs_setenv(const char* name, const char* value)
{

#ifdef _WIN32
  // SetEnvironmentVariable set empty value despite returning success code
  const size_t L = strlen(name) + strlen(value) + 2;
  char* buf = (char*) malloc(L);
  if(!buf) return false;

  snprintf(buf, L, "%s=%s", name, value);
  if(putenv(buf) == 0){
    free(buf);
    return true;
  }
  free(buf);
#else
  // https://www.man7.org/linux/man-pages/man3/setenv.3.html
  if(setenv(name, value, 1) == 0)
    return true;
#endif

  fprintf(stderr, "ERROR:ffilesystem:fs_setenv: %s => %s\n", name, strerror(errno));
  return false;
}


size_t fs_get_tempdir(char* path, const size_t buffer_size)
{
#ifdef _WIN32
  DWORD L = GetTempPathA((DWORD) buffer_size, path);
  if (L == 0 || L >= buffer_size){
    fs_win32_print_error(path, "get_tempdir");
    return 0;
  }
  L++;
#else
  const size_t L = fs_getenv("TMPDIR", path, buffer_size);
#endif

  if(L){
    fs_as_posix(path);
    return L;
  }

  if (buffer_size > 4 && fs_is_dir("/tmp")){
    strncpy(path, "/tmp", 5);
    return 4;
  }

  return 0;
}


size_t fs_get_cwd(char* path, const size_t buffer_size)
{
// direct.h https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd
// unistd.h https://www.man7.org/linux/man-pages/man3/getcwd.3.html
  if(!getcwd(path, buffer_size)){
    fprintf(stderr, "ERROR:ffilesystem:fs_get_cwd: %s\n", strerror(errno));
    return 0;
  }

  fs_as_posix(path);
  return strlen(path);
}


bool fs_set_cwd(const char* path)
{
  // unistd.h / direct.h
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chdir-wchdir?view=msvc-170
  if(chdir(path) == 0)
    return true;

  fprintf(stderr, "ERROR:ffilesystem:set_cwd: %s    %s\n", path, strerror(errno));
  return false;
}


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
  bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0 &&
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


size_t fs_expanduser(const char* path, char* result, const size_t buffer_size)
{
  // The path is also normalized by defintion
  if(path[0] != '~')
    return fs_normal(path, result, buffer_size);
  if(strlen(path) > 1 && path[1] != '/')
    return fs_normal(path, result, buffer_size);

  if (!fs_get_homedir(result, buffer_size))
    return 0;

  // ~ alone
  if (strlen(path) < 3)
    return strlen(result);

  return fs_join(result, path+2, result, buffer_size);
}
