if(HAVE_CXX_FILESYSTEM)

add_executable(test_libpath_cpp test_libpath.cpp)
target_link_libraries(test_libpath_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_libpath COMMAND test_libpath_cpp $<BOOL:${BUILD_SHARED_LIBS}>)
set_property(TEST Cpp_libpath PROPERTY LABELS Cpp)

if(NOT BUILD_SHARED_LIBS)
  set(_p $<TARGET_FILE_DIR:test_libpath_cpp>)
endif()

endif()

if(HAVE_Fortran_FILESYSTEM)

add_executable(test_libpath_fortran test_libpath.f90)
target_link_libraries(test_libpath_fortran PRIVATE ffilesystem)
target_compile_options(test_libpath_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
set_property(TARGET test_libpath_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_libpath COMMAND test_libpath_fortran $<BOOL:${BUILD_SHARED_LIBS}>)
set_property(TEST Fortran_libpath PROPERTY LABELS Fortran)

if(NOT BUILD_SHARED_LIBS)
  # not CMAKE_CURRENT_BINARY_DIR in case of multi-config generator
  set(_p $<TARGET_FILE_DIR:test_libpath_fortran>)
endif()

endif()


get_property(test_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)

if(BUILD_SHARED_LIBS)
  set(_p $<TARGET_FILE:ffilesystem>)
endif()

set_tests_properties(${test_names} PROPERTIES
PASS_REGULAR_EXPRESSION ${_p}
DISABLED $<OR:$<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>,$<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>>
)
