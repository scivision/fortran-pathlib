if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()

foreach(t IN ITEMS copyfile fileop mkdir same touch)
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

set_property(TEST Fortran_same PROPERTY FIXTURES_REQUIRED mkdir_fxt)
