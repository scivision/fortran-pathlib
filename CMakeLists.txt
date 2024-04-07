cmake_minimum_required(VERSION 3.17...3.29)
# 3.17 for cmake -E rm

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out of source build like:
  cmake -Bbuild")
endif()

project(ffilesystem
LANGUAGES C
VERSION 5.6.0
)

include(CTest) # needed for "ctest -T memcheck"
include(FetchContent)
include(GNUInstallDirs)
include(CheckLanguage)

# some parts of ffilesystem have optional C++20 features.
# almost all ffilesystem functions work with C++17 (emits warnings for unavailable functions)
# ffilesystem does not build at all with C++14 due to C++17 code in every function
#
# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
if(DEFINED ${PROJECT_NAME}_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${${PROJECT_NAME}_CXX_STANDARD})
else()
  set(CMAKE_CXX_STANDARD 20)
endif()
# https://cmake.org/cmake/help/latest/prop_tgt/CXX_EXTENSIONS.html
if(DEFINED ${PROJECT_NAME}_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS ${${PROJECT_NAME}_CXX_EXTENSIONS})
endif()

# https://cmake.org/cmake/help/latest/prop_tgt/C_STANDARD.html
if(DEFINED ${PROJECT_NAME}_C_STANDARD)
  set(CMAKE_C_STANDARD ${${PROJECT_NAME}_C_STANDARD})
else()
  set(CMAKE_C_STANDARD 99)
endif()
# https://cmake.org/cmake/help/latest/prop_tgt/C_EXTENSIONS.html
if(DEFINED ${PROJECT_NAME}_C_EXTENSIONS)
  set(CMAKE_C_EXTENSIONS ${${PROJECT_NAME}_C_EXTENSIONS})
endif()

set(_msg "${PROJECT_NAME} ${PROJECT_VERSION} CMake ${CMAKE_VERSION} ${CMAKE_SYSTEM_NAME} Toolchain ${CMAKE_TOOLCHAIN_FILE}")

include(options.cmake)

if(ffilesystem_cpp OR ffilesystem_bench)
  enable_language(CXX)
endif()

if(ffilesystem_fortran)
  check_language(Fortran)
  if(CMAKE_Fortran_COMPILER)
    enable_language(Fortran)
    set(HAVE_Fortran_FILESYSTEM ON)
  else()
    set(HAVE_Fortran_FILESYSTEM OFF)
  endif()
endif()

message(STATUS "CXX_STANDARD ${CMAKE_CXX_STANDARD}  C_STANDARD ${CMAKE_C_STANDARD}")

include(cmake/compilers.cmake)

# CPPcheck
if(${PROJECT_NAME}_cppcheck)
  find_program(cppcheck_exe NAMES cppcheck REQUIRED)
  set(cppcheck_opts --enable=all --inline-suppr --quiet --suppressions-list=${PROJECT_SOURCE_DIR}/cppcheck.supp)
  set(CMAKE_C_CPPCHECK ${cppcheck_exe} --std=c++${CMAKE_CXX_STANDARD} ${cppcheck_opts})
  set(CMAKE_CXX_CPPCHECK ${cppcheck_exe} --std=c++${CMAKE_CXX_STANDARD} ${cppcheck_opts})
endif()

# --- filesystem library

if(NOT HAVE_CXX_FILESYSTEM)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

  FetchContent_Declare(cwalk
  URL https://github.com/likle/cwalk/archive/refs/tags/v1.2.9.tar.gz
  TLS_VERIFY ${CMAKE_TLS_VERIFY}
  )
  if(NOT cwalk_POPULATED)
    FetchContent_Populate(cwalk)
  endif()
endif()


add_library(ffilesystem src/common/common.c
"$<$<BOOL:${HAVE_Fortran_FILESYSTEM}>:src/common/fortran/filesystem.f90>"
)
if(HAVE_CXX_FILESYSTEM)
  target_sources(ffilesystem PRIVATE
    src/common/filesystem.cpp
    src/common/c_ifc.cpp
    src/common/dynamic.cpp
    src/common/inquire.cpp
    src/common/platform.cpp
    src/common/pure.cpp
    src/common/resolve.cpp
    src/common/symlink.cpp
    src/common/windows.cpp
    )
