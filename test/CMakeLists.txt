include(CheckIncludeFile)

if(LINUX)
  check_include_file("linux/magic.h" ffilesystem_HAVE_LINUX_MAGIC)
endif()

if(UNIX)
  check_include_file("sys/statvfs.h" ffilesystem_HAVE_SYS_STATVFS)
endif()

set(CMAKE_CXX_STANDARD ${cxx_std})

add_compile_definitions($<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>)

set(${PROJECT_NAME}_fortran_test_flags
  "$<$<AND:$<Fortran_COMPILER_ID:GNU>,$<VERSION_LESS:$<Fortran_COMPILER_VERSION>,13>>:-Wno-maybe-uninitialized>"
)

if(ffilesystem_bench)
  add_subdirectory(bench)
endif()

foreach(t IN ITEMS cpp fortran core empty environment
  executable exepath
  fileops libpath overflow permissions relative reserved symlink windows)

add_subdirectory(${t})

# --- Windows shared DLLs
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)

  get_property(n DIRECTORY ${t} PROPERTY TESTS)

  # tests run much faster in serial, especially on Windows
  set_property(TEST ${n} DIRECTORY ${t} PROPERTY RUN_SERIAL true)

  if(BUILD_SHARED_LIBS AND (CYGWIN OR WIN32))
    set_property(TEST ${n} DIRECTORY ${t}
    PROPERTY ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:ffilesystem>"
    )
  endif()
endif()

endforeach()

# --- coverage

if(${PROJECT_NAME}_coverage)
  setup_target_for_coverage_gcovr_html(
  NAME coverage
  EXECUTABLE ${CMAKE_CTEST_COMMAND}
  )
endif()
