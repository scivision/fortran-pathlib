env = environment()
env.unset('USERPROFILE')
env.unset('HOME')
env.unset('TMPDIR')
env.unset('TEMP')

if use_cpp

env_exe_cpp = executable('env_cpp', 'test_env.cpp', link_with: filesystem, include_directories: inc)

test('env_cpp', env_exe_cpp, suite: 'cpp')
test('NoEnv_Cpp', env_exe_cpp, suite: 'cpp', env: env)

endif

if fortran

env_exe = executable('env_fortran', 'test_env.f90', link_with: filesystem, include_directories: inc)
setenv_exe = executable('setenv_fortran', 'test_setenv.f90', link_with: filesystem, include_directories: inc)

test('env_fortran', env_exe, suite: 'fortran')

test('NoEnv_fortran', env_exe, suite: 'fortran', env: env)

test('setenv_fortran', setenv_exe, suite: 'fortran')

endif
