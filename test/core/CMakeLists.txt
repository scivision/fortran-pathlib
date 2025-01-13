set_property(DIRECTORY PROPERTY LABELS "core")

if(HAVE_Fortran_FILESYSTEM)

set(tests is_absolute absolute
canonical realpath
env
equivalent
exepath
expanduser
filename
filesize space
filesystem_type
is_dir
is_empty
is_subdir join
normal parent root
resolve
relative proximate
reserved safe
set_cwd
separator
stem
suffix
with_suffix
owner permissions
copyfile mkdir touch
cygwin windows
)

if(HAVE_F03TYPE)
  list(APPEND tests path_t)
endif()

foreach(t IN LISTS tests)

add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE Fortran)

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
set_tests_properties(Fortran_${t} PROPERTIES
LABELS Fortran
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_fortran>
FIXTURES_SETUP ${t}
)

endforeach()

target_compile_options(test_filesystem_type_fortran PRIVATE
$<$<COMPILE_LANG_AND_ID:Fortran,GNU>:-Wno-uninitialized>
)

set_tests_properties(Fortran_filesystem_type PROPERTIES
DISABLED $<AND:$<BOOL:${LINUX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_LINUX_MAGIC}>>>
)

set_tests_properties(Fortran_absolute PROPERTIES FIXTURES_REQUIRED "is_absolute;join")

set_tests_properties(Fortran_resolve PROPERTIES
FIXTURES_REQUIRED "absolute;canonical")

set_tests_properties(Fortran_root PROPERTIES FIXTURES_REQUIRED is_absolute)

set_tests_properties(Fortran_filesystem_type PROPERTIES FIXTURES_REQUIRED root)

set_tests_properties(Fortran_canonical PROPERTIES FIXTURES_REQUIRED "expanduser")

# TARGET_FILE_NAME to allow for macOS possible non-canonical result
# and symlinked / special filesystems like WSL
set_tests_properties(Fortran_exepath PROPERTIES PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_fortran>)

endif()


set(cpp_tests
absolute
canonical
copyfile
env
equivalent
exepath
expanduser
filename
filesize
filesystem_type
hard_link
is_absolute
is_appexec_alias
is_char
is_dir
is_empty
is_fifo
is_file
is_prefix
is_subdir
join
mkdir
owner
normal
parent
permissions
proximate
realpath
relative
rename
reserved root
safe
separator
set_cwd
space
stem
suffix
system
touch
with_suffix
windows
)

foreach(t IN LISTS cpp_tests)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_include_directories(test_${t}_cpp PRIVATE ..)

  add_test(NAME Cpp_${t} COMMAND test_${t}_cpp)
  set_tests_properties(Cpp_${t} PROPERTIES
  LABELS "Cpp"
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
  FIXTURES_SETUP ${t}
  )
endforeach()

target_compile_definitions(test_touch_cpp PRIVATE $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>)


foreach(t IN ITEMS empty)

  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_include_directories(test_${t}_cpp PRIVATE ..)

  add_test(NAME Cpp_${t} COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_${t}_cpp>)
  set_tests_properties(Cpp_${t} PROPERTIES
  LABELS "Cpp"
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
  FIXTURES_SETUP ${t}
  )

endforeach()

if(TARGET test_env_fortran)
  add_test(NAME Fortran_NoEnv COMMAND test_env_fortran)
  set(_noenv Fortran_NoEnv)
else()
  set(_noenv)
endif()
add_test(NAME Cpp_NoEnv COMMAND test_env_cpp)

if(CMAKE_VERSION VERSION_LESS 3.22 OR "$ENV{CI}")
  set_tests_properties(Cpp_NoEnv ${_noenv} PROPERTIES DISABLED true)
else()
  set_tests_properties(Cpp_NoEnv ${_noenv} PROPERTIES ENVIRONMENT_MODIFICATION "HOME=unset:;USERPROFILE=unset:;TEMP=unset:;TMPDIR=unset:")
endif()

set_property(TEST Cpp_empty PROPERTY
SKIP_REGULAR_EXPRESSION "not implemented;SIGABRT;[aA]bort;[sS]egmentation fault;segfault"
)

set_tests_properties(Cpp_mkdir PROPERTIES FIXTURES_REQUIRED root)

# TARGET_FILE_NAME to allow for macOS possible non-canonical result
# and symlinked / special filesystems like WSL
set_tests_properties(Cpp_exepath PROPERTIES PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_cpp>)

if(HAVE_Fortran_FILESYSTEM)

set_tests_properties(Fortran_with_suffix PROPERTIES FIXTURES_REQUIRED stem)
set_property(TEST Fortran_is_empty PROPERTY FIXTURES_REQUIRED "touch;filesize;mkdir")
set_property(TEST Fortran_equivalent PROPERTY FIXTURES_REQUIRED mkdir)

endif()

if(NOT WIN32 AND NOT HAVE_CXX_FILESYSTEM)
  set_tests_properties(Cpp_filesize PROPERTIES DISABLED $<NOT:$<BOOL:${ffilesystem_HAVE_SYS_STATVFS}>>)
endif()


set_tests_properties(Cpp_windows Cpp_is_appexec_alias PROPERTIES
DISABLED $<NOT:$<OR:$<BOOL:${WIN32}>,$<BOOL:${CYGWIN}>>>
LABELS "windows"
)
if(HAVE_Fortran_FILESYSTEM)
  set_tests_properties(Fortran_cygwin Fortran_windows PROPERTIES
  DISABLED $<NOT:$<OR:$<BOOL:${WIN32}>,$<BOOL:${CYGWIN}>>>
  LABELS "windows"
  )

  set_property(TEST Fortran_cygwin PROPERTY DISABLED $<NOT:$<BOOL:${CYGWIN}>>)
endif()

# --- fs_remove
set(to_remove "${CMAKE_CURRENT_BINARY_DIR}/日本語_remove.txt")
add_test(NAME RemoveTouch COMMAND ${CMAKE_COMMAND} -E touch ${to_remove})
set_tests_properties(RemoveTouch PROPERTIES FIXTURES_SETUP "remove_touch")

set(t remove)
add_executable(test_${t}_cpp test_${t}.cpp)
target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
target_include_directories(test_${t}_cpp PRIVATE ..)
target_link_options(test_${t}_cpp PRIVATE $<$<BOOL:${MINGW}>:-municode>)

add_test(NAME Cpp_${t} COMMAND test_${t}_cpp ${to_remove})
set_tests_properties(Cpp_${t} PROPERTIES
LABELS "Cpp"
FIXTURES_REQUIRED "remove_touch"
SKIP_RETURN_CODE 77
FIXTURES_SETUP remove
)
# --- fs_remove

# C

foreach(t IN ITEMS overflow)

add_executable(test_${t}_c test_${t}.c)
target_link_libraries(test_${t}_c PRIVATE ffilesystem)

add_test(NAME C_${t} COMMAND test_${t}_c)
set_tests_properties(C_${t} PROPERTIES
LABELS "C"
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_c>
FIXTURES_SETUP ${t}
)

endforeach()
