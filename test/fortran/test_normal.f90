program main

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

valgrind : block

type(path_t) :: p1, p2

p1 = path_t("a//b/../c")
p2 = p1%normal()
if (p2%path() /= "a/c") error stop "normalize failed: " // p2%path()

end block valgrind


end program
