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
end if


if(i /= 0) call setenv("PATH", "")

valgrind : block

character(:), allocatable :: s1, s2, s3

if (is_windows()) then
    s2 = "cmake.exe"
else
    s2 = "ls"
end if

s1 = which(s2)

print '(a)', "which(" // s2 // ") " // s1

if (len_trim(s1) == 0) error stop "ERROR:test_exe: which("//s2//") failed"

deallocate(s2)

if(s1 /= which(s1)) then
  write(stderr,'(a)') "ERROR:test_exe: which(absolute) failed: " // s1 // " /= " // which(s1)
  error stop
end if

if(which("/not/a/path") /= "") error stop "ERROR:test_exe: which(not_a_path) failed"

if(which("") /= "") error stop "ERROR:test_exe: which(empty) failed"

!> relative path (directory component, not just filename)
s2 = getarg(0)
if(len_trim(s2) == 0) error stop "could not get own program name"
s1 = "./" // file_name(s2)
if(.not. is_file(s1)) error stop "ERROR:test_exe: file not found: " // s1
s3 = which(s1)
print '(a)', "which(" // s1 // ") = " // s3
if(len_trim(s3) == 0) then
  write(stderr, '(a,i0)') "ERROR:test_exe: which(relative) failed: " // s3, len_trim(s3)
  error stop "ERROR:test_exe: which(relative) failed"
endif

!> relative path not exist parent path (should be empty)
s1 = "not-exist/" // file_name(s2)
s3 = which(s1)
print '(a)', "which(" // s1 // ") = " // s3
if(len_trim(s3) /= 0) then
  write(stderr,'(a,i0)') "ERROR:test_exe: which(relative_not_exist) failed: " // s3, len_trim(s3)
  error stop "ERROR:test_exe: which(relative_not_exist) failed"
endif

!> cwd priority on Windows only
if(is_windows()) then
  s3 = file_name(s2)
  if (.not. is_file(s3)) error stop "ERROR:test_exe: file not found--is test running in exe directory: " // s3
  s1 = which(s3)
  print '(a)', "cwd: which(" // s3 // ")" // " = " // s1
  if (len_trim(s1) == 0) error stop "ERROR:test_exe: which(cwd) failed"
end if

end block valgrind

print '(a)', "OK: which()"

end program
