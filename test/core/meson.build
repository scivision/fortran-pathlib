env = environment()
env.unset('USERPROFILE')
env.unset('HOME')
env.unset('TMPDIR')
env.unset('TEMP')


if fortran

core_tests = ['is_absolute', 'absolute',
'canonical', 'realpath',
'equivalent',
'expanduser',
'filesystem_type',
'filesize', 'space',
'filename', 'is_dir',
'is_subdir', 'join', 'normal', 'parent', 'root', 'separator', 'stem',
'permissions', 'reserved',
'safe',
'setenv',
'set_cwd',
'suffix', 'with_suffix',
'resolve',
'relative', 'proximate',
'path_t'
]


foreach t : core_tests

tec = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)

test(t + '_fortran', tec, suite: 'fortran', workdir: meson.current_build_dir())

endforeach

tenv = executable('env_fortran', 'test_env.f90', link_with: filesystem, include_directories: inc)

test('Env_fortran', tenv, suite: 'fortran')
test('NoEnv_fortran', tenv, suite: 'fortran', env: env)

endif


cpp_core = [
'absolute',
'canonical',
'copyfile',
'env',
'equivalent',
'exepath',
'expanduser',
'filename',
'filesize',
'filesystem_type',
'hard_link',
'is_absolute',
'is_dir',
'is_empty',
'is_fifo',
'is_file',
'is_prefix',
'is_subdir',
'join',
'locale',
'mkdir',
'owner',
'normal',
'parent',
'permissions',
'proximate',
'realpath',
'relative',
'rename',
'reserved',
'root',
'safe',
'separator',
'set_cwd',
'space',
'stem',
'suffix',
'system',
'touch',
'with_suffix'
]

if is_windows
  cpp_core += 'windows'
endif

foreach t : cpp_core

    t_cpp = executable(t + '_cpp', 'test_' + t + '.cpp', link_with: filesystem,
      include_directories: [inc, '..'])

    test(t + '_cpp', t_cpp, suite: 'cpp', workdir: meson.current_build_dir())

endforeach

tenv = executable('noenv_cpp', 'test_env.cpp', link_with: filesystem, include_directories: [inc, '..'])

test('NoEnv_cpp', tenv, suite: 'cpp', env: env)
