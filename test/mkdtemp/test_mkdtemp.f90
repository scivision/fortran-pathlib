program mkd

use, intrinsic :: iso_fortran_env, only : stderr => error_unit
use filesystem, only : make_tempdir, is_dir

implicit none

valgrind: block

character(:), allocatable :: temp_dir

temp_dir = make_tempdir()

if(.not. is_dir(temp_dir)) then
  write(stderr,'(a)') "test_mkdtemp: temp dir not created " // temp_dir
  error stop
end if

print '(a)', "OK: Fortran mkdtemp: " // temp_dir

end block valgrind

end program
