if(NOT HAVE_Fortran_FILESYSTEM)
  return()
endif()

add_executable(test_fileop test_fileop.f90)
target_link_libraries(test_fileop PRIVATE ffilesystem)
target_compile_options(test_fileop PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

add_test(NAME Fortran_fileop COMMAND test_fileop ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TEST Fortran_fileop PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_fileop>)
# workdir avoids test state being shared between tests

add_executable(test_copyfile test_copyfile.f90)
target_link_libraries(test_copyfile PRIVATE ffilesystem)
add_test(NAME Fortran_copyfile COMMAND test_copyfile)
set_property(TEST Fortran_copyfile PROPERTY WORKING_DIRECTORY $<TARGET_FILE_DIR:test_copyfile>)

add_executable(test_mkdir test_mkdir.f90)
target_link_libraries(test_mkdir PRIVATE ffilesystem)
target_compile_options(test_mkdir PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

add_test(NAME Fortran_mkdir COMMAND test_mkdir)
set_property(TEST Fortran_mkdir PROPERTY FIXTURES_SETUP mkdir_fxt)

add_executable(fortran_same test_same.f90)
target_link_libraries(fortran_same PRIVATE ffilesystem)
add_test(NAME Fortran_same COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:fortran_same>)
set_property(TEST Fortran_same PROPERTY FIXTURES_REQUIRED mkdir_fxt)

add_executable(test_touch test_touch.f90)
target_link_libraries(test_touch PRIVATE ffilesystem)
target_compile_options(test_touch PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
add_test(NAME Fortran_touch COMMAND test_touch)
set_property(TEST Fortran_touch PROPERTY SKIP_REGULAR_EXPRESSION "C\\+\\+20 required")

# --- props

set_property(TEST Fortran_fileop Fortran_same PROPERTY
SKIP_REGULAR_EXPRESSION "not implemented;not available;SIGABRT;[aA]bort")

set_property(TEST Fortran_copyfile Fortran_fileop Fortran_mkdir Fortran_same PROPERTY LABELS "Fortran")

# Intel needs linker_language Fortran else error "undefined reference to `main'"
set_property(TARGET test_fileop test_copyfile test_mkdir fortran_same test_touch PROPERTY LINKER_LANGUAGE Fortran)
