if(CMAKE_VERSION VERSION_LESS 3.21)
  get_property(not_top DIRECTORY PROPERTY PARENT_DIRECTORY)
  if(not_top)
    set(${PROJECT_NAME}_IS_TOP_LEVEL false)
  else()
    set(${PROJECT_NAME}_IS_TOP_LEVEL true)
  endif()
endif()

option(ffilesystem_cpp "Use C++ filesystem for full functionality" on)
option(ffilesystem_fortran "use the Fortran interaces to C functions" on)
option(ffilesystem_cli "Build CLI" ${${PROJECT_NAME}_IS_TOP_LEVEL})
option(ffilesystem_fallback "Fallback to non-C++ if C++ stdlib is not working")
option(ffilesystem_trace "debug trace output" off)
option(ffilesystem_bench "enable benchmark tests")

option(BUILD_SHARED_LIBS "Build shared libraries")
option(${PROJECT_NAME}_coverage "Code coverage tests")
option(${PROJECT_NAME}_tidy "Run clang-tidy on the code")
option(${PROJECT_NAME}_cppcheck "Run cppcheck on the code")
option(${PROJECT_NAME}_iwyu "Run include-what-you-use on the code")

option(CMAKE_TLS_VERIFY "Verify TLS certificates" on)

option(${PROJECT_NAME}_BUILD_TESTING "Build tests" ${${PROJECT_NAME}_IS_TOP_LEVEL})

file(GENERATE OUTPUT .gitignore CONTENT "*")
