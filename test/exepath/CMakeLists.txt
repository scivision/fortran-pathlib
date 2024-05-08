if(HAVE_CXX_FILESYSTEM)

add_executable(test_exepath_cpp test_exepath.cpp)
target_link_libraries(test_exepath_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_exepath COMMAND test_exepath_cpp)

set_tests_properties(Cpp_exepath PROPERTIES
PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_cpp>
LABELS Cpp
)

endif()


if(HAVE_Fortran_FILESYSTEM)

add_executable(test_exepath_fortran test_exepath.f90)
target_link_libraries(test_exepath_fortran PRIVATE ffilesystem)
target_compile_options(test_exepath_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
set_property(TARGET test_exepath_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_exepath COMMAND test_exepath_fortran)

# TARGET_FILE_NAME to allow for macOS possible non-canonical result
# and symlinked / special filesystems like WSL
set_tests_properties(Fortran_exepath PROPERTIES
PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_fortran>
LABELS Fortran
)

endif()
