set_directory_properties(PROPERTIES LABELS "extra")

set(extra_tests)

if(ffilesystem_locale)
  list(APPEND extra_tests locale)
endif()

foreach(t IN LISTS extra_tests)

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


if(TEST Cpp_locale)

set_tests_properties(Cpp_locale PROPERTIES
SKIP_REGULAR_EXPRESSION "locale\:\:facet\:\:\_S\_create\_c\_locale name not valid"
)

endif()
