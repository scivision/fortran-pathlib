include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

include(${CMAKE_CURRENT_LIST_DIR}/CppCheck.cmake)

unset(CMAKE_REQUIRED_FLAGS)
# --- some compilers require these manual settings
unset(CMAKE_REQUIRED_LIBRARIES)
unset(CMAKE_REQUIRED_DEFINITIONS)
unset(GNU_stdfs)

if( (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0") OR
    (LINUX AND CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "23") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "23.11") )
  set(GNU_stdfs stdc++fs stdc++)
endif()
# GCC < 9.1 needs -lstdc++ to avoid C main program link error
# NVHPC at least 23.11 and newer doesn't need the flags, but at least 23.5 and older do.
# INtel oneAPI 2021.1 and older needs, but 2023 and newer doesn't. (not sure about 2022)

if(GNU_stdfs)
  set(CMAKE_REQUIRED_LIBRARIES ${GNU_stdfs})
  message(STATUS "adding library ${GNU_stdfs}")
endif()

if(ffilesystem_cpp)
  cpp_check()
else()
  unset(HAVE_CXX_FILESYSTEM CACHE)

  check_symbol_exists(__has_include "" c23_has_include)
  check_symbol_exists(__has_c_attribute "" c23_has_c_attribute)
  if(c23_has_c_attribute)
    check_source_compiles(C
    "#if !__has_c_attribute(maybe_unused)
    #error \"no maybe_unused\"
    #endif
    int main(void) { return 0; }"
    c23_maybe_unused)

    check_source_compiles(C
    "#if !__has_c_attribute(nodiscard)
    #error \"no nodiscard\"
    #endif
    int main(void) { return 0; }"
    c23_nodiscard)

    # [[reproducible]] [[unsequenced]] support by compilers:
    # https://en.cppreference.com/w/c/compiler_support/23

    # check_source_compiles(C
    # "#if !__has_c_attribute(reproducible)
    # #error \"no reproducible\"
    # #endif
    # int main(void) { return 0; }"
    # c23_reproducible)

    # check_source_compiles(C
    # "#if !__has_c_attribute(unsequenced)
    # #error \"no unsequenced\"
    # #endif
    # int main(void) { return 0; }"
    # c23_unsequenced)
  endif()
endif()

# --- deeper filesystem check: C, C++ and Fortran compiler ABI compatibility

if(HAVE_CXX_FILESYSTEM)

include(${CMAKE_CURRENT_LIST_DIR}/FScheck.cmake)
fs_check()

endif(HAVE_CXX_FILESYSTEM)

if(ffilesystem_cpp AND NOT ffilesystem_fallback AND NOT HAVE_CXX_FILESYSTEM)
  message(FATAL_ERROR "C++ filesystem not available. To fallback to C filesystem:
  cmake -Dffilesystem_fallback=on -B build"
  )
endif()

# fixes errors about needing -fPIE
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(CheckPIESupported)
  check_pie_supported()
  set(CMAKE_POSITION_INDEPENDENT_CODE true)
endif()


if(HAVE_Fortran_FILESYSTEM)

check_source_compiles(Fortran
"module dummy
type :: path_t
private
character(:), allocatable :: path_str
contains
final :: destructor
end type

interface path_t
  module procedure set_path
end interface

contains

subroutine destructor(self)
type(path_t), intent(inout) :: self
if(allocated(self%path_str)) deallocate(self%path_str)
end subroutine destructor

type(path_t) function set_path(path)
character(*), intent(in) :: path
allocate(character(100) :: set_path%path_str)
set_path%path_str = path
end function set_path

end module dummy

program main
use dummy
implicit none
type(path_t) :: p
p = path_t('.')
end program"
HAVE_F03TYPE
)

endif()

# --- END COMPILER CHECKS


# --- C compile flags
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU|^Intel")
  add_compile_options(
  "$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:Debug>>:-Wextra>"
  "$<$<COMPILE_LANGUAGE:C,CXX>:-Wall>"
  "$<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>"
  )
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
  add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:/W3;/wd4996>")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "IntelLLVM")
  add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:Debug>>:-Rno-debug-disables-optimization>")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wno-attributes>")
  # this is for UNLIKELY/LIKELY macros
endif()

# --- Fortran compile flags
if(CMAKE_Fortran_COMPILER_ID MATCHES "^Intel")

add_compile_options(
"$<$<COMPILE_LANGUAGE:Fortran>:-warn>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-traceback;-check;-debug>"
)

# this flag needs to be applied EVERYWHERE incl. submodule projects
# or runtime errors / weird behavior with unresolved procedures that actually exist.
# -standard-semantics is no good because it breaks linkage within oneAPI itself e.g. oneMPI library!
if(NOT WIN32)
  add_compile_options("$<$<COMPILE_LANGUAGE:Fortran>:-fpscomp;logicals>")
endif()

elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")

add_compile_options(
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-Wextra>"
"$<$<COMPILE_LANGUAGE:Fortran>:-Wall;-fimplicit-none>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-fcheck=all;-Werror=array-bounds>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<NOT:$<CONFIG:Debug>>>:-fno-backtrace>"
)

elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "NVHPC")
  # C_BOOL correctness
  add_compile_options("$<$<COMPILE_LANGUAGE:Fortran>:-Munixlogical>")
endif()

# --- code coverage
if(${PROJECT_NAME}_coverage)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDES ${PROJECT_SOURCE_DIR}/src/tests)
endif()

# --- clang-tidy
if(${PROJECT_NAME}_tidy)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED
  PATHS /opt/homebrew/opt/llvm/bin
  )
  set(tidy_cmd ${CLANG_TIDY_EXE} -format-style=file)
  set(CMAKE_C_CLANG_TIDY ${tidy_cmd})
  set(CMAKE_CXX_CLANG_TIDY ${tidy_cmd})
endif()

# --- IWYU
if(${PROJECT_NAME}_iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  message(STATUS "IWYU_EXE: ${IWYU_EXE}")
  set(iwyu_cmd ${IWYU_EXE})
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
endif()
