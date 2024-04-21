if fortran

core_tests = ['absolute', 'filename', 'is_dir',
'is_subdir', 'join', 'normal', 'parent', 'root', 'separator', 'stem',
'suffix', 'with_suffix']

core_tests += 'path_t'

foreach t : core_tests
tec = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)
    test(t + '_fortran', tec, suite: 'fortran')
endforeach

endif

t_cpp = executable('core_cpp', 'cpp/test_core.cpp', link_with: filesystem, include_directories: inc)
test('core_cpp', t_cpp, suite: 'cpp')
