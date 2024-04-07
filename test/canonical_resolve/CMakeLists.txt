if(HAVE_CXX_FILESYSTEM)

add_executable(test_canonical_cpp test_canonical.cpp)
target_link_libraries(test_canonical_cpp PRIVATE ffilesystem)

add_test(NAME test_canonical_cpp COMMAND test_canonical_cpp)
set_property(TEST test_canonical_cpp PROPERTY PROPERTY LABELS "Cpp")

endif()


if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS canonical resolve)
  add_executable(Fortran_${t} test_${t}.f90)
  target_link_libraries(Fortran_${t} PRIVATE ffilesystem)
  target_compile_options(Fortran_${t} PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
  set_property(TARGET Fortran_${t} PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_${t} COMMAND Fortran_${t})
  set_property(TEST Fortran_${t} PROPERTY PROPERTY LABELS "Fortran")
endforeach()

endif()
