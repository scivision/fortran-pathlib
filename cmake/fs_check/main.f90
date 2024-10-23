program main

use filesystem_test

implicit none

integer :: argc, L

valgrind : block

character(:), allocatable :: filename

argc = command_argument_count()

call get_command_argument(0, length=L)
allocate(character(L) :: filename)
call get_command_argument(0, filename)

if (has_filename("hello.txt")) then
  print '(a)', "OK: has_filename " // filename
else
  error stop "ERROR: has_filename should be true " // filename
end if

end block valgrind

end program
