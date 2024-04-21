if fortran

foreach t : ['filesize', 'find', 'utf8']
tec = executable(t + '_fortran', 'test_' + t + '.f90', link_with: filesystem, include_directories: inc)
    test(t + '_fortran', tec, suite: 'fortran')
endforeach

endif
