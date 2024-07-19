#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h> // putenv, setenv, malloc, free, wcstombs
#include <string.h>

#ifdef _WIN32
#include <Objbase.h> // CoTaskMemFree
#include <KnownFolders.h> // FOLDERID_LocalAppData
#include <Shlobj.h> // SHGetKnownFolderPath
#endif

#include "ffilesystem.h"


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
  // SetEnvironmentVariable returned OK but set blank values
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv
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

size_t fs_user_config_dir(char* path, const size_t buffer_size)
{
#ifdef _WIN32
  PWSTR s;
  if (SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &s) != S_OK) {
    CoTaskMemFree(s);
    return 0;
  }

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  wcstombs(buf, s, buffer_size);
  CoTaskMemFree(s);

  const size_t L = fs_strncpy(buf, path, buffer_size);
  free(buf);

  fs_as_posix(path);
  return L;
#else
  const char* xdg = getenv("XDG_CONFIG_HOME");
  if(xdg){
    return fs_strncpy(xdg, path, buffer_size);
  }

  return fs_get_homedir(path, buffer_size);
#endif
}
