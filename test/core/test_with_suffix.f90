program test_with_suffix

use, intrinsic :: iso_fortran_env, only: stderr=>error_unit
use filesystem

implicit none

valgrind : block

character(:), allocatable :: r

r = with_suffix("", ".h5")
if(r /= ".h5") error stop "with_suffix(,.h5) " // r // " /= .h5"

r = with_suffix("foo.h5", "")
if(r /= "foo") error stop "with_suffix(foo.h5,) " // r // " /= foo"

r = with_suffix(".foo.h5", ".txt")
if(r /= ".foo.txt") error stop "with_suffix(.foo.h5,.txt) " // r // " /= .foo.txt"

r = with_suffix(".h5", "")
if(r /= ".h5") error stop "with_suffix(.h5,) " // r // " /= .h5"

r = with_suffix(".h5", ".h5")
if(r /= ".h5.h5") then
  write(stderr,*) "ERROR: with_suffix(.h5,.h5) " // r
  error stop
end if

r = with_suffix('c:/a/hi.nc', '.h5')
if(r /= 'c:/a/hi.h5') then
  write(stderr,'(a)') "ERROR: with_suffix c:/a/hi.nc to .h5: " // r
  error stop
end if

r = with_suffix("my/file.h5", ".hdf5")
if (r /= "my/file.hdf5") error stop "with_suffix() failed: " // r

r = with_suffix("a/boo", ".h5")
if (r /= "a/boo.h5") error stop "with_suffix() failed: " // r

r = with_suffix("boo", ".h5")
if (r /= "boo.h5") error stop "with_suffix() failed: " // r

!> this is an odd case that would't be expected to be used
r = with_suffix("a/b/c.d/", ".hdf5")
if (r /= "a/b/c.d/.hdf5") error stop "with_suffix() failed: " // r

r = with_suffix("dir.h5/", ".hdf5")
if (r /= "dir.h5/.hdf5") error stop "with_suffix() failed: " // r

end block valgrind

print '(a)', "OK: with_suffix"

end program
