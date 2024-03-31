if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()

set_property(DIRECTORY PROPERTY LABELS "Fortran")

if(ffilesystem_cli)

if(HAVE_CXX_FILESYSTEM)
  set(_r "^Ffilesystem\> T")
else()
  set(_r "^Ffilesystem\> F")
endif()

add_test(NAME Fortran_CLI_Cpp
COMMAND ${CMAKE_COMMAND} -Dexe=$<TARGET_FILE:filesystem_cli>
-Dregex=${_r}
-P ${CMAKE_CURRENT_SOURCE_DIR}/../stdin.cmake
)

endif()

foreach(t IN ITEMS find normal utf8)
  add_executable(test_${t} test_${t}.f90)
  target_link_libraries(test_${t} PRIVATE ffilesystem)
  target_compile_options(test_${t} PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

  add_test(NAME Fortran_${t} COMMAND test_${t})
endforeach()


# --- shaky tests

add_executable(fortran_filesize test_filesize.f90)
target_link_libraries(fortran_filesize PRIVATE ffilesystem)
add_test(NAME Fortran_filesize
COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:fortran_filesize> ${CMAKE_CURRENT_BINARY_DIR}/test_filesize.dat)

set_property(TEST Fortran_filesize PROPERTY SKIP_REGULAR_EXPRESSION "not implemented;SIGABRT;[aA]bort")

# Intel needs linker_language Fortran else error "undefined reference to `main'"
get_property(test_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
set_property(TARGET ${test_targets} PROPERTY LINKER_LANGUAGE Fortran)
