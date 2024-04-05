program test_path

use filesystem

implicit none

valgrind : block

character(:), allocatable :: path

path = exe_path()

if(len_trim(path) == 0) error stop "exe_path() is empty"

if(.not. is_file(path)) error stop "exe_path() is not a file: " // path

print '(a)', path

end block valgrind

end program
