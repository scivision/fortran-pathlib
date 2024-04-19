program test_which

use filesystem

implicit none

valgrind : block

character(:), allocatable :: s1, s2

if (is_windows()) then
    s1 = which("cmd.exe")
else
    s1 = which("ls")
endif

print '(a)', "which: " // s1

if (len_trim(s1) == 0) error stop "ERROR:test_exe: which() failed"

if(s1 /= which(s1)) error stop "ERROR:test_exe: which(absolute) failed"

if(which("/not/a/path") /= "") error stop "ERROR:test_exe: which(not_a_path) failed"

if(which("") /= "") error stop "ERROR:test_exe: which(empty) failed"

!> cwd priority on Windows only
if(is_windows()) then
  allocate(character(len=max_path()) :: s2)
  call get_command_argument(0, s2)
  s2 = file_name(s2)
  print '(a)', "cwd: " // s2
  s1 = which(s2)
  if (len_trim(s1) == 0) error stop "ERROR:test_exe: which(cwd) failed"
endif

end block valgrind

print '(a)', "OK: which()"

end program
