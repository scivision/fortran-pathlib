#include <string>

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

#include "ffilesystem.h"


std::string fs_to_cygpath(std::string_view path){

#ifdef __CYGWIN__
  const auto what = CCP_WIN_A_TO_POSIX;

  const ssize_t L = cygwin_conv_path(what, path.data(), nullptr, 0);
  if(L < 0){
    fs_print_error(path, "cygwin_conv_path:size");
    return {};
  }

  std::string r(L, '\0');

  // this call does not return size
  if(cygwin_conv_path(what, path.data(), r.data(), L)) {
    fs_print_error(path, "cygwin_conv_path");
    return {};
  }

  return r;
#else
    fs_print_error(path, "to_cygpath: only for Cygwin");
    return {};
#endif
}


std::string fs_to_winpath(std::string_view path){

#ifdef __CYGWIN__
  const auto what = CCP_POSIX_TO_WIN_A;

  const ssize_t L = cygwin_conv_path(what, path.data(), nullptr, 0);
  if(L < 0){
    fs_print_error(path, "cygwin_conv_path:size");
    return {};
  }

  std::string r(L, '\0');

  // this call does not return size
  if(cygwin_conv_path(what, path.data(), r.data(), L)) {
    fs_print_error(path, "cygwin_conv_path");
    return {};
  }

  return r;
#else
    fs_print_error(path, "to_cygpath: only for Cygwin");
    return {};
#endif
}
