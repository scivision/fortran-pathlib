program main

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: tfile, tdir

if (command_argument_count() == 0) error stop "please give test link file"

tfile = getarg(1)

if(is_symlink("not-exist-file")) error stop "is_symlink() should be false for non-existent file"
if(is_symlink("")) error stop "is_symlink('') should be false"

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
