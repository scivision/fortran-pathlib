# Check compiler C++ capabilities

function(cpp_check)

# use 17 for the essential filesystem test to avoid compiler
# with shaky C++20 from breaking essential test
set(CMAKE_CXX_STANDARD 17)

# some compilers e.g. Cray claim to have filesystem, but their libstdc++ doesn't have it.
check_cxx_source_compiles([=[
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

set(CMAKE_CXX_STANDARD 20)

# e.g. AppleClang 15 doesn't yet have this, maybe not worth the bother
# i.e. benchmarking may reveal miniscule benefit.
# check_cxx_symbol_exists(__cpp_lib_smart_ptr_for_overwrite "memory" cpp20_smart_ptr_for_overwrite)

# for Ffs::get_modtime
check_cxx_source_compiles("
#include <chrono>
#include <filesystem>

int main(){

std::filesystem::file_time_type t_fs;
auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
return 0;
}"
${PROJECT_NAME}_HAVE_CLOCK_CAST
)

if(${PROJECT_NAME}_cli)
  check_cxx_source_compiles("#include <map>
   int main(){
    std::map<int, char> m{{1, 'a'}, {2, 'b'}};
    return m.contains(2);
   }"
  ${PROJECT_NAME}_cpp_map_contains
  )
endif()


if(${PROJECT_NAME}_trace)
  check_cxx_symbol_exists(__cpp_lib_starts_ends_with "string" cpp20_string_ends_with)
  check_cxx_symbol_exists(__cpp_using_enum "" cpp20_using_enum)
  check_cxx_symbol_exists(__cpp_deduction_guides "" cpp17_deduction_guides)

  check_cxx_symbol_exists(__has_cpp_attribute "" cpp20_has_cpp_attribute)

  if(cpp20_has_cpp_attribute)

  check_cxx_source_compiles("#if !__has_cpp_attribute(likely)
  #error \"no likely attribute\"
  #endif
  int main(){ return 0; }"
  cpp20_likely
  )

  endif()
endif()

endfunction()
