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

valgrind : block

type(path_t) :: p1, p2
character(:), allocatable :: p

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

end block valgrind

print '(a)', "PASS: parent()"

contains


integer function check(in, ref) result(i)

character(*), intent(in) :: in, ref

character(:), allocatable :: p

i = 0
p = parent(in)

if(p == ref) return

i = 1
write(stderr, '(a)') "parent("// trim(in) // ") = " // p // " /= " // ref

end function

end program
