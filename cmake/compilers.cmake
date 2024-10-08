include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

# --- some compilers require these manual settings
unset(CMAKE_REQUIRED_FLAGS)
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
  include(${CMAKE_CURRENT_LIST_DIR}/CppCheck.cmake)
  cpp_check()
else()
  unset(HAVE_CXX_FILESYSTEM CACHE)
  include(${CMAKE_CURRENT_LIST_DIR}/CCheck.cmake)
  c_check()
endif()

if(UNIX)
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS})
  set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_symbol_exists(dladdr "dlfcn.h" ffilesystem_HAVE_DLADDR)

  set(CMAKE_REQUIRED_DEFINITIONS -D_DEFAULT_SOURCE)
  check_symbol_exists(getloadavg "stdlib.h" ffilesystem_HAVE_GETLOADAVG)

  unset(CMAKE_REQUIRED_DEFINITIONS)
endif()

# --- deeper filesystem check: C, C++ and Fortran compiler ABI compatibility

if(HAVE_CXX_FILESYSTEM)

include(${CMAKE_CURRENT_LIST_DIR}/FScheck.cmake)
fs_check()

endif(HAVE_CXX_FILESYSTEM)

if(ffilesystem_cpp AND NOT ffilesystem_fallback AND NOT HAVE_CXX_FILESYSTEM)
  message(FATAL_ERROR "C++ filesystem not available. To fallback to plain C++:
  cmake -Dffilesystem_fallback=on -B build"
  )
endif()

# --- compiler standard setting
set(cxx_std 17)
# https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_COMPILE_FEATURES.html
if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  set(cxx_std 20)
elseif(HAVE_CXX_FILESYSTEM AND NOT "cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  message(WARNING "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} may not support at least C++17 standard")
endif()

if(CMAKE_VERSION VERSION_LESS 3.25 AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LINUX true)
endif()

# fixes errors about needing -fPIE
if(LINUX)
  include(CheckPIESupported)
  check_pie_supported()
  set(CMAKE_POSITION_INDEPENDENT_CODE true)
endif()

if(HAVE_Fortran_FILESYSTEM)
  include(${CMAKE_CURRENT_LIST_DIR}/FortranCheck.cmake)
  fortran_check()
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
if(ffilesystem_coverage)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDES ${PROJECT_SOURCE_DIR}/src/tests)
endif()

# --- clang-tidy
if(ffilesystem_tidy)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED
  PATHS /opt/homebrew/opt/llvm/bin
  )
  set(tidy_cmd ${CLANG_TIDY_EXE} -format-style=file)
  set(CMAKE_C_CLANG_TIDY ${tidy_cmd})
  set(CMAKE_CXX_CLANG_TIDY ${tidy_cmd})
endif()

# --- IWYU
if(ffilesystem_iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  message(STATUS "IWYU_EXE: ${IWYU_EXE}")
  set(iwyu_cmd ${IWYU_EXE})
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
endif()
