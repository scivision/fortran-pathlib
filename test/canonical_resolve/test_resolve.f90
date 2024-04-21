program test_res

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


call test_resolve()
print '(a)', "OK: resolve full"

contains

subroutine test_resolve()

character(:), allocatable :: p1, p2
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = resolve(".")
L1 = len_trim(p1)
if (L1 < 1) then
  write(stderr,*) "ERROR: resolve '.' " // p1
  error stop
endif

if (p1 /= get_cwd()) then
  write(stderr,*) "ERROR: resolve('.') " // p1 // " /= get_cwd: " // get_cwd()
  error stop
endif

print *, "OK: current dir = ", p1

! -- home directory
p1 = resolve("~")
if (p1(1:1) == "~") error stop "%resolve ~ did not expanduser: " // p1
print *, "OK: home dir = ", p1

p2 = parent(p1)
L2 = len_trim(p2)
if (L2 >= L1) error stop "parent home " // p2
print *, "OK: parent(resolve(~)) = ", p2


! -- relative dir
p1 = resolve("~/..")
L3 = len_trim(p1)
if (L2 /= L3) then
  write(stderr,*) 'ERROR:resolve:relative: up dir not resolved: ~/.. => ' // p1, L3, L2
  error stop
endif
print *, 'OK: canon_dir = ', p1

! -- relative, non-existing file
if(is_cygwin()) then
  print '(a)', 'skip relative file not-exist as Cygwin does not support it'
else
p1 = resolve('~/../' // dummy)
L3 = len_trim(p1)
if(L3 == 0) error stop "ERROR: relative file did not resolve: " // p1

L1 = len(dummy)
if (L2 > 1) L1 = L1 + 1  !< when L2==1, $HOME is like /root instead of /home/user

if (L3 - L2 /= L1) then
  write(stderr,*) 'ERROR relative file was not resolved: ' // p1
  error stop
endif
endif

!> empty
if(resolve("") /= get_cwd()) then
  write(stderr,*) "resolve('') " // resolve("") // " /= " // get_cwd()
  error stop
end if

print '(a)', 'OK: resolve'

end subroutine


end program
