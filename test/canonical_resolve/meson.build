if use_cpp

t_cpp = executable('canonical_cpp', 'test_canonical.cpp', link_with: filesystem, include_directories: inc)
test('canonical_cpp', t_cpp, suite: 'cpp')

endif

if fortran

foreach t : ['canonical', 'realpath', 'resolve']
  te = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)
  test(t + '_fortran', te, suite: 'fortran')
endforeach

endif
