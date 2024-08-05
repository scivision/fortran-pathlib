cmake_minimum_required(VERSION 3.20)

project(checker LANGUAGES C CXX Fortran)

# trigger warning from AppleClang that indicates try-catch is unreliable

add_executable(main_cxx main.cpp lib.f90)
