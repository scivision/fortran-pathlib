set_property(DIRECTORY PROPERTY LABELS "core")

if(HAVE_Fortran_FILESYSTEM)

set(tests absolute filename
filesystem_type
is_dir is_subdir join
normal parent root separator stem
suffix with_suffix
)

if(HAVE_F03TYPE)
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

target_compile_options(test_fortran_filesystem_type PRIVATE
$<$<COMPILE_LANG_AND_ID:Fortran,GNU>:-Wno-uninitialized>
)

set_tests_properties(Fortran_filesystem_type PROPERTIES
SKIP_RETURN_CODE 77
DISABLED $<AND:$<BOOL:${LINUX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_LINUX_MAGIC}>>>
)

set_tests_properties(Fortran_root PROPERTIES FIXTURES_SETUP root)

set_tests_properties(Fortran_filesystem_type Fortran_parent Fortran_space PROPERTIES FIXTURES_REQUIRED root)

endif()


if(HAVE_CXX_FILESYSTEM)

# --- C++
add_executable(test_cpp_core cpp/test_core.cpp)
target_link_libraries(test_cpp_core PRIVATE ffilesystem)

add_test(NAME Cpp_core COMMAND test_cpp_core)
set_property(TEST Cpp_core PROPERTY LABELS Cpp)

endif()
