if fortran

env_exe = executable('env_fortran', 'test_env.f90', link_with: filesystem, include_directories: inc)
setenv_exe = executable('setenv_fortran', 'test_setenv.f90', link_with: filesystem, include_directories: inc)

test('env_fortran', env_exe, suite: 'fortran')

env = environment()
env.set('USERPROFILE', '')
env.set('HOME', '')
env.set('TMPDIR', '')
env.set('TEMP', '')
test('NoEnv_fortran', env_exe, suite: 'fortran', env: env)

test('setenv_fortran', setenv_exe, suite: 'fortran')



endif
