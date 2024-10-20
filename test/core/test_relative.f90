program main

use filesystem

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

implicit none

integer, parameter :: L = 21, N = 15

character(L) :: in1(N) = [character(L) :: "", "",  "/", "/", "Hello", "Hello",  "/dev/null", "/a/b", "c", &
    "/a/b", "/a/b", "/a/b/c/d", "./this/one", "/this/one", "/path/same"]
character(L) :: in2(N) = [character(L) :: "", "/", "",  "/", "Hello", "Hello/", "/dev/null", "c",    "/a/b", &
    "/a/b", "/a",   "/a/b",     "./this/two", "/this/two", "/path/same/hi/.."]
character(L) :: ref(N) = [character(L) :: ".", "",  "",  ".", ".",     ".",      ".",         "",     "", &
    ".",    "..",   "../..",    "../two",     "../two", "."]

integer, parameter :: Nw = 9

character(L) :: in1w(Nw) = [character(L) :: "", "Hello", "Hello", &
"c:\a\b", "c:/", "c:/a/b", "c:/a/b", "c:\a/b\c/d", "c:/path"]
character(L) :: in2w(Nw) = [character(L) :: "", "Hello", "Hello/", &
"c:/", "c:/a/b", "c:/a/b", "c:/a", "c:/a\b", "d:/path"]
character(L) :: refw(Nw) = [character(L) :: ".", ".", ".", "../..", "a/b", ".", "..", "../..", ""]

integer :: i, j

i = 0

if(is_windows()) then
  do j = 1, Nw
    i = i + check(in1w(j), in2w(j), refw(j))
  end do
else
  do j = 1, N
    i = i + check(in1(j), in2(j), ref(j))
  end do
end if

if(i /= 0) error stop "FAIL: relative_to()"

print '(a)', "PASS: relative_to()"

contains


integer function check(in1, in2, ref) result(i)

character(*), intent(in) :: in1, in2, ref

character(:), allocatable :: s

i = 0

s = relative_to(in1, in2)

if(s == ref) return

i = 1
write(stderr, '(a)') "FAIL: relative_to("// trim(in1) // ", "//trim(in2)// ") = " // s // " /= " // ref

end function

end program
