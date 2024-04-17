if(HAVE_CXX_FILESYSTEM)

add_executable(test_permissions_cpp test_permissions.cpp)
target_link_libraries(test_permissions_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_permissions COMMAND test_permissions_cpp)
set_property(TEST Cpp_permissions PROPERTY LABELS Cpp)

endif()



if(HAVE_Fortran_FILESYSTEM)
add_executable(test_permissions_fortran test_permissions.f90)
target_link_libraries(test_permissions_fortran PRIVATE ffilesystem)
set_property(TARGET test_permissions_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_permissions COMMAND test_permissions_fortran)
set_property(TEST Fortran_permissions PROPERTY LABELS Fortran)
endif()
