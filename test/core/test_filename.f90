program main

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p

p = file_name("")
if(p /= "") error stop "FAILED filename('')"

p = file_name("a/b/c")
if(p /= "c") error stop "FAILED filename('a/b/c') " // p

end block valgrind

print '(a)', "OK: Fortran filename()"

end program
