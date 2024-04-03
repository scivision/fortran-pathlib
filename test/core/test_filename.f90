program main

use filesystem

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit

implicit none

integer, parameter :: L = 21, N = 15

character(L) :: in(N) =  [character(L) ::  "", "/", ".", "./", "..", "../", "a", "a/", "a/b", "a/b/", "a/b/c", &
  "ab/.parent", "ab/.parent.txt", "a/b/../.parent.txt", "./fil"]
character(L) :: ref(N) = [character(L) ::  "", "",  ".", "",  "..",  "",    "a", "",   "b",   "",    "c", &
  ".parent",    ".parent.txt",   ".parent.txt", "fil"]

integer :: i, j

i = 0

do j = 1, size(in)
  i = i + check(in(j), ref(j))
end do

if(is_windows()) then
  i = i + check("C:/", "")
  i = i + check("C:\ab\asb", "asb")
endif

if(i /= 0) error stop "FAIL: file_name()"

print '(a)', "PASS: filename()"

contains


integer function check(in, ref) result(i)

character(*), intent(in) :: in, ref

character(:), allocatable :: s, s1
type(path_t) :: p

i = 0

s = file_name(in)

p = path_t(in)
s1 = p%file_name()

if(s == ref .and. s1 == ref) return

i = 1
write(stderr, '(a)') "file_name("// trim(in) // ") = " // s // " /= " // ref
write(stderr, '(a)') "path_t("// trim(in) // ")%file_name() = " // s1 // " /= " // ref

end function

end program
