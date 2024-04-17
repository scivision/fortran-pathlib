program main

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

integer :: i, L

valgrind : block

character(:), allocatable :: tfile, tdir

allocate(character(max_path()) :: tfile)

if(is_symlink("not-exist-file")) error stop "is_symlink() should be false for non-existent file"
if(is_symlink("")) error stop "is_symlink('') should be false"

if (command_argument_count() == 0) error stop "please give test link file"
call get_command_argument(1, tfile, status=i, length=L)
if(i /= 0 .or. L == 0) error stop "could not get test link file from command line"

!> symlink file
if(.not.is_symlink(tfile)) then
  write(stderr, '(a)') "is_symlink() should be true for symlink file: " // tfile
  error stop
endif

!> symlink directory
tdir = parent(tfile)
if (.not.is_symlink(tdir)) then
  write(stderr, '(a)') "is_symlink() should be true for symlink directory: " // tdir
  error stop
endif

end block valgrind

print '(a)', "OK: is_symlink"

end program
