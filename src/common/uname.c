#include <string.h>
#include <stdbool.h>

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

  return 0;
#else
  return -1;
#endif

}
