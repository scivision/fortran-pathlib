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

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

add_library(fs_cpp OBJECT lib_fs.cpp)
target_include_directories(fs_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fs_cpp PUBLIC ${GNU_stdfs})
target_compile_features(fs_cpp PUBLIC cxx_std_17)

add_executable(c_fs main_fs.c)
target_link_libraries(c_fs PRIVATE fs_cpp)
add_test(NAME C_fs COMMAND c_fs)

# when the -L system flag is used, say oneAPI with a too old GCC,
# a main C++ target may fail to link but Fortran main targets may succeed
add_executable(cxx_fs main.cpp)
target_link_libraries(cxx_fs PRIVATE fs_cpp)
add_test(NAME Cxx_fs COMMAND cxx_fs)

if(ffilesystem_fortran)
  add_executable(fortran_fs main.f90)
  target_link_libraries(fortran_fs PRIVATE fs_cpp)
  set_property(TARGET fortran_fs PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_fs COMMAND fortran_fs)
endif()
