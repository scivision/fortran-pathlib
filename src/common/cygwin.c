#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

#include "ffilesystem.h"


size_t fs_to_cygpath(FFS_MUNUSED const char* path, FFS_MUNUSED char* result, FFS_MUNUSED const size_t buffer_size){

#ifdef __CYGWIN__
  const cygwin_conv_path_t what = CCP_WIN_A_TO_POSIX;

  const ssize_t L = cygwin_conv_path(what, path, NULL, 0);
  if(L < 0){
    fprintf(stderr, "cygwin_conv_path failed: %s\n", strerror(errno));
    return 0;
  }
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:to_cygpath: buffer_size too small\n");
    return 0;
  }

  // this call does not return size
  if(cygwin_conv_path(what, path, result, L)) {
    fprintf(stderr, "cygwin_conv_path failed: %s\n", strerror(errno));
    return 0;
  }

  if (FS_TRACE) printf("to_cygpath: %s => %s  length %lu\n", path, result, L);

  return (size_t) L;
#else
    fprintf(stderr, "ERROR:to_cygpath: only for Cygwin\n");
    return 0;
#endif
}


size_t fs_to_winpath(FFS_MUNUSED const char* path, FFS_MUNUSED char* result, FFS_MUNUSED const size_t buffer_size){

#ifdef __CYGWIN__
  const cygwin_conv_path_t what = CCP_POSIX_TO_WIN_A;

  const ssize_t L = cygwin_conv_path(what, path, NULL, 0);
  if(L < 0){
    fprintf(stderr, "cygwin_conv_path failed: %s\n", strerror(errno));
    return 0;
  }
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:to_winpath: buffer_size too small\n");
    return 0;
  }

  // this call does not return size
  if(cygwin_conv_path(what, path, result, L) ){
    fprintf(stderr, "cygwin_conv_path failed: %s\n", strerror(errno));
    return 0;
  }

  if (FS_TRACE) printf("to_winpath: %s => %s  length %lu\n", path, result, L);

  fs_as_posix(result);

  return (size_t) L;
#else
    fprintf(stderr, "ERROR:to_winpath: only for Cygwin\n");
    return 0;
#endif
}
