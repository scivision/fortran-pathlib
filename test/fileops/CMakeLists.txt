foreach(t IN ITEMS mkdtemp)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)

  add_test(NAME Cpp_${t} COMMAND test_${t}_cpp)
  set_property(TEST Cpp_${t} PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>)
endforeach()


if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS copyfile fileop is_empty mkdir mkdtemp same touch)
  add_executable(test_${t}_fortran test_${t}.f90)
  target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
  target_compile_options(test_${t}_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
  set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
  set_property(TEST Fortran_${t} PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_fortran>)
endforeach()

set_tests_properties(Fortran_mkdir PROPERTIES
FIXTURES_SETUP mkdir
FIXTURES_REQUIRED root
)

set_tests_properties(Fortran_mkdtemp PROPERTIES
LABELS Fortran
FIXTURES_REQUIRED "join;mkdir"
)

set_tests_properties(Fortran_touch PROPERTIES FIXTURES_SETUP touch)
set_tests_properties(Fortran_copyfile PROPERTIES FIXTURES_REQUIRED filesize)

set_property(TEST Fortran_same PROPERTY FIXTURES_REQUIRED mkdir)

set_property(TEST Fortran_is_empty PROPERTY FIXTURES_REQUIRED "touch;filesize;mkdir")

endif()
