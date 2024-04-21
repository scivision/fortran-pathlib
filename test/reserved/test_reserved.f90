program reserved

use filesystem, only : is_char_device, is_reserved, is_unix, is_windows

implicit none

logical :: b

valgrind: block

character(:), allocatable :: s1

s1 = "a"
if (is_reserved(s1)) error stop "a is not reserved"

s1 = "NUL"
b = is_reserved(s1)
if (is_windows()) then
    if (.not. b) error stop "NUL is reserved on Windows"
else
    if (b) error stop "NUL is not reserved on Unix"
endif

if(is_char_device("a")) error stop "a is not a char device"

b = is_char_device("/dev/null")
if(is_unix()) then
    if (.not. b) error stop "/dev/null is a char device on Unix"
else
    if (b) error stop "/dev/null is not a char device on non-Unix systems"
endif

end block valgrind

end program
