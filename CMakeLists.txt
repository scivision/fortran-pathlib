cmake_minimum_required(VERSION 3.20...3.30)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out of source build like:
  cmake -Bbuild")
endif()

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

project(ffilesystem
LANGUAGES C
VERSION 5.7.1
)

include(CTest) # needed for "ctest -T memcheck"
include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
# This project isn't using C++ modules; avoid bugs in module scanning CMake 3.28, 3.29 with GCC >= 14.

include(options.cmake)

if(ffilesystem_cpp OR ffilesystem_bench)
  enable_language(CXX)
endif()

if(ffilesystem_fortran)
    enable_language(Fortran)
    set(HAVE_Fortran_FILESYSTEM ON)
else()
    set(HAVE_Fortran_FILESYSTEM OFF)
endif()

include(cmake/compilers.cmake)

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} C++${cxx_std} CMake ${CMAKE_VERSION} ${CMAKE_SYSTEM_NAME} Toolchain ${CMAKE_TOOLCHAIN_FILE}")

# CPPcheck
if(ffilesystem_cppcheck)
  find_program(cppcheck_exe NAMES cppcheck REQUIRED)
  set(cppcheck_opts --enable=all --inline-suppr --quiet --suppressions-list=${PROJECT_SOURCE_DIR}/cppcheck.supp)
  set(CMAKE_C_CPPCHECK ${cppcheck_exe} --std=c++20 ${cppcheck_opts})
  set(CMAKE_CXX_CPPCHECK ${cppcheck_exe} --std=c++20 ${cppcheck_opts})
endif()

# --- filesystem library

if(NOT HAVE_CXX_FILESYSTEM)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

  FetchContent_Populate(cwalk
  URL https://github.com/likle/cwalk/archive/refs/tags/v1.2.9.tar.gz
  )
endif()

# use separate object for Fortran to work with Visual Studio generator
if(HAVE_Fortran_FILESYSTEM)

add_library(ffilesystem_Fortran OBJECT src/common/fortran/filesystem.F90)

target_compile_definitions(ffilesystem_Fortran PRIVATE
$<$<NOT:$<BOOL:${HAVE_F03TYPE}>>:NO_F03TYPE>
)
set_property(TARGET ffilesystem_Fortran PROPERTY
Fortran_MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/include
)

endif()


add_library(ffilesystem src/common/common.c
src/common/compiler.c
src/common/cygwin.c
src/common/exepath.c
src/common/env.c
src/common/home.c
src/common/libpath.c
src/common/os.c
src/common/owner.c
src/common/partition.c
src/common/touch.c
src/common/uid.c
src/common/uname.c
src/common/which.c
src/common/windows.c
$<$<TARGET_EXISTS:ffilesystem_Fortran>:$<TARGET_OBJECTS:ffilesystem_Fortran>>
)
if(HAVE_CXX_FILESYSTEM)
  target_sources(ffilesystem PRIVATE
    src/common/copy.cpp
    src/common/filesystem.cpp
    src/common/c_ifc.cpp
    src/common/ifc.cpp
    src/common/inquire.cpp
    src/common/mkdir.cpp
    src/common/platform.cpp
    src/common/pure.cpp
    src/common/resolve.cpp
    src/common/symlink.cpp
    src/common/time.cpp
    )
else()
  target_sources(ffilesystem PRIVATE
  src/c/copy.c
  src/c/filesystem.c
  src/c/inquire.c
  src/c/mkdir.c
  src/c/platform.c
  src/c/pure.c
  src/c/resolve.c
  src/c/symlink.c
  src/c/time.c
  ${cwalk_SOURCE_DIR}/src/cwalk.c
  )
endif()
target_include_directories(ffilesystem PRIVATE
${PROJECT_SOURCE_DIR}/include
$<$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>:${cwalk_SOURCE_DIR}/include>
)

target_compile_definitions(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>>:CWK_EXPORTS>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_WIN32_SYMLINK}>>:WIN32_SYMLINK>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_HAVE_CLOCK_CAST}>>:HAVE_CLOCK_CAST>"
"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<BOOL:${ffilesystem_trace}>>:FS_TRACE=1>"
)
target_compile_features(ffilesystem PRIVATE cxx_std_${cxx_std} c_std_99)
target_compile_options(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANG_AND_ID:C,AppleClang,Clang,GNU>,$<NOT:$<BOOL:${c23_maybe_unused}>>>:-Wno-unused-parameter>"
)
set_target_properties(ffilesystem PROPERTIES
PUBLIC_HEADER include/ffilesystem.h
EXPORT_NAME filesystem
VERSION ${PROJECT_VERSION}
WINDOWS_EXPORT_ALL_SYMBOLS ON
)
if(WIN32 OR CYGWIN)
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
"$<$<BOOL:${WIN32}>:Ole32;Shell32;uuid;Userenv;Advapi32>"
)
# both Advapi32 and Userenv must be linked
# Ole32 is needed for CoTaskMemFree
# Shell32: SHGetKnownFolderPath
# uuid is needed for KnownFolderID

get_property(_ffs_defs TARGET ffilesystem PROPERTY COMPILE_DEFINITIONS)
message(VERBOSE "Ffilesystem COMPILE_DEFINITIONS: ${_ffs_defs}")
get_property(_ffs_flags TARGET ffilesystem PROPERTY COMPILE_OPTIONS)
message(VERBOSE "Ffilesystem COMPILE_OPTIONS: ${_ffs_flags}")
get_property(_ffs_libs TARGET ffilesystem PROPERTY LINK_LIBRARIES)
message(VERBOSE "Ffilesystem LINK_LIBRARIES: ${_ffs_libs}")

if(HAVE_Fortran_FILESYSTEM)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/filesystem.mod TYPE INCLUDE)
endif()

install(TARGETS ffilesystem EXPORT ${PROJECT_NAME}-targets)

# GLOBAL for use from FetchContent
add_library(ffilesystem::filesystem INTERFACE IMPORTED GLOBAL)

target_link_libraries(ffilesystem::filesystem INTERFACE ffilesystem)
set_property(TARGET ffilesystem::filesystem PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGE CXX)
# https://cmake.org/cmake/help/latest/prop_tgt/IMPORTED_LINK_INTERFACE_LANGUAGES.html
# imported targets use above instead of LINKER_LANGUAGE
# target_link_libraries(ffilesystem::filesystem INTERFACE stdc++)  # did not help


# --- CLI

if(ffilesystem_cli)
  if(HAVE_CXX_FILESYSTEM AND ffilesystem_cpp_map_contains AND cxx_std GREATER_EQUAL 20)
    add_executable(fs_cli app/main.cpp)
    target_link_libraries(fs_cli PRIVATE ffilesystem)
    target_compile_features(fs_cli PRIVATE cxx_std_${cxx_std})

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
if(ffilesystem_BUILD_TESTING)
  add_subdirectory(test)
endif()

include(cmake/install.cmake)

include(FeatureSummary)

add_feature_info(Fortran HAVE_Fortran_FILESYSTEM "Fortran bindings")
add_feature_info(Cpp HAVE_CXX_FILESYSTEM "C++ stdlib filesystem")
add_feature_info(shared BUILD_SHARED_LIBS "Build shared library: enables lib_path()")
add_feature_info(path_t HAVE_F03TYPE "Fortran 2003 path_t object-oriented interface")

feature_summary(WHAT ENABLED_FEATURES DESCRIPTION "${PROJECT_NAME} enabled features:")
