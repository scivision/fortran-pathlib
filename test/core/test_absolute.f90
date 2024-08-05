program test_absolute

use filesystem

implicit none

valgrind : block

if (is_absolute("")) error stop "blank is not absolute"

if (is_windows()) then
  if (.not. is_absolute("J:/")) error stop "J:/ on Windows should be absolute"
  if (.not. is_absolute("j:/")) error stop "j:/ on Windows should be absolute"
  if (is_absolute("/")) error stop "/ on Windows is not absolute"
else
  if (.not. is_absolute("/")) error stop "/ on Unix should be absolute"
  if (is_absolute("j:/")) error stop "j:/ on Unix is not absolute"
end if

end block valgrind

end program
