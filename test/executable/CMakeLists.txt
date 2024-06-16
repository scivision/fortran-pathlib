if(HAVE_CXX_FILESYSTEM)

add_executable(test_exe_cpp test_exe.cpp)
target_link_libraries(test_exe_cpp PRIVATE ffilesystem)
add_test(NAME Cpp_exe COMMAND test_exe_cpp)

set_tests_properties(Cpp_exe PROPERTIES
LABELS "Cpp"
SKIP_RETURN_CODE 77
)

endif()


if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()


foreach(t IN ITEMS exe which)

add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
target_compile_options(test_${t}_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
set_property(TEST Fortran_${t} PROPERTY LABELS "Fortran")

set_tests_properties(Fortran_exe PROPERTIES
SKIP_RETURN_CODE 77
)

endforeach()

set_property(TEST Fortran_which PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_which_fortran>)
# Windows CWD priority test with multi-config generators

add_test(NAME Fortran_which_noPath COMMAND test_which_fortran 1)

if(NOT WIN32)
# windows emits error 135 since it can't find DLL.
if(CMAKE_VERSION VERSION_LESS 3.22)
  set_property(TEST Fortran_which_noPath PROPERTY ENVIRONMENT "PATH=")
else()
  set_property(TEST Fortran_which_noPath PROPERTY ENVIRONMENT_MODIFICATION "PATH=unset:")
endif()
endif()

set_tests_properties(Fortran_which_noPath PROPERTIES
WILL_FAIL true
LABELS "Fortran"
)
