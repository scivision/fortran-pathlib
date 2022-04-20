include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)
include(CheckSourceRuns)
include(CheckSourceCompiles)

# check if Fortran compiler new enough
check_source_compiles(Fortran "
module a
implicit none (type, external)

interface
module subroutine d()
end subroutine d
end interface
end module

submodule (a) b
contains
module procedure d
end procedure d
end submodule

program c
use a, only : d

character :: e
error stop e
end program
"
HAS_Fortran_2018
)
if(NOT HAS_Fortran_2018)
  message(FATAL_ERROR "Fortran compiler ${CMAKE_Fortran_COMPILER_ID} ${CMAKE_Fortran_COMPILER_VERSION} does not support Fortran 2018 syntax")
endif()

# setup / check C++ filesystem

set(libfs)

if(fallback)

  unset(HAVE_CXX_FILESYSTEM CACHE)
  unset(HAVE_CXX_EXPERIMENTAL_FILESYSTEM CACHE)

else()
  if(CMAKE_CXX_COMPILER_ID STREQUAL GNU AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1.0)
    set(libfs stdc++fs)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0.0)
  # https://releases.llvm.org/9.0.0/projects/libcxx/docs/UsingLibcxx.html#using-filesystem
    # set(libfs c++fs)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "^Intel" AND CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(libfs stdc++)
  endif()

  set(CMAKE_REQUIRED_LIBRARIES ${libfs})

  if(MSVC)
    set(CMAKE_REQUIRED_FLAGS /std:c++17)
  else()
    set(CMAKE_REQUIRED_FLAGS -std=c++17)
  endif()

  check_cxx_symbol_exists(__has_include "" HAVE_INCLUDE_MACRO)
  if(NOT HAVE_INCLUDE_MACRO)
    message(FATAL_ERROR "C++ compiler not C++17 capable ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
  endif()

  check_cxx_symbol_exists(__cpp_lib_filesystem filesystem HAVE_CXX_FILESYSTEM)
  if(NOT HAVE_CXX_FILESYSTEM)
    check_include_file_cxx(experimental/filesystem HAVE_CXX_EXPERIMENTAL_FILESYSTEM)
  endif()

endif()

if(HAVE_CXX_FILESYSTEM)
  message(STATUS "C++ filesystem support enabled.")
elseif(HAVE_CXX_EXPERIMENTAL_FILESYSTEM)
  message(STATUS "Using deprecated experimental/filesystem, some functionality is limited.")
else()
  message(STATUS "C++ filesystem support is not available with ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
  enabling fallback filesystem library with limited functionality."
  )
  set(fallback true)
endif()

if(NOT fallback)
  # some compilers e.g. Cray claim to have filesystem, but their libstdc++ doesn't have it.
  check_source_compiles(CXX [=[
  #if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
  #elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
  #else
  #error "No C++ filesystem support"
  #endif

  int main () {
  fs::path tgt(".");
  auto h = tgt.has_filename();
  return EXIT_SUCCESS;
  }
  ]=]
  HAVE_FILESYSTEM_STDLIB
  )

  if(NOT HAVE_FILESYSTEM_STDLIB)
    message(STATUS "C++ compiler has filesystem, but filesystem is missing from libstdc++ ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
    Using fallback with limited functionality.")
    set(fallback true)
  endif()

endif()

if(NOT fallback)

  # C++ filesystem or C lstat() symbolic link information
  file(READ ${CMAKE_CURRENT_LIST_DIR}/check_fs_symlink.cpp symlink_src)
  check_source_runs(CXX "${symlink_src}" HAVE_SYMLINK)

endif()

# fixes errors about needing -fPIE
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|Intel)")
    set(CMAKE_POSITION_INDEPENDENT_CODE true)
  elseif(BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_POSITION_INDEPENDENT_CODE true)
  endif()
endif()

# --- flags

if(CMAKE_Fortran_COMPILER_ID MATCHES "^Intel")

add_compile_options(
"$<$<COMPILE_LANGUAGE:Fortran>:-warn;-heap-arrays>"
"$<$<COMPILE_LANGUAGE:C,CXX>:-Wall>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug,RelWithDebInfo>>:-traceback;-check;-debug>"
)

elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")

add_compile_options(
-Wall
$<$<CONFIG:Debug,RelWithDebInfo>:-Wextra>
"$<$<COMPILE_LANGUAGE:Fortran>:-fimplicit-none>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug,RelWithDebInfo>>:-fcheck=all;-Werror=array-bounds>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Release>>:-fno-backtrace>"
)

add_compile_options($<$<COMPILE_LANGUAGE:Fortran>:-Wno-maybe-uninitialized>)
# spurious warning on character(:), allocatable :: C

add_compile_options($<$<COMPILE_LANGUAGE:Fortran>:-Wno-uninitialized>)
# spurious warning on character(:), allocatable :: C(:)

endif()

# --- code coverage
if(ENABLE_COVERAGE)
include(CodeCoverage)
append_coverage_compiler_flags()
set(COVERAGE_EXCLUDES ${PROJECT_SOURCE_DIR}/src/tests)
endif()
