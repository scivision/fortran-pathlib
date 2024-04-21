option(ffilesystem_cpp "Use C++ filesystem for full functionality" on)
option(ffilesystem_fortran "use the Fortran interaces to C functions" on)
option(ffilesystem_cli "Build CLI" ${PROJECT_IS_TOP_LEVEL})
option(ffilesystem_fallback "Fallback to non-C++ if C++ stdlib is not working")
option(ffilesystem_trace "debug trace output" off)
option(ffilesystem_bench "enable benchmark tests")

option(BUILD_SHARED_LIBS "Build shared libraries")
option(${PROJECT_NAME}_coverage "Code coverage tests")
option(${PROJECT_NAME}_tidy "Run clang-tidy on the code")
option(${PROJECT_NAME}_cppcheck "Run cppcheck on the code")
option(${PROJECT_NAME}_iwyu "Run include-what-you-use on the code")

option(CMAKE_TLS_VERIFY "Verify TLS certificates" on)


option(${PROJECT_NAME}_BUILD_TESTING "Build tests" ${PROJECT_IS_TOP_LEVEL})

if(PROJECT_IS_TOP_LEVEL AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${PROJECT_BINARY_DIR}/local")
endif()

file(GENERATE OUTPUT .gitignore CONTENT "*")
