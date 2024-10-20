add_executable(test_C_overflow test_overflow.c)
target_link_libraries(test_C_overflow PRIVATE ffilesystem)

add_test(NAME C_overflow COMMAND test_C_overflow)
