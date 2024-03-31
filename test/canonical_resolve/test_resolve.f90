program test_res

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


call test_resolve()
print '(a)', "OK: resolve full"

contains

subroutine test_resolve()

type(path_t) :: cur, par, file, p1, p2
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
cur = path_t(".")
cur = cur%resolve()
L1 = cur%length()
if (L1 < 1) then
  write(stderr,*) "ERROR: resolve '.' " // cur%path()
  error stop
endif

if (cur%path() /= get_cwd()) then
  write(stderr,*) "ERROR: resolve('.') " // cur%path() // " /= get_cwd: " // get_cwd()
  error stop
endif

print *, "OK: current dir = ", cur%path()
! -- home directory
p1 = path_t("~")
p1 = p1%resolve()
if (p1%path(1,1) == "~") error stop "%resolve ~ did not expanduser: " // p1%path()
if (resolve("~") == "~") error stop "resolve('~') should not be '~'"
print *, "OK: home dir = ", p1%path()

p2 = path_t(p1%parent())
L1 = p2%length()
if (L1 >= p1%length()) error stop "parent home " // p2%path()
print *, "OK: parent(resolve(~)) = ", p2%path()


! -- relative dir
par = path_t("~/..")
par = par%resolve()

L2 = par%length()
if (L2 /= L1) then
  write(stderr,*) 'ERROR:resolve:relative: up dir not resolved: ~/.. => ' // par%path(), L1, L2
  error stop
endif
print *, 'OK: canon_dir = ', par%path()

! -- relative, non-existing file
if(is_cygwin()) then
  print '(a)', 'skip relative file not-exist as Cygwin does not support it'
else
file = path_t('~/../' // dummy)
file = file%resolve()
if(file%length() == 0) error stop "ERROR: relative file did not resolve: " // file%path()
L3 = file%length()

L1 = len(dummy)
if (L2 > 1) L1 = L1 + 1  !< when L2==1, $HOME is like /root instead of /home/user

if (L3 - L2 /= L1) then
  write(stderr,*) 'ERROR relative file was not resolved: ' // file%path(), L2, par%path(), L3, len(dummy)
  error stop
endif
endif

!> empty
if(resolve("") /= get_cwd()) then
  write(stderr,*) "resolve('') " // resolve("") // " /= " // get_cwd()
  error stop
end if

print *, 'OK: resolved = ', file%path()

end subroutine


end program
