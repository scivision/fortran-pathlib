#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

bool fs_is_rosetta() {
#ifdef __APPLE__
// https://developer.apple.com/documentation/apple-silicon/about-the-rosetta-translation-environment
    int ret = 0;
    size_t size = sizeof(ret);

    if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) < 0) {
        if (errno != ENOENT)
            fprintf(stderr, "ERROR:fs_is_rosetta:sysctlbyname: %s\n", strerror(errno));
        return false;
    }

    return ret == 1;
#else
    return false;
#endif
}
