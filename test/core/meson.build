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
'set_cwd',
'suffix', 'with_suffix',
'resolve',
'relative', 'proximate']

if have_f03type
core_tests += 'path_t'
endif

foreach t : core_tests

tec = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)

test(t + '_fortran', tec, suite: 'fortran', workdir: meson.current_build_dir())

endforeach

endif


cpp_core = [
'absolute',
'canonical', 'core',
'copyfile',
'equivalent',
'expanduser',
'filename', 'filesize',
'hard_link',
'is_absolute',
'is_dir',
'is_empty',
'is_file',
'join',
'locale',
'mkdir',
'owner',
'normal',
'parent', 'permissions',
'proximate',
'realpath', 'relative',
'reserved', 'root',
'safe',
'set_cwd',
'stem',
'system',
'touch',
'with_suffix'
]

foreach t : cpp_core

    t_cpp = executable(t + '_cpp', 'test_' + t + '.cpp', link_with: filesystem,
      include_directories: [inc, '..'])

    test(t + '_cpp', t_cpp, suite: 'cpp', workdir: meson.current_build_dir())

endforeach
