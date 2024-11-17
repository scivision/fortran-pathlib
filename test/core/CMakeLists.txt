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
relative proximate
reserved safe
owner permissions
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
set_tests_properties(Fortran_${t} PROPERTIES
LABELS Fortran
SKIP_RETURN_CODE 77
)

endforeach()

target_compile_options(test_fortran_filesystem_type PRIVATE
$<$<COMPILE_LANG_AND_ID:Fortran,GNU>:-Wno-uninitialized>
)

set_tests_properties(Fortran_filesystem_type PROPERTIES
DISABLED $<AND:$<BOOL:${LINUX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_LINUX_MAGIC}>>>
)

set_tests_properties(Fortran_relative PROPERTIES FIXTURES_SETUP relative)
set_tests_properties(Fortran_is_subdir PROPERTIES FIXTURES_REQUIRED relative)


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

set_tests_properties(Fortran_filesize PROPERTIES FIXTURES_SETUP filesize)

set_tests_properties(Fortran_filesystem_type Fortran_space PROPERTIES FIXTURES_REQUIRED root)

set_tests_properties(Fortran_canonical PROPERTIES
FIXTURES_REQUIRED "expanduser"
FIXTURES_SETUP "canonical"
)

endif()


set(cpp_tests canonical expanduser core
filename filesize
hard_link
is_absolute
join
normal
parent permissions
realpath relative reserved root
safe system
wide
)

foreach(t IN LISTS cpp_tests)
  add_executable(test_cpp_${t} test_${t}.cpp)
  target_link_libraries(test_cpp_${t} PRIVATE ffilesystem)
  target_include_directories(test_cpp_${t} PRIVATE ..)

  add_test(NAME Cpp_${t} COMMAND test_cpp_${t})
  set_tests_properties(Cpp_${t} PROPERTIES
  LABELS Cpp
  SKIP_RETURN_CODE 77
  )
endforeach()

add_executable(test_cpp_locale test_locale.cpp)
target_link_libraries(test_cpp_locale PRIVATE ffilesystem)
target_include_directories(test_cpp_locale PRIVATE ..)

add_test(NAME Cpp_locale COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_cpp_locale>)
set_tests_properties(Cpp_locale PROPERTIES
SKIP_REGULAR_EXPRESSION "locale\:\:facet\:\:\_S\_create\_c\_locale name not valid"
LABELS Cpp
)

# the MSVC compiler requires this option or it triples the size of the non-English L"" strings and fails the test
# Windows oneAPI is fine without this flag
target_compile_options(test_cpp_wide PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

if(NOT WIN32 AND NOT HAVE_CXX_FILESYSTEM)
  set_tests_properties(Cpp_filesize PROPERTIES DISABLED $<NOT:$<BOOL:${ffilesystem_HAVE_SYS_STATVFS}>>)
endif()
