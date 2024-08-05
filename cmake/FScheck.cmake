include(CheckSourceRuns)


function(fs_check)

set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.25 AND NOT DEFINED ffilesystem_abi_ok)

message(CHECK_START "checking that compilers can link C++ Filesystem together")

try_compile(ffilesystem_abi_ok
PROJECT fs_check
SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/fs_check
CMAKE_FLAGS -DGNU_stdfs=${GNU_stdfs} -Dffilesystem_fortran:BOOL=${HAVE_Fortran_FILESYSTEM}
)

if(ffilesystem_abi_ok)
  message(CHECK_PASS "OK")
else()
  message(CHECK_FAIL "Failed")
  message(WARNING "Disabling C++ filesystem due to ABI-incompatible compilers")
  set(HAVE_CXX_FILESYSTEM false CACHE BOOL "ABI problem with C++ filesystem" FORCE)
endif()

endif()


if(NOT HAVE_CXX_FILESYSTEM)
  return()
endif()


if(MINGW)

set(CMAKE_CXX_STANDARD 17)  # need this for check_source_runs

file(READ ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fs_check/check_fs_symlink.cpp _src)

check_source_runs(CXX "${_src}" ffilesystem_symlink_ok)

set(ffilesystem_WIN32_SYMLINK $<NOT:$<BOOL:${ffilesystem_symlink_ok}>> PARENT_SCOPE)

endif()

endfunction()
