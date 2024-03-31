program test_windows

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

integer :: i

character(:), allocatable :: buf, buf2, buf3

allocate(character(max_path()) :: buf, buf2, buf3)


call get_environment_variable("PROGRAMFILES", buf, status=i)
if (i /= 0) then
    write(stderr, '(a)') "Error getting PROGRAMFILES"
    error stop 77
endif

print '(a)', "PROGRAMFILES: " // trim(buf)

buf2 = shortname(buf)
print '(a)', trim(buf) // " => " // trim(buf2)
if(len_trim(buf2) == 0) then
    write(stderr, '(a)') "Error converting long path to short path: " // trim(buf2)
    error stop
endif

buf3 = longname(buf2)
print '(a)', trim(buf2) // " => " // trim(buf3)
if(len_trim(buf3) == 0) then
    write(stderr, '(a)') "Error converting short path to long path: " // trim(buf3)
    error stop
endif

if(buf /= buf3) then
    write(stderr, '(a)') "Error: shortname(longname(x)) != x"
    error stop
endif

end program
