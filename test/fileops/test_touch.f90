program test_touch

use, intrinsic :: iso_c_binding, only : C_LONG_LONG
use filesystem

implicit none

integer(C_LONG_LONG) :: t0

valgrind : block

character(:), allocatable :: fn

type(path_t) :: p

fn = "test_fileop.h5"

! call touch("")  !< error stops

call touch(fn)
call assert_is_file(fn)
t0 = get_modtime(fn)

p = path_t("test_fileop.empty")
call p%touch()
if(.not. p%is_file()) error stop "touch failed"
call assert_is_file(p%path())


if(t0 < 0) error stop "Could not get modtime - is negative integer"
if(t0 == 0) error stop "Could not get modtime - integer 0 value"

end block valgrind

print '(a,1x,i0)', "OK: touch. modtime: ", t0

end program
