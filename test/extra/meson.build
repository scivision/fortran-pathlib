cpp_extra = []

if get_option('locale')
  cpp_extra += ['locale']
endif

foreach t : cpp_extra

    t_cpp = executable(t + '_cpp', 'test_' + t + '.cpp', link_with: filesystem,
      include_directories: [inc, '..'])

    test(t + '_cpp', t_cpp, suite: ['cpp', 'extra'], workdir: meson.current_build_dir())

endforeach
