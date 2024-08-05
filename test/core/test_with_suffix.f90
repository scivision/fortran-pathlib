program test_with_suffix

use, intrinsic :: iso_fortran_env, only: stderr=>error_unit
use filesystem

implicit none

valgrind : block

character(:), allocatable :: b

if(with_suffix("", ".h5") /= ".h5") error stop "with_suffix empty: " // with_suffix("", ".h5")
if(with_suffix("foo.h5", "") /= "foo") error stop "with_suffix foo.h5 to empty: " // with_suffix("foo.h5", "")
if(with_suffix(".h5", "") /= ".h5") error stop "with_suffix .h5 to .h5"
if(with_suffix(".h5", ".h5") /= ".h5.h5") then
  write(stderr,*) "ERROR: with_suffix .h5.h5: " // with_suffix(".h5", ".h5")
  error stop
end if

b = with_suffix('c:/a/hi.nc', '.h5')
if(b /= 'c:/a/hi.h5') then
  write(stderr,'(a)') "ERROR: with_suffix c:/a/hi.nc to .h5: " // b
  error stop
end if

b = with_suffix("my/file.h5", ".hdf5")
if ("my/file.hdf5" /= b) error stop "with_suffix() failed: " // b

end block valgrind

end program