else()
  target_sources(ffilesystem PRIVATE
  src/c/filesystem.c
  src/c/dynamic.c
  src/c/inquire.c
  src/c/platform.c
  src/c/pure.c
  src/c/resolve.c
  src/c/symlink.c
  ${cwalk_SOURCE_DIR}/src/cwalk.c
  )
endif()
target_include_directories(ffilesystem PRIVATE
${PROJECT_SOURCE_DIR}/include
$<$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>:${cwalk_SOURCE_DIR}/include>
)

target_compile_definitions(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>>:CWK_EXPORTS>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${${PROJECT_NAME}_WIN32_SYMLINK}>>:WIN32_SYMLINK>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${${PROJECT_NAME}_HAVE_CLOCK_CAST}>>:HAVE_CLOCK_CAST>"
"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<BOOL:${ffilesystem_trace}>>:FS_TRACE=1>"
)
target_compile_options(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANG_AND_ID:C,AppleClang,Clang,GNU>,$<NOT:$<BOOL:${c23_maybe_unused}>>>:-Wno-unused-parameter>"
)
set_target_properties(ffilesystem PROPERTIES
PUBLIC_HEADER include/ffilesystem.h
EXPORT_NAME filesystem
Fortran_MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/include
VERSION ${PROJECT_VERSION}
WINDOWS_EXPORT_ALL_SYMBOLS ON
)
if(BUILD_SHARED_LIBS AND (WIN32 OR CYGWIN))
  target_compile_definitions(ffilesystem PRIVATE FS_DLL_NAME="$<TARGET_FILE_NAME:ffilesystem>")
endif()

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include)

target_include_directories(ffilesystem PUBLIC
"$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include;${PROJECT_SOURCE_DIR}/include>"
$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ffilesystem PUBLIC
${GNU_stdfs}
${CMAKE_DL_LIBS}
"$<$<BOOL:${WIN32}>:Userenv;Advapi32>"
)
# both Advapi32 and Userenv must be linked

if(HAVE_Fortran_FILESYSTEM)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/filesystem.mod TYPE INCLUDE)
endif()

install(TARGETS ffilesystem EXPORT ${PROJECT_NAME}-targets)

# GLOBAL for use from FetchContent
add_library(ffilesystem::filesystem INTERFACE IMPORTED GLOBAL)

target_link_libraries(ffilesystem::filesystem INTERFACE ffilesystem)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)
  set_property(TARGET ffilesystem::filesystem PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGE CXX)
  # https://cmake.org/cmake/help/latest/prop_tgt/IMPORTED_LINK_INTERFACE_LANGUAGES.html
  # imported targets use above instead of LINKER_LANGUAGE
  # target_link_libraries(ffilesystem::filesystem INTERFACE stdc++)  # did not help
endif()

# --- CLI

if(ffilesystem_cli)
  if(HAVE_CXX_FILESYSTEM AND ${PROJECT_NAME}_format AND ${PROJECT_NAME}_cpp20_ranges)
    add_executable(fs_cli app/main.cpp)
    target_link_libraries(fs_cli PRIVATE ffilesystem)

    install(TARGETS fs_cli EXPORT ${PROJECT_NAME}-targets)
  endif()

  if(HAVE_Fortran_FILESYSTEM)
  add_executable(filesystem_cli app/fortran/main.f90)
  target_link_libraries(filesystem_cli PRIVATE ffilesystem)
  set_property(TARGET filesystem_cli PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set_property(TARGET filesystem_cli PROPERTY LINKER_LANGUAGE Fortran)
  # Intel/NVHPC needs linker_language Fortran else error "undefined reference to `main'"

  install(TARGETS filesystem_cli EXPORT ${PROJECT_NAME}-targets)
  endif()
endif()

# --- Tests
if(${PROJECT_NAME}_BUILD_TESTING)
  add_subdirectory(test)
endif()

include(cmake/install.cmake)

include(FeatureSummary)

add_feature_info(Fortran HAVE_Fortran_FILESYSTEM "Fortran bindings")
add_feature_info(Cpp HAVE_CXX_FILESYSTEM "C++ stdlib filesystem")
add_feature_info(shared BUILD_SHARED_LIBS "Build shared library: enables lib_path()")

feature_summary(WHAT ENABLED_FEATURES DESCRIPTION "${PROJECT_NAME} enabled features:")
