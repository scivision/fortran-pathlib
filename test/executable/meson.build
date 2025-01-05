t_exe_cpp = executable('exe_cpp', 'test_exe.cpp', link_with: filesystem, include_directories: [inc, '..'])
test('exe_cpp', t_exe_cpp, suite: 'cpp')

t_which_cpp = executable('which_cpp', 'test_which.cpp', link_with: filesystem, include_directories: [inc, '..])
test('which_cpp', t_which_cpp, suite: 'cpp')


test('NoPathWhich_cpp', t_which_cpp,
suite: 'cpp',
args: '1',
workdir: meson.current_build_dir()
)


if fortran

t_exe = executable('exe_fortran', 'test_exe.f90', link_with: filesystem, include_directories: inc)
t_which = executable('which_fortran', 'test_which.f90', link_with: filesystem, include_directories: inc)

test('exe_fortran', t_exe, suite: 'fortran')

test('which_fortran', t_which, suite: 'fortran', workdir: meson.current_build_dir())

endif
