program test_is_abs

use filesystem

if (is_absolute("")) error stop "blank is not absolute"

if (is_windows()) then
    if (.not. is_absolute("J:/")) error stop "is_absolute(J:/) on Windows should be true"
    if (.not. is_absolute("j:/")) error stop "is_absolute(j:/) on Windows should be true"
    if (is_absolute("/")) error stop "is_absolute(/) on Windows should be false"
else
    if (.not. is_absolute("/")) error stop "is_absolute(/) on Unix should be true"
    if (is_absolute("j:/")) error stop "is_absolute(j:/) on Unix should be false"
end if

end program
