add_executable(test_Cpp_windows test_windows.cpp)
target_link_libraries(test_Cpp_windows PRIVATE ffilesystem)
target_include_directories(test_Cpp_windows PRIVATE ..)

add_test(NAME Cpp_windows COMMAND test_Cpp_windows)
set_property(TEST Cpp_windows PROPERTY LABELS Cpp)


if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS cygwin windows)
  add_executable(test_Fortran_${t} test_${t}.f90)
  target_link_libraries(test_Fortran_${t} PRIVATE ffilesystem)
  set_property(TARGET test_Fortran_${t} PROPERTY LINKER_LANGUAGE Fortran)
  target_compile_options(test_Fortran_${t} PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

  add_test(NAME Fortran_${t} COMMAND test_Fortran_${t})
  set_property(TEST Fortran_${t} PROPERTY LABELS Fortran)
endforeach()

endif()

get_property(test_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)

set_tests_properties(${test_names} PROPERTIES
DISABLED $<NOT:$<OR:$<BOOL:${WIN32}>,$<BOOL:${CYGWIN}>>>
LABELS "windows"
)

if(TEST Fortran_cygwin)
  set_property(TEST Fortran_cygwin PROPERTY DISABLED $<NOT:$<BOOL:${CYGWIN}>>)
endif()
