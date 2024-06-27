program test_which

use, intrinsic :: iso_fortran_env, only : stderr => error_unit
use filesystem

implicit none

integer :: argc, i
character(2) :: argv

argc = command_argument_count()

if(argc > 0) then
  call get_command_argument(1, argv, status=i)
  if(i /= 0) error stop "could not get command argument 1"
  read(argv, "(i2)") i
else
  i = 0
endif


if(i /= 0) call setenv("PATH", "")

valgrind : block

character(:), allocatable :: s1, s2, s3

if (is_windows()) then
    s2 = "cmd.exe"
else
    s2 = "ls"
endif

s1 = which(s2)

print '(a)', "which(" // s2 // ") " // s1

if (len_trim(s1) == 0) error stop "ERROR:test_exe: which("//s2//") failed"

deallocate(s2)

if(s1 /= which(s1)) then
  write(stderr,'(a)') "ERROR:test_exe: which(absolute) failed: " // s1 // " /= " // which(s1)
  error stop
endif

if(which("/not/a/path") /= "") error stop "ERROR:test_exe: which(not_a_path) failed"

if(which("") /= "") error stop "ERROR:test_exe: which(empty) failed"

!> relative path (directory component, not just filename)
allocate(character(len=max_path()) :: s2)
call get_command_argument(0, s2)
s1 = "./" // file_name(s2)
s3 = which(s1)
print '(a)', "which(" // s1 // ") = " // s3
if(len_trim(s3) == 0) error stop "ERROR:test_exe: which(relative) failed"

!> relative path not exist parent path (should be empty)
s1 = "not-exist/" // file_name(s2)
s3 = which(s1)
print '(a)', "which(" // s1 // ") = " // s3
if(len_trim(s3) /= 0) error stop "ERROR:test_exe: which(relative_not_exist) failed"

!> cwd priority on Windows only
if(is_windows()) then
  s3 = file_name(s2)
  s1 = which(s3)
  print '(a)', "cwd: which(" // s3 // ")" // " = " // s1
  if (len_trim(s1) == 0) error stop "ERROR:test_exe: which(cwd) failed"
endif

end block valgrind

print '(a)', "OK: which()"

end program
