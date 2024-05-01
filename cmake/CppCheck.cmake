# Check compiler's C++17 capabilities

function(cpp_check)

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
  check_cxx_source_compiles("
  #include <iostream>
  #include <chrono>
  #include <format>
  #include <filesystem>

  int main(){
  std::filesystem::file_time_type t;
  t = std::filesystem::file_time_type::clock::now();
  std::cout << t << std::endl;
  return 0;
  }"
  ${PROJECT_NAME}_format
  )

  check_cxx_symbol_exists(__cpp_lib_ranges "algorithm" ${PROJECT_NAME}_cpp20_ranges)
endif()


if(${PROJECT_NAME}_trace)
  check_cxx_symbol_exists(__cpp_lib_starts_ends_with "string" cpp20_string_ends_with)
  check_cxx_symbol_exists(__cpp_using_enum "" cpp20_using_enum)

  check_cxx_symbol_exists(__cpp_lib_make_unique "memory" cpp14_make_unique)
  if(NOT cpp14_make_unique)
    message(WARNING "C++ compiler lacks C++14 std::make_unique() -- some functions not available")
  endif()

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
