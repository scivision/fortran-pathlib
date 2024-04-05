program main

use filesystem

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

implicit none

integer, parameter :: L = 21, N = 15+6

character(L) :: in1(N) = [character(L) :: "", "",  "/", "/", "Hello", "Hello",  "/dev/null", "/a/b", "c", &
    "/a/b", "/a/b", "/a/b/c/d", "./this/one", "/this/one", "/path/same", &
    "c:\a\b", "c", "c:/a/b", "c:/a/b", "c:\a/b\c/d", "c:/path"]
character(L) :: in2(N) = [character(L) :: "", "/", "",  "/", "Hello", "Hello/", "/dev/null", "c",    "/a/b", &
    "/a/b", "/a",   "/a/b",     "./this/two", "/this/two", "/path/same/hi/..", &
    "c", "c:/a/b", "c:/a/b", "c:/a", "c:/a\b", "d:/path"]
character(L) :: ref(N) = [character(L) :: "", "",  "",  ".", ".",     ".",      ".",         "",     "", &
    ".",    "..",   "../..",    "../two",     "../two", ".", &
    "",  "",       ".",      "..",   "../.."  , ""]

integer :: i, j, k

i = 0

k = 15
if(is_windows()) k = 21

do j = 1, k
  i = i + check(in1(j), in2(j), ref(j))
end do


if(i /= 0) error stop "FAIL: relative_to()"

print '(a)', "PASS: relative_to()"

contains


integer function check(in1, in2, ref) result(i)

character(*), intent(in) :: in1, in2, ref

character(:), allocatable :: s, s1
type(path_t) :: p

i = 0

s = relative_to(in1, in2)

p = path_t(in1)
s1 = p%relative_to(in2)

if(s == ref .and. s1 == ref) return

i = 1
write(stderr, '(a)') "relative_to("// trim(in1) // ", "//trim(in2)// ") = " // s // " /= " // ref
write(stderr, '(a)') "path_t("// trim(in1) // ")%relative(" // trim(in2)//") = " // s1 // " /= " // ref

end function

end program