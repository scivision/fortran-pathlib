t_cpp = executable('canonical_cpp', 'test_canonical.cpp', link_with: filesystem, include_directories: inc)
test('canonical_cpp', t_cpp, suite: 'cpp')


if fortran

t_canon = executable('canonical_fortran', 'test_canonical.f90', link_with: filesystem, include_directories: inc)
t_resolve = executable('resolve_fortran', 'test_resolve.f90', link_with: filesystem, include_directories: inc)

test('canonical_fortran', t_canon, suite: 'fortran')
test('resolve_fortran', t_resolve, suite: 'fortran')

endif
