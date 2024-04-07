if(HAVE_Fortran_FILESYSTEM)

set(tests core filename is_subdir parent)

if(HAVE_F03_TYPE)
  list(APPEND tests path_t)
endif()

foreach(t IN LISTS tests)

add_executable(test_fortran_${t} test_${t}.f90)
target_link_libraries(test_fortran_${t} PRIVATE ffilesystem)
target_compile_options(test_fortran_${t} PRIVATE ${${PROJECT_NAME}_fortran_test_flags})
set_property(TARGET test_fortran_${t} PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_${t} COMMAND test_fortran_${t})
set_property(TEST Fortran_${t} PROPERTY LABELS Fortran)

endforeach()

endif()


if(HAVE_CXX_FILESYSTEM)

# --- C++
add_executable(test_cpp_core cpp/test_core.cpp)
target_link_libraries(test_cpp_core PRIVATE ffilesystem)

add_test(NAME Cpp_core COMMAND test_cpp_core)
set_property(TEST Cpp_core PROPERTY LABELS Cpp)

endif()
