if use_cpp

exp_cpp = executable('expanduser_cpp', 'test_expanduser.cpp', link_with: filesystem, include_directories: inc)
test('expanduser_cpp', exp_cpp, suite: 'cpp')

endif

if fortran

tec = executable('expanduser_fortran', 'test_expanduser.f90', link_with: filesystem, include_directories: inc)
test('expanduser_fortran', tec, suite: 'fortran')

endif
