if(HAVE_CXX_FILESYSTEM)

add_executable(test_Cpp_windows test_windows.cpp)
target_link_libraries(test_Cpp_windows PRIVATE ffilesystem)

add_test(NAME Cpp_windows COMMAND test_Cpp_windows)
set_property(TEST Cpp_windows PROPERTY LABELS Cpp)

endif()


if(HAVE_Fortran_FILESYSTEM)

add_executable(test_Fortran_windows test_windows.f90)
target_link_libraries(test_Fortran_windows PRIVATE ffilesystem)
set_property(TARGET test_Fortran_windows PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_Ffilesystem_windows COMMAND test_Fortran_windows)
set_property(TEST Fortran_Ffilesystem_windows PROPERTY LABELS Fortran)

if(CYGWIN)
  add_executable(test_cygwin test_cygwin.f90)
  target_link_libraries(test_cygwin PRIVATE ffilesystem)
  set_property(TARGET test_cygwin PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_cygwin COMMAND test_cygwin)
  set_property(TEST Fortran_cygwin PROPERTY LABELS Fortran)
endif()

endif()

get_property(test_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)

set_property(TEST ${test_names} PROPERTY DISABLED $<NOT:$<BOOL:${WIN32}>>)
