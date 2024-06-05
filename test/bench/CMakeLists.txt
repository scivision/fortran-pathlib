set_property(DIRECTORY PROPERTY LABELS "bench")

add_executable(ffilesystem_bench ffilesystem_bench.cpp)

target_include_directories(ffilesystem_bench PRIVATE ${PROJECT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ffilesystem_bench PRIVATE ffilesystem)

target_compile_definitions(ffilesystem_bench PRIVATE HAVE_CXX_FILESYSTEM=$<BOOL:${HAVE_CXX_FILESYSTEM}>)

add_test(NAME FfilesystemBenchmark COMMAND ffilesystem_bench)
set_property(TEST FfilesystemBenchmark PROPERTY RUN_SERIAL TRUE)
