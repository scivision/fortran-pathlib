#include <string>
#include <string_view>

#include "ffilesystem.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#endif


int fs_is_wsl()
{

#if __has_include(<sys/utsname.h>)
  struct utsname buf;
  if (uname(&buf) != 0)
    return -1;

  std::string_view s(buf.sysname);
  std::string_view r(buf.release);

  if(s != "Linux")
    return 0;
#ifdef __cpp_lib_starts_ends_with
  if (r.ends_with("microsoft-standard-WSL2"))
    return 2;
  if (r.ends_with("-Microsoft"))
    return 1;
#endif
#endif
  return 0;
}


std::string fs_cpu_arch()
{
#if __has_include(<sys/utsname.h>)

  if (struct utsname buf;
       uname(&buf) == 0)
    return buf.machine;

  fs_print_error("", "fs_cpu_arch");
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return "x86_64";
    case PROCESSOR_ARCHITECTURE_ARM:
        return "arm";
    case PROCESSOR_ARCHITECTURE_ARM64:
        return "arm64";
    case PROCESSOR_ARCHITECTURE_IA64:
        return "ia64";
    case PROCESSOR_ARCHITECTURE_INTEL:
        return "x86";
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        return "unknown";
    }
#endif

  return {};
}
