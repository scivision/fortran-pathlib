if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()

set_property(DIRECTORY PROPERTY LABELS "Fortran")

if(ffilesystem_cli)

if(HAVE_CXX_FILESYSTEM)
  set(_r "^Ffs\> T")
else()
  set(_r "^Ffs\> F")
endif()

add_test(NAME Fortran_CLI_Cpp
COMMAND ${CMAKE_COMMAND} -Dexe=$<TARGET_FILE:filesystem_cli>
-Dregex=${_r}
-P ${CMAKE_CURRENT_SOURCE_DIR}/../stdin.cmake
)

endif()

foreach(t IN ITEMS filesize find utf8)
  add_executable(test_${t} test_${t}.f90)
  target_link_libraries(test_${t} PRIVATE ffilesystem)
  target_compile_options(test_${t} PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
  # Intel needs linker_language Fortran else error "undefined reference to `main'"
  set_property(TARGET test_${t} PROPERTY LINKER_LANGUAGE Fortran)

  add_test(NAME Fortran_${t} COMMAND test_${t})
endforeach()
