# Check compiler C++ capabilities

function(cpp_check)

set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)

# some compilers claim to have filesystem, but their libstdc++ doesn't have it.
check_source_compiles(CXX
[=[
#include <cstdlib>
#include <filesystem>

static_assert(__cpp_lib_filesystem, "No C++ filesystem support");

namespace fs = std::filesystem;


int main () {
fs::path tgt(".");
auto h = tgt.has_filename();
return EXIT_SUCCESS;
}
]=]
HAVE_CXX_FILESYSTEM
)

if(NOT HAVE_CXX_FILESYSTEM)
  message(WARNING "C++ stdlib filesystem is broken in libstdc++ ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
  return()
endif()

# for mkdtemp
check_source_compiles(CXX
"#include <random>

int main(){
  std::mt19937 mt_rand(0);
  return 0;
}"
ffilesystem_HAVE_MERSENNE_TWISTER
)

if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)

# for fs_get_modtime
check_source_compiles(CXX
"#include <chrono>
#include <filesystem>

int main(){

std::filesystem::file_time_type t_fs;
auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
return 0;
}"
ffilesystem_HAVE_CLOCK_CAST
)
else()
  unset(ffilesystem_HAVE_CLOCK_CAST CACHE)
endif()


if(ffilesystem_trace)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

  check_cxx_symbol_exists(__cpp_lib_format "" cpp20_format)

  check_cxx_symbol_exists(__cpp_lib_starts_ends_with "string" cpp20_string_ends_with)
  check_cxx_symbol_exists(__cpp_using_enum "" cpp20_using_enum)
  check_cxx_symbol_exists(__cpp_deduction_guides "" cpp17_deduction_guides)

  check_cxx_symbol_exists(__has_cpp_attribute "" cpp20_has_cpp_attribute)

  if(cpp20_has_cpp_attribute)

  check_source_compiles(CXX
  "#if !__has_cpp_attribute(likely)
  #error \"no likely attribute\"
  #endif"
  cpp20_likely
  )

  endif()
endif()

endfunction()
