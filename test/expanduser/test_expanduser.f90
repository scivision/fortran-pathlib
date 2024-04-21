program test_expanduser

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none


if(expanduser("") /= "") error stop "expanduser blank failed"
if(expanduser(".") /= ".") error stop "expanduser dot failed: " // expanduser(".")

if(expanduser("~P") /= "~P") error stop "expanduser ~P failed: " // expanduser("~P")

valgrind: block

character(:), allocatable :: s1, s2

!> does expanduser() get homedir correctly
s1 = expanduser("~")
if(s1 /= get_homedir()) then
   write(stderr, '(a)') "expanduser(~) failed: " // s1 // " /= " // get_homedir()
   error stop
endif

!> idempotent
s2 = expanduser(s1)
if (s1 /= s2) then
  write(stderr, '(a)') "expanduser() idempotent failed: " // s1 // " /= " // s2
  error stop
endif

!> separator
s2 = expanduser("~/")
if (s1 /= s2) error stop "expanduser trailing separator failed: " // s1 // " /= " // s2

if (expanduser("~//") /= s2) error stop "expanduser double separator failed: " // &
   expanduser("~//") // " /= " // s2

!> the C code is drastically simpler by using CWalk join that normalizes
if(fs_cpp()) then
  s1 = get_homedir() // "/.."
else
  s1 = parent(get_homedir())
endif

!> double dot
s2 = expanduser("~/..")

if (s2 /= s1) error stop "expanduser(~/..) failed: " // s2 // " /= " // s1

s2 = expanduser("~/../")
if (s2 /= s1) error stop "expanduser(~/../) failed: " // s2 // " /= " // s1

!> double dot separator
s2 = expanduser("~//..")
if(s2 /= s1) error stop "expanduser ~//.. failed: " // s2 // " /= " // s1


end block valgrind

print *, "OK: filesystem: expanduser  ", expanduser("~")

end program
