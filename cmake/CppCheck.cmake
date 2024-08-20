# Check compiler C++ capabilities

function(cpp_check)

set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)

set(CMAKE_CXX_STANDARD 20)

# some compilers e.g. Cray claim to have filesystem, but their libstdc++ doesn't have it.
check_source_compiles(CXX
[=[
#include <cstdlib>

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std {
  namespace filesystem = experimental::filesystem;
}
#else
#error "No C++ <filesystem> header available."
#endif


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
  message(WARNING "C++ stdlib filesystem not detected: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
  return()
endif()

# for Ffs::get_modtime
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

if(ffilesystem_cli)
  check_source_compiles(CXX
  "#include <map>
   int main(){
    std::map<int, char> m{{1, 'a'}, {2, 'b'}};
    return m.contains(2);
   }"
  ffilesystem_cpp_map_contains
  )
endif()


if(ffilesystem_trace)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

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
