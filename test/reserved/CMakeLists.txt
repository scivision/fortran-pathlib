add_executable(test_reserved_cpp test_reserved.cpp)
target_link_libraries(test_reserved_cpp PRIVATE ffilesystem)
target_compile_definitions(test_reserved_cpp PRIVATE $<$<BOOL:${HAVE_CXX_FILESYSTEM}>:HAVE_CXX_FILESYSTEM>)

add_test(NAME Cpp_reserved COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_reserved_cpp>)
# the -E env launcher is needed to catch subprocess abort for macOS with Clang
set_tests_properties(Cpp_reserved PROPERTIES
SKIP_REGULAR_EXPRESSION "SIGABRT;[aA]bort"
)


if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS reserved safe)
add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE Fortran)
target_compile_options(test_${t}_fortran PRIVATE ${${PROJECT_NAME}_fortran_test_flags})

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
set_property(TEST Fortran_${t} PROPERTY LABELS "Fortran")
endforeach()

endif()

if(HAVE_CXX_FILESYSTEM)

add_executable(test_safe_cpp test_safe.cpp)
target_link_libraries(test_safe_cpp PRIVATE ffilesystem)

add_test(NAME Cpp_safe COMMAND test_safe_cpp)

set_property(TEST Cpp_safe PROPERTY LABELS "Cpp")

endif()
