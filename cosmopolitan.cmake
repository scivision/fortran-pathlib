# ref: https://jcbhmr.me/blog/cosmocc-cmake

set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_SYSTEM_PROCESSOR)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/cosmocc-override.cmake)

set(ffilesystem_fortran false)
