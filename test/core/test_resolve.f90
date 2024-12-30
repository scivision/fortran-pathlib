program test_res

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


valgrind : block

character(:), allocatable :: p1, p2, cwd
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = resolve(".")
L1 = len_trim(p1)
if (L1 < 1) then
  write(stderr,*) "ERROR: resolve '.' " // p1
  error stop
end if

cwd = get_cwd()

if (p1 /= cwd) then
  write(stderr,*) "ERROR: resolve('.') " // p1 // " /= " // cwd
  error stop
end if

print *, "OK: current dir = ", p1

! -- home directory
p2 = get_homedir()
p1 = resolve("~")

if (p1(1:1) == "~") then
  write(stderr,'(a)') "%resolve ~ did not expanduser: " // p1
  error stop
end if
if(p1 /= p2) then
  write(stderr,*) "ERROR: resolve('~') " // p1 // " /= get_homedir: " // p2
  error stop
end if
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
end if
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
end if
end if

!> empty
if(resolve("") /= cwd) then
  write(stderr,*) "resolve('') " // resolve("") // " /= " // cwd
  error stop
end if

!> not strict, not exist
p1 = resolve("not-exist/dir/..")

if (p1 /= cwd // "/not-exist") then
  !! not a trailing slash input to avoid ambiguity in various backends
  write(stderr,*) 'failed: resolve/.. dir did not resolve: ' // p1
  error stop
end if

!> strict, not exist
if(backend() == "<filesystem>") then
p1 = resolve("not-exist/dir/..", strict=.true.)
!! not a trailing slash input to avoid ambiguity in various backends
if (len_trim(p1) /= 0) then
  write(stderr,*) 'failed: resolve(strict not-exist should return empty string: ' // p1
  error stop
end if
endif

end block valgrind

print '(a)', 'OK: resolve'

end program
