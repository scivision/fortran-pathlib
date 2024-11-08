#include <string>
#include <string_view>

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#else
#define CCP_WIN_A_TO_POSIX 0
#define CCP_POSIX_TO_WIN_A 0
#endif

#include "ffilesystem.h"


static std::string fs_convert_path(std::string_view path, [[maybe_unused]] const int what)
{
#ifdef __CYGWIN__
  const ssize_t L = cygwin_conv_path(what, path.data(), nullptr, 0);
  if(L > 0){
    if (std::string r(L, '\0'); !cygwin_conv_path(what, path.data(), r.data(), L))
      return r;
  }
#endif

  fs_print_error(path, "cygwin_convert_path");
  return {};
}

std::string fs_to_cygpath(std::string_view path) {
  return fs_convert_path(path, CCP_WIN_A_TO_POSIX);
}

std::string fs_to_winpath(std::string_view path) {
  return fs_convert_path(path, CCP_POSIX_TO_WIN_A);
}
