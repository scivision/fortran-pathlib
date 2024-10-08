cmake_minimum_required(VERSION 3.20...3.30)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out of source build like:
  cmake -Bbuild")
endif()

project(ffilesystem
LANGUAGES C CXX
VERSION 5.8.0
)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30.0 AND CMAKE_VERSION VERSION_LESS 3.30.4)
  cmake_policy(SET CMP0168 OLD)
  # workaround to stop repeat FetchContent download on each configure for CMake 3.30.0..3.30.3
endif()

include(CTest) # needed for "ctest -T memcheck"
include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
# This project isn't using C++ modules; avoid bugs in module scanning CMake 3.28, 3.29 with GCC >= 14.

include(options.cmake)

if(ffilesystem_fortran)
    enable_language(Fortran)
    set(HAVE_Fortran_FILESYSTEM ON)
else()
    set(HAVE_Fortran_FILESYSTEM OFF)
endif()

include(cmake/compilers.cmake)

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} C++${cxx_std} CMake ${CMAKE_VERSION} ${CMAKE_SYSTEM_NAME} Toolchain ${CMAKE_TOOLCHAIN_FILE}")
message(VERBOSE "CMAKE_AR ${CMAKE_AR}
CMAKE_RANLIB ${CMAKE_RANLIB}
CMAKE_STRIP ${CMAKE_STRIP}")

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

include(cmake/json_array.cmake)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/sources.json json)

# use separate object for Fortran to work with Visual Studio generator
if(HAVE_Fortran_FILESYSTEM)

  json_array(${json} "fortran" src_fortran)
  add_library(ffilesystem_Fortran OBJECT ${src_fortran})

  target_compile_definitions(ffilesystem_Fortran PRIVATE
  $<$<NOT:$<BOOL:${HAVE_F03TYPE}>>:NO_F03TYPE>
  )
  set_property(TARGET ffilesystem_Fortran PROPERTY
  Fortran_MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/include
  )

endif()

json_array(${json} "common" src_common)
add_library(ffilesystem ${src_common}
$<$<TARGET_EXISTS:ffilesystem_Fortran>:$<TARGET_OBJECTS:ffilesystem_Fortran>>
)
if(HAVE_CXX_FILESYSTEM)
  json_array(${json} "cpp" src_cpp)
  target_sources(ffilesystem PRIVATE ${src_cpp})
else()
  json_array(${json} "c" src_c)
  target_sources(ffilesystem PRIVATE ${cwalk_SOURCE_DIR}/src/cwalk.c ${src_c})
endif()
target_include_directories(ffilesystem PRIVATE
${PROJECT_SOURCE_DIR}/include
$<$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>:${cwalk_SOURCE_DIR}/include>
)

target_compile_definitions(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>>:CWK_EXPORTS>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_WIN32_SYMLINK}>>:WIN32_SYMLINK>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_HAVE_CLOCK_CAST}>>:HAVE_CLOCK_CAST>"
"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<BOOL:${ffilesystem_HAVE_MERSENNE_TWISTER}>>:HAVE_MERSENNE_TWISTER>"
"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<BOOL:${ffilesystem_trace}>>:FS_TRACE=1>"
"$<$<AND:$<COMPILE_LANGUAGE:C>,$<BOOL:${ffilesystem_HAVE_DLADDR}>>:HAVE_DLADDR>"
"$<$<AND:$<COMPILE_LANGUAGE:C>,$<BOOL:${ffilesystem_HAVE_GETLOADAVG}>>:HAVE_GETLOADAVG>"
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
"$<$<BOOL:${WIN32}>:Ole32;Shell32;uuid;Userenv;Advapi32;ws2_32>"
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

    if("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
      set(app_cxx_std 23)
    else()
      set(app_cxx_std ${cxx_std})
    endif()
    target_compile_features(fs_cli PRIVATE cxx_std_${app_cxx_std})

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
