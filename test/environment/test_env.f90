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

call test_user_config_dir()
print '(a)', "OK fs: user_config_dir"


contains


subroutine test_exists()

if(exists("")) error stop "empty does not exist"

if(.not. exists(get_cwd())) error stop "exists(get_cwd) failed"

end subroutine


subroutine test_homedir()

character(:), allocatable :: h, p, k, buf

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
if (len_trim(h) == 0) error stop "get_homedir failed"

p = get_profile_dir()
if (len_trim(p) == 0) error stop "get_profile_dir failed"

if(.not. is_dir(h)) error stop "get_homedir failed: not a directory: " // h

if (p /= h) error stop "get_profile_dir failed: " // p // " /= " // h

print '(a)', "OK: get_homedir: " // h

end subroutine


subroutine test_user_config_dir()

character(:), allocatable :: d

d = user_config_dir()
if (len_trim(d) == 0) error stop "get_user_config_dir failed"

print '(a)', "OK: get_user_config_dir: " // d

end subroutine


end program
