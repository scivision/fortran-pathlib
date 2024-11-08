#include <string>

#if __has_include(<format>)
#include <format> // IWYU pragma: keep
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
  v = std::format("{} {}", __VERSION__, __INTEL_LLVM_COMPILER);
#else
  v = std::string(__VERSION__) + " " + std::to_string(__INTEL_LLVM_COMPILER);
#endif

#elif defined(__NVCOMPILER_LLVM__)

// not __VERSION__ because it emits "EDG g++ mode" instead of "NVIDIA"

#ifdef __cpp_lib_format
  v = std::format("NVIDIA {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#else
  v = "NVIDIA " + std::to_string(__NVCOMPILER_MAJOR__) + "." + std::to_string(__NVCOMPILER_MINOR__) + "." + std::to_string(__NVCOMPILER_PATCHLEVEL__);
#endif

#elif defined(__clang__)

# ifdef __VERSION__
  v = __VERSION__;
# elif defined(__cpp_lib_format)
  v = std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
# else
  v = "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
# endif

#elif defined(__GNUC__)

# ifdef __cpp_lib_format
v = std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
# else
v = "GNU GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
# endif

#elif defined(_MSC_VER)
  v = "MSVC " + std::to_string(_MSC_FULL_VER);
#endif

  return v;
}
