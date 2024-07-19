#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>


bool fs_is_optimized(){
// This is a heuristic, trusting the build system or user to set NDEBUG if optimized.
#if defined(NDEBUG)
  return true;
#else
  return false;
#endif
}


size_t fs_compiler(char* name, const size_t buffer_size)
{

  name[0] = '\0';

  const int L =
#if defined(__INTEL_LLVM_COMPILER)
  snprintf(name, buffer_size, "Intel LLVM %d %s", __INTEL_LLVM_COMPILER,  __VERSION__);
#elif defined(__NVCOMPILER_LLVM__)
  snprintf(name, buffer_size, "NVIDIA nvc %d.%d.%d", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#elif defined(__clang__)
  #ifdef __VERSION__
    snprintf(name, buffer_size, "Clang %s", __VERSION__);
  #else
    snprintf(name, buffer_size, "Clang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#elif defined(__GNUC__)
  snprintf(name, buffer_size, "GNU GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
  snprintf(name, buffer_size, "MSVC %d", _MSC_FULL_VER);
#else
  0;
#endif

if (L < 0 || L >= (int) buffer_size){
  fprintf(stderr, "ERROR:ffilesystem:fs_compiler: snprintf failed %s\n", strerror(errno));
  return 0;
}

  return (size_t) L;
}
