if fortran

core_tests = ['is_absolute', 'absolute',
'canonical', 'realpath',
'expanduser',
'filesystem_type',
'filesize', 'space',
'filename', 'is_dir',
'is_subdir', 'join', 'normal', 'parent', 'root', 'separator', 'stem',
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


if use_cpp

cpp_core = ['canonical', 'core', 'expanduser']

foreach t : cpp_core

t_cpp = executable(t + '_cpp', 'cpp/test_' + t + '.cpp', link_with: filesystem, include_directories: inc)

test(t + '_cpp', t_cpp, suite: 'cpp')

endforeach


endif
