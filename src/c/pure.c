#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h>
#include <ctype.h> // isalpha


size_t fs_with_suffix(const char* path, const char* suffix,
                      char* result, const size_t buffer_size)
{
  const size_t Lf = strlen(suffix);

  char* bp = (char*) malloc(buffer_size);
  if(!bp) return 0;

  size_t Lp;
  size_t L = strlen(path);
  if(L)
    Lp = fs_parent(path, bp, buffer_size);
  else
    Lp = 0;

  char* bs = (char*) malloc(buffer_size);
  if(!bs){
    free(bp);
    return 0;
  }
  const size_t Ls = fs_stem(path, bs, buffer_size);

  if(Lf + L + 1 >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_with_suffix: buffer_size %zu too small for string\n", buffer_size);
    free(bp);
    free(bs);
    return 0;
  }

  if(FS_TRACE) printf("Lp=%zu Ls=%zu Lf=%zu L=%zu buffer_size=%zu bp=%s bs=%s suffix=%s\n", Lp, Ls, Lf, L, buffer_size, bp, bs, suffix);

  if(Lp){
    if(Ls)
      L = fs_join(bp, bs, result, buffer_size);
    else
      L = fs_strncpy(bp, result, buffer_size);
  }
  else if (Ls)
    L = fs_strncpy(bs, result, buffer_size);
  else if (Lf){
    free(bp);
    free(bs);
    return fs_strncpy(suffix, result, buffer_size);
  } else
    L = 0;

  free(bs);
  free(bp);

  if(L == 0 || Lf == 0)
    return L;

  strcat(result, suffix);
  return strlen(result);
}
