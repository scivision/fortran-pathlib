#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "ffilesystem.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#endif


#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>

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
#endif
  return 0;
}


size_t fs_cpu_arch(char* arch, const size_t buffer_size)
{
#if __has_include(<sys/utsname.h>)
  struct utsname buf;
  if (uname(&buf) == 0)
    return fs_strncpy(buf.machine, arch, buffer_size);

  fprintf(stderr, "ERROR:ffilesystem:fs_cpu_arch: %s\n", strerror(errno));
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return fs_strncpy("x86_64", arch, buffer_size);
    case PROCESSOR_ARCHITECTURE_ARM:
        return fs_strncpy("arm", arch, buffer_size);
    case PROCESSOR_ARCHITECTURE_ARM64:
        return fs_strncpy("arm64", arch, buffer_size);
    case PROCESSOR_ARCHITECTURE_IA64:
        return fs_strncpy("ia64", arch, buffer_size);
    case PROCESSOR_ARCHITECTURE_INTEL:
        return fs_strncpy("x86", arch, buffer_size);
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        return fs_strncpy("unknown", arch, buffer_size);
    }
#endif

  return 0;
}
