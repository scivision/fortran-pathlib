program main

use, intrinsic :: iso_fortran_env, only : stderr => error_unit

use filesystem

implicit none

integer, parameter :: L = 21, N = 16

character(L) :: in(N) =  [character(L) ::  "",  "/", ".", "./", "..", "../", &
  "a", "a/", "a/.", "a/..", "a/b", "a/b/", "a/b/c", &
  "ab/.parent", "ab/.parent.txt", "a/b/../.parent.txt"]
character(L) :: ref(N) = [character(L) ::  ".", "/", ".", ".",  ".",  ".",  &
  ".", ".",  "a", "a",    "a",   "a",    "a/b", &
  "ab",         "ab",            "a/b/.."]

integer :: i, j

i = 0

do j = 1, size(in)
  i = i + check(in(j), ref(j))
end do

if(is_windows()) then
  i = i + check("c:\a\b\..\.parent.txt", "c:/a/b/..")
  i = i + check("x:/", "x:/")
end if

if(i /= 0) error stop "FAIL: parent()"

print '(a)', "PASS: parent()"

contains


integer function check(in1, ref1) result(i)

character(*), intent(in) :: in1, ref1

character(:), allocatable :: s

i = 0

s = parent(in1)

if(s == ref1) return

i = 1
write(stderr, '(a)') "parent("// trim(in1) // ") = " // s // " /= " // ref1

end function

end program
