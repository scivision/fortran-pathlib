if(HAVE_CXX_FILESYSTEM)

add_executable(test_permissions_cpp test_permissions.cpp)
target_link_libraries(test_permissions_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_permissions COMMAND test_permissions_cpp)

set_tests_properties(Cpp_permissions PROPERTIES
LABELS Cpp
SKIP_RETURN_CODE 77
)

endif()



if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS owner permissions)

add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE Fortran)
target_compile_options(test_${t}_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)

set_tests_properties(Fortran_${t} PROPERTIES
LABELS Fortran
SKIP_RETURN_CODE 77
)

endforeach()

set_property(TEST Fortran_permissions PROPERTY DISABLED $<BOOL:${CYGWIN}>)

endif()
