program test_root

use, intrinsic :: iso_fortran_env, only: stderr => error_unit
use filesystem

implicit none

if(root("") /= "") error stop "root empty"

if(.not. is_windows() .and. root("/a/b") /= "/") error stop "relative root should be empty"

print '(A)', "OK: root: C++"

end program
