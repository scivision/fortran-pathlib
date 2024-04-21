program test_is_dir

use, intrinsic :: iso_fortran_env, only: stderr=>error_unit
use filesystem

implicit none

valgrind : block

character(:), allocatable :: s1, s2
integer :: i


if(is_dir("")) error stop "is_dir empty should be false"

s1 = get_cwd()
if(.not. is_dir(s1)) then
  write(stderr, '(a)') "is_dir(get_cwd()) failed: " // s1
  error stop
endif

if(is_windows()) then
  s1 = root(s1)
  print '(3A,i0)', "root(get_cwd()) = ", s1, " length = ", len_trim(s1)
  if(.not. is_dir(s1)) error stop "is_dir('" // s1 // "') failed"
else
  if(.not. is_dir("/")) error stop "is_dir('/') failed"
endif

if(.not. is_dir(".")) error stop "did not detect '.' as directory"
if(is_file(".")) error stop "detected '.' as file"
call assert_is_dir(".")

if(.not. is_readable(".")) error stop "%is_readable failed on '.'"

allocate(character(max_path()) :: s2)
call get_command_argument(0, s2, status=i)
if(i /= 0) error stop "get_command_argument(0) failed"

if (is_dir(s2)) error stop "detected file as directory"

if(is_dir("not-exist-dir")) error stop "not-exist-dir should not exist"

end block valgrind

end program
