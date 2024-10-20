if(TARGET fs_cli)

add_test(NAME Cpp_CLI_Cpp
COMMAND ${CMAKE_COMMAND}
-Dexe=$<TARGET_FILE:fs_cli>
"-Dregex=^Ffs\> 1"
-P ${CMAKE_CURRENT_SOURCE_DIR}/../stdin.cmake
)

endif()

set(tests file system utf8)

foreach(t IN LISTS tests)
  add_executable(test_cpp_${t} test_${t}.cpp)
  target_link_libraries(test_cpp_${t} PRIVATE ffilesystem)

  add_test(NAME Cpp_${t} COMMAND test_cpp_${t})
  set_tests_properties(Cpp_${t} PROPERTIES
    LABELS "Cpp"
    SKIP_RETURN_CODE 77
    )
endforeach()

set_tests_properties(Cpp_file PROPERTIES
DISABLED $<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_SYS_STATVFS}>>>
)
