if fortran

core_tests = ['is_absolute', 'absolute',
'canonical', 'realpath',
'expanduser',
'filesystem_type',
'filesize', 'space',
'filename', 'is_dir',
'is_subdir', 'join', 'normal', 'parent', 'root', 'separator', 'stem',
'permissions', 'reserved', 'safe',
'suffix', 'with_suffix',
'resolve',
'relative', 'proximate']

if have_f03type
core_tests += 'path_t'
endif

foreach t : core_tests

tec = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)

test(t + '_fortran', tec, suite: 'fortran')

endforeach

endif


cpp_core = ['canonical', 'core', 'expanduser',
'filename', 'filesize',
'hard_link',
'is_absolute',
'join',
'locale',
'normal',
'parent', 'permissions',
'realpath', 'relative', 'reserved', 'root',
'safe', 'system',
'wide']

foreach t : cpp_core

    t_cpp = executable(t + '_cpp', 'test_' + t + '.cpp', link_with: filesystem,
      include_directories: [inc, '..'])

    test(t + '_cpp', t_cpp, suite: 'cpp')

endforeach
