#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include <stdlib.h>

#ifdef _WIN32
#include <io.h>  // _mktemp
#else
#include <unistd.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "ffilesystem.h"

#ifndef HAVE_MERSENNE_TWISTER
size_t fs_mkdtemp(char* result, const size_t buffer_size)
{
  char tmpl[] = "tmp.XXXXXX";
  const char* tmp;

#ifdef _WIN32
  tmp = _mktemp(tmpl);
  if(!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_mkdtemp:_mktemp: could not generate tempdir name %s\n", strerror(errno));
    return 0;
  }
  if(!fs_get_tempdir(result, buffer_size))
    return 0;

  const size_t L = fs_join(result, tmp, result, buffer_size);
  if(L == 0)
    return 0;

  fs_mkdir(result);
  return L;
#else
  tmp = mkdtemp(tmpl);
  /* Linux: stdlib.h  macOS: unistd.h */
  if (!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_mkdtemp:mkdtemp: could not create temporary directory %s\n", strerror(errno));
    return 0;
  }

  return fs_strncpy(tmp, result, buffer_size);
#endif

}
#endif
