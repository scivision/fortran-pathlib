set_property(DIRECTORY PROPERTY LABELS "core")

if(HAVE_Fortran_FILESYSTEM)

set(tests is_absolute absolute
canonical realpath
expanduser
filename
filesize space
filesystem_type
is_dir is_subdir join
normal parent root separator stem
suffix with_suffix
resolve
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

set_tests_properties(Fortran_stem PROPERTIES FIXTURES_SETUP stem)
set_tests_properties(Fortran_with_suffix PROPERTIES FIXTURES_REQUIRED stem)

set_tests_properties(Fortran_join PROPERTIES FIXTURES_SETUP join)
set_tests_properties(Fortran_is_absolute PROPERTIES FIXTURES_SETUP is_absolute)
set_tests_properties(Fortran_absolute PROPERTIES
FIXTURES_SETUP absolute
FIXTURES_REQUIRED "is_absolute;join"
)
set_tests_properties(Fortran_expanduser PROPERTIES FIXTURES_SETUP expanduser)

set_tests_properties(Fortran_resolve PROPERTIES
FIXTURES_REQUIRED "absolute;canonical")

set_tests_properties(Fortran_root PROPERTIES
FIXTURES_SETUP root
FIXTURES_REQUIRED is_absolute
)

set_tests_properties(Fortran_filesystem_type Fortran_space PROPERTIES FIXTURES_REQUIRED root)

set_tests_properties(Fortran_canonical PROPERTIES
FIXTURES_REQUIRED "expanduser"
FIXTURES_SETUP "canonical"
)

endif()


if(HAVE_CXX_FILESYSTEM)

foreach(t IN ITEMS canonical expanduser core)
  add_executable(test_cpp_${t} cpp/test_${t}.cpp)
  target_link_libraries(test_cpp_${t} PRIVATE ffilesystem)

  add_test(NAME Cpp_${t} COMMAND test_cpp_${t})
  set_property(TEST Cpp_${t} PROPERTY LABELS Cpp)
endforeach()

endif()
