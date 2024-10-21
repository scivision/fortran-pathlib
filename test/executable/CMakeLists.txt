add_executable(test_exe_cpp test_exe.cpp)
target_link_libraries(test_exe_cpp PRIVATE ffilesystem)
target_include_directories(test_exe_cpp PRIVATE ..)
add_test(NAME Cpp_exe COMMAND test_exe_cpp)

set_tests_properties(Cpp_exe PROPERTIES
LABELS "Cpp"
SKIP_RETURN_CODE 77
)



if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()


foreach(t IN ITEMS exe which)

add_executable(test_fortran_${t} test_${t}.f90)
target_link_libraries(test_fortran_${t} PRIVATE ffilesystem)
target_compile_options(test_fortran_${t} PRIVATE ${ffilesystem_fortran_test_flags})
set_property(TARGET test_fortran_${t} PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_${t} COMMAND test_fortran_${t})
set_property(TEST Fortran_${t} PROPERTY LABELS "Fortran")

endforeach()

set_tests_properties(Fortran_exe PROPERTIES SKIP_RETURN_CODE 77)

set_property(TEST Fortran_which PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_fortran_which>)
# Windows CWD priority test with multi-config generators

add_test(NAME Fortran_which_noPath COMMAND test_fortran_which 1)

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
