program test_env

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

call test_exists()
print '(a)', "OK fs: exists"

call test_homedir()
print '(a)', "OK fs: homedir"

if (len_trim(get_tempdir()) == 0) error stop "get_tempdir failed"
print '(a)', "OK: get_tempdir: " // get_tempdir()


contains


subroutine test_exists()

type(path_t) :: p1

if(exists("")) error stop "empty does not exist"

p1 = path_t(get_cwd())

if(.not. p1%exists()) error stop "%exists() failed"
if(.not. exists(get_cwd())) error stop "exists(get_cwd) failed"

end subroutine


subroutine test_homedir()

character(:), allocatable :: h, k, buf


if(is_windows()) then
  k = "USERPROFILE"
else
  k = "HOME"
end if

buf = getenv(k)
if (len_trim(buf) == 0) then
  print '(a)', "env var " // k // " not set"
else
  print '(a)', "getenv: " // k // " = " // trim(buf)
endif

h = get_homedir()

if(.not. is_dir(h)) error stop "get_homedir failed: not a directory: " // h

print '(a)', "OK: get_homedir: " // h

end subroutine


end program
