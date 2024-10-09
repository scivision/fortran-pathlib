#include <string>

#if __has_include(<format>)
#include <format>
#endif

#include "ffilesystem.h"


bool fs_is_optimized(){
// This is a heuristic, trusting the build system or user to set NDEBUG if optimized.
#if defined(NDEBUG)
  return true;
#else
  return false;
#endif
}


std::string fs_compiler()
{

  std::string v = {};
#ifdef __cpp_lib_format

#if defined(__INTEL_LLVM_COMPILER)
  v = std::format("Intel LLVM {} {}", __INTEL_LLVM_COMPILER,  __VERSION__);
#elif defined(__NVCOMPILER_LLVM__)
  v = std::format("NVIDIA nvc {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#elif defined(__clang__)
  #ifdef __VERSION__
    v = std::format("Clang {}", __VERSION__);
  #else
    v = std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#elif defined(__GNUC__)
  v = std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
  v = std::format("MSVC {}", _MSC_FULL_VER);
#endif

#endif

  return v;
}
