program test_canon

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


call test_canonical()
print '(a)', "OK: canonical full"

contains

subroutine test_canonical()

character(:), allocatable :: p1, p2
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = canonical(".")
L1 = len_trim(p1)
if(L1 == 0) error stop "ERROR: canonical '.' failed"

p2 = get_cwd()
if(p1 /= p2) error stop "ERROR: canonical '.' failed: " // p1 // " /= " // p2

print *, "OK: current dir = ", p1

! -- home directory
p1 = canonical("~")
L1 = len_trim(p1)
if (p1(1:1) == "~") error stop "canonical(~) did not expanduser: " // p1

p2 = get_homedir()
if(p1 /= p2) error stop "ERROR: canonical(~) failed: " // p1 // " /= " // p2

print *, "OK: home dir = ", p1

! -- relative dir
p1 = canonical("~/..")

L2 = len_trim(p1)
if (L2 >= L1) then
  write(stderr,*) 'ERROR:canonical:relative: up dir not canonicalized: ~/.. => ' // p1
  error stop
end if
print *, 'OK: canon_dir = ', p1

! -- relative, non-existing file
p1 = canonical('~/../' // dummy)

L3 = len_trim(p1)
if(L3 == 0) error stop "ERROR: relative file did not canonicalize: " // p1

L1 = len(dummy)
if (L2 > 1) L1 = L1 + 1  !< when L2==1, $HOME is like /root instead of /home/user

if (L3 - L2 /= L1) then
  write(stderr,*) 'ERROR relative file was not canonicalized: ' // p1, L2, L3, len(dummy)
  error stop
end if

p1 = canonical('../' // dummy)

if (p1 /= "../" // dummy) then
  write(stderr,*) 'ERROR: relative file did not canonicalize: ' // p1
  error stop
end if

!> not strict, not exist
p1 = canonical("not-exist/dir/../")

if (p1 /= "not-exist") then
  write(stderr,*) 'ERROR: relative dir did not canonicalize: ' // p1
  error stop
end if

!> strict, not exist
p1 = canonical("not-exist/dir/../", strict=.true.)
if (len_trim(p1) /= 0) then
  write(stderr,*) 'ERROR: strict not-exist should return empty string: ' // p1
  error stop
end if

!> empty
p2 = canonical("")
if(p2 /= "") error stop "canonical('') not empty: " // p2

print *, 'OK: canon_file = ', p1

end subroutine test_canonical


end program
