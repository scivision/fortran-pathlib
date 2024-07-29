program test_owner

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

character(:), allocatable :: exe, user, owner

valgrind : block

exe = getarg(0)
print '(a)', "Executable: " // exe

user = get_username()
print '(a)', "User: " // user

owner = get_owner(exe)
print '(a)', "Owner: " //  owner
if(len_trim(owner) == 0) error stop "error: No owner information"

if(user == owner) stop "OK: User and owner match"

end block valgrind

write(stderr, '(a)') "User and owner didn't match"

if(getenv("CI") == "true") then
  write(stderr, '(a)') "mismatched username and owner can happen on CI systems"
endif

end program
