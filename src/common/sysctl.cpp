#include <cerrno>

#include "ffilesystem.h"

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

bool fs_is_rosetta() {
#ifdef __APPLE__
// https://developer.apple.com/documentation/apple-silicon/about-the-rosetta-translation-environment
    int ret = 0;
    size_t size = sizeof(ret);

    if (sysctlbyname("sysctl.proc_translated", &ret, &size, nullptr, 0) < 0) {
        if (errno != ENOENT)
            fs_print_error("sysctl.proc_translated", "fs_is_rosetta");
        return false;
    }

    return ret == 1;
#else
    return false;
#endif
}
