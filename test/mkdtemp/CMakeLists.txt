if(HAVE_Fortran_FILESYSTEM)

add_executable(test_fortran_mkdtemp test_mkdtemp.f90)
target_link_libraries(test_fortran_mkdtemp PRIVATE ffilesystem)
target_compile_options(test_fortran_mkdtemp PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
set_property(TARGET test_fortran_mkdtemp PROPERTY LINKER_LANGUAGE Fortran)


add_test(NAME Fortran_mkdtemp COMMAND test_fortran_mkdtemp)
set_property(TEST Fortran_mkdtemp PROPERTY LABELS Fortran)
endif()
