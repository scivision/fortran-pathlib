program test_root

use, intrinsic :: iso_fortran_env, only: stderr => error_unit
use filesystem

implicit none

valgrind : block
character(:), allocatable :: r

if(root("") /= "") error stop "root empty"

if(root("a/b") /= "") error stop "relative root should be empty"
if(root("./a/b") /= "") error stop "relative root ./a should be empty"
if(root("../a/b") /= "") error stop "relative root ../a should be empty"

if(is_windows()) then
  r = root("/etc")
  if(r /= "") then
  write(stderr,'(a,i0)') "ERROR: windows root /etc failed: "// r // " length: ", len_trim(r)
  error stop
  end if

  r = root("c:/")
  if(r /= "c:/") then
  write(stderr, '(a)') "ERROR: windows root c:/ failed: " // r
  error stop
  end if

  r = root("c:\")
  if(r /= "c:/") then
  write(stderr, '(a)') "ERROR: windows root c:\ failed: " // r
  error stop
  end if


  r = root("c:/etc")
  if(r /= "c:/") then
  write(stderr, '(a)') "ERROR: windows root c:/etc failed: " // r
  error stop
  end if
else
  r = root("c:/etc")
  if(r /= "") error stop "unix root c:/etc failed : " // r

  r = root("/etc")
  if(r /= "/") error stop "unix root /etc failed: " // r
end if

end block valgrind

print '(A,L1)', "OK: root: C++: ", fs_cpp()

end program
