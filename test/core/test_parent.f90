program main

use, intrinsic :: iso_fortran_env, only : stderr => error_unit

use filesystem

implicit none

integer, parameter :: L = 21, N = 14

character(L) :: in(N) =  [character(L) ::  "",  "/", ".", "./", "..", "../", "a", "a/", "a/b", "a/b/", "a/b/c", &
  "ab/.parent", "ab/.parent.txt", "a/b/../.parent.txt"]
character(L) :: ref(N) = [character(L) ::  ".", "/", ".", ".",  ".",  ".",   ".", ".",  "a",   "a",    "a/b", &
  "ab",         "ab",            "a/b/.."]

integer :: i, j

i = 0

do j = 1, size(in)
  i = i + check(in(j), ref(j))
end do

if(is_windows()) then
  i = i + check("c:\a\b\..\.parent.txt", "c:/a/b/..")

  if (all([check("x:/", "x:/"), check("x:/", "x:")] == 1)) then
    write(stderr, '(a)') "ERROR: parent(x:/)"
    i = i+1
  endif
  !! Python also may give either result
endif

if(i /= 0) error stop "FAIL: parent()"

print '(a)', "PASS: parent()"

contains


integer function check(in, ref) result(i)

character(*), intent(in) :: in, ref

character(:), allocatable :: s, s1
type(path_t) :: p

i = 0

s = parent(in)

p = path_t(in)
s1 = p%parent()

if(s == ref .and. s1 == ref) return

i = 1
write(stderr, '(a)') "parent("// trim(in) // ") = " // s // " /= " // ref
write(stderr, '(a)') "path_t("// trim(in) // ")%parent() = " // s1 // " /= " // ref

end function

end program
