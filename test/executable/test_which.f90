program test_which

use filesystem

implicit none

valgrind : block

character(:), allocatable :: buf

if (is_windows()) then
    buf = which("cmd.exe")
else
    buf = which("ls")
endif

print '(a)', "which: " // buf

if (len_trim(buf) == 0) error stop "ERROR:test_exe: which() failed"

if(buf /= which(buf)) error stop "ERROR:test_exe: which(absolute) failed"

if(which("/not/a/path") /= "") error stop "ERROR:test_exe: which(not_a_path) failed"

if(which("") /= "") error stop "ERROR:test_exe: which(empty) failed"

end block valgrind

print '(a)', "OK: which()"

end program
