t_cpp = executable('exe_cpp', 'test_exe.cpp', link_with: filesystem, include_directories: inc)
test('exe_cpp', t_cpp, suite: 'cpp')


if fortran

t_exe = executable('exe_fortran', 'test_exe.f90', link_with: filesystem, include_directories: inc)
t_which = executable('which_fortran', 'test_which.f90', link_with: filesystem, include_directories: inc)

test('exe_fortran', t_exe, suite: 'fortran')

env = environment()
env.unset('PATH')

test('NoPathWhich_fortran', t_which,
suite: 'fortran',
env: env,
args: '1',
should_fail: true,
workdir: meson.current_build_dir()
)

test('which_fortran', t_which, suite: 'fortran', workdir: meson.current_build_dir())

endif
