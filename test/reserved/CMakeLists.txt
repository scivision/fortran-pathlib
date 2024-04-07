add_executable(test_reserved_c test_reserved.c)
target_link_libraries(test_reserved_c PRIVATE ffilesystem)

add_test(NAME C_reserved COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_reserved_c>)
# the -E env launcher is needed to catch subprocess abort for macOS with Clang (shaky exception handling in general)
set_tests_properties(C_reserved PROPERTIES
SKIP_REGULAR_EXPRESSION "SIGABRT;[aA]bort"
)


if(HAVE_Fortran_FILESYSTEM)
add_executable(test_reserved_fortran test_reserved.f90)
target_link_libraries(test_reserved_fortran PRIVATE ffilesystem)
set_property(TARGET test_reserved_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_reserved COMMAND test_reserved_fortran)

add_executable(test_safe_fortran test_safe.f90)
target_link_libraries(test_safe_fortran PRIVATE ffilesystem)
set_property(TARGET test_safe_fortran PROPERTY LINKER_LANGUAGE Fortran)
target_compile_options(test_safe_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

add_test(NAME Fortran_safe COMMAND test_safe_fortran)


set_property(TEST Fortran_reserved Fortran_safe PROPERTY LABELS "Fortran")
endif()

if(HAVE_CXX_FILESYSTEM)

add_executable(test_safe_cpp test_safe.cpp)
target_link_libraries(test_safe_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_safe COMMAND test_safe_cpp)

set_property(TEST Cpp_safe PROPERTY LABELS "Cpp")

endif()
