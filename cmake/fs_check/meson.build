project('fs_check', 'c', 'cpp', 'fortran',
default_options: ['cpp_std=c++17'])

# this is a last-resort debugging tool if CMake won't build the test project.

# this is a Fortran interface for the Ffilesystem standin
libfortran = library('fs_fortran', 'lib_fs.f90')

# this is a standin for the Ffilesystem backend library in C++
standin = library('ffilesystem_standin', 'lib_fs.cpp',
 link_with: libfortran)

# this is a Fortran main program using Ffilesystem standin
main_fortran = executable('fortran_main', 'main.f90',
link_with: [standin, libfortran])
# have to link_with libfortran too, or the .mod file isn't found

test('Fortran_fs', main_fortran)

# this is a C main program using Ffilesystem standin
main_c = executable('c_main', 'main.c', link_with: standin)

test('C_fs', main_c)

# this is a C++ main program using Ffilesystem standin
main_cpp = executable('cpp_main', 'main.cpp', link_with: standin)

test('Cpp_fs', main_cpp)
