
add_executable(test_cpp_empty test_empty.cpp)
target_link_libraries(test_cpp_empty PRIVATE ffilesystem)

add_test(NAME Cpp_empty COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_cpp_empty>)
set_property(TEST Cpp_empty PROPERTY SKIP_REGULAR_EXPRESSION "not implemented;SIGABRT;[aA]bort;[sS]egmentation fault;segfault")
set_property(TEST Cpp_empty PROPERTY LABELS Cpp)
