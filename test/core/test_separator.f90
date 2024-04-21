program test_separator

use, intrinsic :: iso_fortran_env, only: stderr=>error_unit
use filesystem

implicit none

integer :: c = 0

valgrind : block

character(:), allocatable :: b
character :: sep

sep = pathsep()

if(is_windows() .and. .not. sep == ";") then
  write(stderr,*) "ERROR: pathsep: ", sep
  c = c+1
endif

if(.not. is_windows() .and. .not. sep == ":") then
  write(stderr,*) "ERROR: pathsep: ", sep
  c = c+1
endif

b = as_posix("")
if (b /= "") then
  write(stderr,*) "ERROR: as_posix empty: " // b, len_trim(b)
  c = c+1
endif

if(is_windows()) then

b = as_posix("a" // char(92) // "b")
if (b /= "a/b") then
  write(stderr,*) "ERROR: as_posix(a" // char(92) // "b): " // b
  c = c+1
endif

endif

end block valgrind

if (c /= 0) error stop "failed separator tests"

print '(a)', "OK: separator"

end program
