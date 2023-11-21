option(cpp "Use C++ filesystem for full functionality" on)
option(fortran "use the Fortran interaces to C functions" on)
option(cli "Build CLI" on)
option(BUILD_SHARED_LIBS "Build shared libraries")
option(fallback "Fallback to non-C++ filesystem.c if C++ stdlib is not working")

option(coverage "Code coverage tests")
option(tidy "Run clang-tidy on the code")
option(cppcheck "Run cppcheck on the code")
option(iwyu "Run include-what-you-use on the code")

option(CMAKE_TLS_VERIFY "Verify TLS certificates" on)

include(GNUInstallDirs)

# Necessary for shared library with Visual Studio / Windows oneAPI
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

file(GENERATE OUTPUT .gitignore CONTENT "*")
