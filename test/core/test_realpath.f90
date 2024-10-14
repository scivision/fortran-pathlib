program test_canon

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


call test_realpath()
print '(a)', "OK: realpath full"

contains

subroutine test_realpath()

character(:), allocatable :: p1, p2
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = realpath(".")
L1 = len_trim(p1)
if(L1 == 0) error stop "ERROR: realpath '.' failed"

p2 = get_cwd()
if(p1 /= p2) error stop "ERROR: realpath '.' failed: " // p1 // " /= " // p2

print *, "OK: current dir = ", p1


! -- relative dir
p1 = realpath("..")

L2 = len_trim(p1)
if (L2 >= L1) then
  write(stderr,*) 'ERROR:realpath:relative: up dir not realpath: ~/.. => ' // p1
  error stop
end if
print *, 'OK: realpath(..) = ', p1

! -- relative, non-existing file
p2 = '../' // dummy
p1 = realpath(p2)

L3 = len_trim(p1)
if(L3 == 0) then
  if(is_cygwin()) then
    write(stderr,'(a)') "On Cygwin, realpath() returns empty string for non-existing file"
  else
    error stop "FAILED: relative file: realpath(" // p2 // ") => " // p1
  endif
endif

if(.not. is_cygwin()) then
  L1 = len(dummy)
  if (L2 > 1) L1 = L1 + 1  !< when L2==1, $HOME is like /root instead of /home/user

  if (L3 - L2 /= L1) then
    write(stderr,*) 'ERROR relative file was not realpath: ' // p1, L2, L3, len(dummy)
    error stop
  end if
endif

!> not exist
p1 = realpath("not-exist/dir/..")
p2 = get_cwd() // "/not-exist"

print '(a)', "realpath(not-exist/dir/..) = ", p1

if (p1 /= p2) then
  write(stderr,*) 'ERROR: relative not-exist dir did not realpath: ' // p1 // " /= " // p2
  if(.not. is_cygwin()) error stop
end if

print *, 'OK: canon_file = ', p1

end subroutine test_realpath


end program
