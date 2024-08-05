function(fs_check)

set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)

if(NOT DEFINED ${PROJECT_NAME}_abi_ok)

message(CHECK_START "checking that compilers can link C++ Filesystem together")

try_compile(${PROJECT_NAME}_abi_ok
${CMAKE_CURRENT_BINARY_DIR}/fs_check ${CMAKE_CURRENT_LIST_DIR}/fs_check
fs_check
CMAKE_FLAGS -DGNU_stdfs=${GNU_stdfs} -Dffilesystem_fortran:BOOL=${HAVE_Fortran_FILESYSTEM}
)

if(${PROJECT_NAME}_abi_ok)
  message(CHECK_PASS "OK")
else()
  message(CHECK_FAIL "Failed")
  message(WARNING "Disabling C++ filesystem due to ABI-incompatible compilers")
  set(HAVE_CXX_FILESYSTEM false CACHE BOOL "ABI problem with C++ filesystem" FORCE)
  return()
endif()

endif()


if(MINGW AND NOT DEFINED ${PROJECT_NAME}_symlink_code)

message(CHECK_START "check if MinGW C++ filesystem support symlink")

set(_symlink ${CMAKE_CURRENT_BINARY_DIR}/symlink_check)
set(_dummy ${_symlink}/dummy_tgt)

file(MAKE_DIRECTORY ${_symlink})
file(TOUCH ${_dummy})

try_run(${PROJECT_NAME}_symlink_code ${PROJECT_NAME}_symlink_build ${_symlink}
        SOURCES ${CMAKE_CURRENT_LIST_DIR}/fs_check/check_fs_symlink.cpp
        LINK_LIBRARIES "${GNU_stdfs}"
        CXX_STANDARD 17
        ARGS ${_dummy}
)
if(${PROJECT_NAME}_symlink_code EQUAL 0)
    set(${PROJECT_NAME}_WIN32_SYMLINK false CACHE BOOL "MinGW doesn't need workaround")
    message(CHECK_PASS "OK")
else()
    message(CHECK_FAIL "applying workaround")
    set(${PROJECT_NAME}_WIN32_SYMLINK true CACHE BOOL "MinGW needs workaround")
endif()
endif()

endfunction()
