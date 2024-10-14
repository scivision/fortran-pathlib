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
#if defined(__INTEL_LLVM_COMPILER)

#ifdef __cpp_lib_format
  v = std::format("Intel LLVM {} {}", __INTEL_LLVM_COMPILER,  __VERSION__);
#else
  v = "Intel LLVM " + std::to_string(__INTEL_LLVM_COMPILER) + " " + std::string(__VERSION__);
#endif

#elif defined(__NVCOMPILER_LLVM__)

#ifdef __cpp_lib_format
  v = std::format("NVIDIA nvc {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#else
  v = "NVIDIA nvc " + std::to_string(__NVCOMPILER_MAJOR__) + "." + std::to_string(__NVCOMPILER_MINOR__) + "." + std::to_string(__NVCOMPILER_PATCHLEVEL__);
#endif

#elif defined(__clang__)

#ifdef __cpp_lib_format
  #ifdef __VERSION__
    v = std::format("Clang {}", __VERSION__);
  #else
    v = std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#else
  #ifdef __VERSION__
    v = "Clang " + std::string(__VERSION__);
  #else
    v = "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
  #endif
#endif

#elif defined(__GNUC__)

#ifdef __cpp_lib_format
v = std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
v = "GNU GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
#endif

#elif defined(_MSC_VER)
  v = "MSVC " + std::to_string(_MSC_FULL_VER);
#endif

  return v;
}
