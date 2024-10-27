program test

use filesystem

implicit none

valgrind : block

character(:), allocatable :: s

if (is_empty(".")) error stop "is_empty(.) should be false generally"

call mkdir("test-empty-dir")
if (.not. is_empty("test-empty-dir")) error stop "is_empty(test-empty-dir) should be true"

call touch("test-empty-file")
if (.not. is_empty("test-empty-file")) error stop "is_empty(test-empty-file) should be true"

s = getarg(1)
if (is_empty(s)) error stop "is_empty should be false for " // s

end block valgrind

print '(a)', "OK: is_empty Fortran"

end program
