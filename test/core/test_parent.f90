program main

use, intrinsic :: iso_fortran_env, only : stderr => error_unit

use filesystem

implicit none

valgrind : block

integer :: i
type(path_t) :: p1, p2
character(:), allocatable :: p

i = 0

p = parent("")
!! parent("") == "." is a generally accepted definition
if(p /= ".") then
  write(stderr, *) "ERROR: parent empty: " // p, len(p)
  i = i+1
endif

p = parent("/")
if (p /= "/") then
  write(stderr, '(a)') "ERROR: parent(/) idempotent failed: " // p
  i = i+1
endif

p = parent(".")
if(p /= ".") then
  write(stderr, '(a)') "ERROR: parent(.) idempotent failed: " // p
  i = i+1
endif

p = parent("a")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(a): " // p
endif

p = parent("a/")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(a/): " // p
endif

p = parent(".")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(.): " // p
endif

p = parent("./")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(./): " // p
endif

p = parent("..")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(..): " // p
endif

p = parent("../")
if(p /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent(../): " // p
endif

p1 = path_t("a/b/c")
p = p1%parent()
if (len_trim(p) /= 3 .or. p /= "a/b") then
  write(stderr, '(a,i0)') "ERROR: parent failed: " // trim(p) // " expected a/b length: ", len_trim(p)
  i = i+1
endif
p2 = path_t(p1%parent())
if (p2%parent() /= "a") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent nest failed: " // p2%parent()
endif
p2 = path_t("a")
if (p2%parent() /= ".") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent idempotent failed. Expected '.', but got: " // p2%parent()
endif

p = parent("ab/.parent")
if(p /= "ab") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent leading dot filename cwd: " // p
endif

p = parent("ab/.parent.txt")
if(p /= "ab") then
  i = i + 1
  write(stderr, '(a)') "ERROR: parent leading dot filename w/ext: " // p
endif

p = parent("a/b/../.parent.txt")
if(p /= "a/b/..") then
  write(stderr,*) "ERROR: parent leading dot filename w/ext up ",  p
  i = i+1
endif

if(is_windows()) then
  p = parent("c:\a\b\..\.parent.txt")
  if(p /= "c:/a/b/..") then
    write(stderr, '(a)') "ERROR: parent leading dot filename w/ext up " // p
    i = i+1
  endif

  p = parent("x:/")
  !! Python also may give either result
  if(all(p /= [character(3) :: "x:", "x:/"])) then
    write(stderr, '(a)') "ERROR: parent(x:/): " // p
    i = i+1
  endif
endif

end block valgrind

print '(a)', "PASS: parent()"

end program
