cmake_minimum_required(VERSION 3.20)

project(fs_check LANGUAGES C CXX)

option(ffilesystem_fortran "Build Fortran test")

if(ffilesystem_fortran)
  enable_language(Fortran)
endif()

enable_testing()

if(GNU_stdfs)
  message(STATUS "fs_check: applying flags ${GNU_stdfs}")
endif()

set(CMAKE_CXX_STANDARD 17)

add_library(fs_cpp OBJECT lib_fs.cpp)
target_include_directories(fs_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fs_cpp PUBLIC ${GNU_stdfs})
target_compile_features(fs_cpp PUBLIC cxx_std_17)

add_executable(c_fs main_fs.c)
target_link_libraries(c_fs PRIVATE fs_cpp)

add_test(NAME C_fs COMMAND c_fs)

if(ffilesystem_fortran)
  add_executable(fortran_fs main.f90)
  target_link_libraries(fortran_fs PRIVATE fs_cpp)
  set_property(TARGET fortran_fs PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_fs COMMAND fortran_fs)
endif()
