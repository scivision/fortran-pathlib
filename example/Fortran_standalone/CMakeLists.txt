# show that for Fortran, we only need filesystem.cpp, common.c, filesystem.f90

cmake_minimum_required(VERSION 3.14)

project(Fortran_standalone LANGUAGES C CXX Fortran)
# need to enable C language to allow extern "C" functions in C++/Fortran code--else linker failure

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)


add_library(ffilesystem ../../src/common/filesystem.cpp ../../src/common/common.c
../../src/common/fortran/filesystem.f90
)

target_include_directories(ffilesystem PRIVATE ../../include)

add_executable(main ../1/main.f90)
set_property(TARGET main PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries(main PRIVATE ffilesystem)

add_test(NAME Fortran_standalone COMMAND main)

file(GENERATE OUTPUT .gitignore CONTENT "*")
