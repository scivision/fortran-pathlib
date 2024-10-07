#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>  // mkdir
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h> // _mkdir
#ifndef mkdir
#define mkdir(dir, mode) _mkdir(dir)
#endif
#ifndef S_IRWXU
#define S_IRWXU (_S_IREAD | _S_IWRITE | _S_IEXEC)
#endif
#else
#include <unistd.h>
#endif

#include "ffilesystem.h"


bool fs_mkdir(const char* path)
{

  if(fs_is_dir(path))
    return true;

  const size_t m = fs_get_max_path();

  char *buf = (char*)malloc(m);
  if(!buf)
    return false;

  size_t L = fs_normal(path, buf, m);
  if(L == 0){
    free(buf);
    return false;
  }

  char *seg = (char*)malloc(m);
  if(!seg){
    free(buf);
    return false;
  }

  // iterate over path components using strchr and mkdir each segment
  char *p = buf;
  while((p = strchr(p, '/'))){
    L = p - buf;
    if(L == 0){
      p++;
      continue;
    }

    strncpy(seg, buf, L);
    seg[L] = '\0';

    if(mkdir(seg, S_IRWXU) && !(errno == EEXIST || errno == EACCES)) {
      fprintf(stderr, "ERROR:ffilesystem:mkdir(%s) %s %u %s\n", path, seg, errno, strerror(errno));
      free(buf);
      free(seg);
      return false;
    }

    p++;
  }

  // Create the last segment
  if (mkdir(buf, S_IRWXU) && !(errno == EEXIST || errno == EACCES)) {
    fprintf(stderr, "ERROR:ffilesystem:mkdir(%s) %s %s\n", path, buf, strerror(errno));
    free(buf);
    free(seg);
    return false;
  }

  free(buf);
  free(seg);

  if(!fs_is_dir(path)){
    fprintf(stderr, "ERROR:ffilesystem:mkdir: %s not created\n", path);
    return false;
  }

  return true;
}
