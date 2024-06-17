program test_copy

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

call test_copyfile()
print '(a)', "OK: copy_file"

contains

subroutine test_copyfile()

character(*), parameter :: s1 = "dummy.txt", s2 = "dummy.txt.copy"
logical :: ok

call touch(s1, ok)
if(.not. ok) then
  write(stderr, '(a)') "touch("//s1//") failed"
  error stop 77
endif

!> copy a file
call copy_file(s1, s2, overwrite=.true., ok=ok)
if(.not. is_file(s2)) error stop "did not detect file after copy " // s2
if(.not. ok) error stop "copy_file ok logical is false despite success of copy " // s2

!> empty target
! omitted because this fails when ABI shaky e.g. macOS with Clang+Gfortran
! call copy_file(s1, "", status=i)
! if(i==0) error stop "copy_file should fail on empty target"

end subroutine test_copyfile

end program
