program main

use filesystem

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

implicit none

integer, parameter :: L = 21, N = 14+6

character(L) :: in1(N) = [character(L) :: "", "",  "/", "/", "Hello", "Hello",  "/dev/null", "/a/b", &
    "/a/b", "/a/b", "/a/b/c/d", "./this/one", "/this/one", "/path/same", &
    "c:\a\b", "x", "c:/a/b", "c:/a/b", "c:\a/b\c/d", "c:/path"]
character(L) :: in2(N) = [character(L) :: "", "/", "",  "/", "Hello", "Hello/", "/dev/null", "c", &
    "/a/b", "/a",   "/a/b",     "./this/two", "/this/two", "/path/same/hi/..", &
    "x",      "c:/a/b", "c:/a/b", "c:/a", "c:/a\b",   "x:/path"]
character(L) :: ref(N) = [character(L) :: "", "/",  "",  ".", ".",     ".",      ".",        "c", &
    ".",    "..",   "../..",    "../two",     "../two", ".", &
    "x",      "c:/a/b",       ".",      "..",   "../..", "x:/path"]

integer :: i, j, k

i = 0

k = 14
if(is_windows()) k = N

do j = 1, k
    i = i + check(in1(j), in2(j), ref(j))
end do

if(.not. is_windows()) then
  i = i + check("c", "/a/b", "/a/b")
endif


if(i /= 0) error stop "FAIL: file_name()"

print '(a)', "PASS: proximate_to()"

contains


integer function check(in1, in2, ref) result(i)

character(*), intent(in) :: in1, in2, ref

character(:), allocatable :: s, s1
type(path_t) :: p

i = 0

s = proximate_to(in1, in2)

p = path_t(in1)
s1 = p%proximate_to(in2)

if(s == ref .and. s1 == ref) return

i = 1
write(stderr, '(a)') "proximate_to("// trim(in1) // ", "//trim(in2)// ") = " // s // " /= " // ref
write(stderr, '(a)') "path_t("// trim(in1) // ")%proximate(" // trim(in2)//") = " // s1 // " /= " // ref

end function

end program
