program test_owner

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

character(:), allocatable :: exe, user, name, group

valgrind : block

exe = getarg(0)
print '(a)', "Executable: " // exe

user = get_username()
print '(a)', "User: " // user

name = get_owner_name(exe)
print '(a)', "Owner name: " // name
if(len_trim(name) == 0) error stop "error: No owner information"

if(.not. is_windows()) then
  group = get_owner_group(exe)
  print '(a)', "Owner group: " // group
  if(len_trim(group) == 0) error stop "error: No group information"
endif

if(user == name) stop "OK: User and owner match"

end block valgrind

write(stderr, '(a)') "User and owner didn't match"

if(getenv("CI") == "true") then
  write(stderr, '(a)') "mismatched username and owner can happen on CI systems"
end if

end program
