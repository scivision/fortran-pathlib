# ref: https://jcbhmr.me/blog/cosmocc-cmake

set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_SYSTEM_PROCESSOR)

find_program(CMAKE_AR
NAMES cosmoar
REQUIRED
PATH_SUFFIXES bin
HINTS $ENV{COSMO_ROOT}
)
find_program(CMAKE_RANLIB
NAMES cosmoranlib
REQUIRED
PATH_SUFFIXES bin
HINTS $ENV{COSMO_ROOT}
 )

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/cosmocc-override.cmake)

set(ffilesystem_fortran false)
