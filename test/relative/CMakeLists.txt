if(HAVE_Fortran_FILESYSTEM)
add_executable(test_relative_fortran test_relative.f90)
target_link_libraries(test_relative_fortran PRIVATE ffilesystem)
set_property(TARGET test_relative_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_relative COMMAND test_relative_fortran)
set_property(TEST Fortran_relative PROPERTY LABELS Fortran)
endif()
