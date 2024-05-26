program test

use, intrinsic :: iso_fortran_env, only: stderr => error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: exp(:)
character(:), allocatable :: fsname

if(is_cygwin()) then
  fsname = filesystem_type(getenv("SYSTEMDRIVE"))
else
  fsname = filesystem_type(".")
end if

print '(a)', fsname

if(is_windows() .or. is_cygwin()) then
  exp = [character(5) :: "NTFS", "EXFAT"]
elseif (is_wsl() > 0) then
  exp = [character(4) :: "ext4", "nfs", "v9fs"]
elseif (is_linux()) then
  exp = [character(5) :: "ext4", "nfs", "tmpfs", "xfs"]
elseif (is_macos()) then
  exp = [character(4) :: "apfs", "hfs"]
else
  error stop 77
end if

if (.not. any(fsname == exp)) then
  write(stderr, *) "FAIL: got " // fsname // " but expected: ", exp
  error stop
endif

end block valgrind

print '(a)', "OK: filesystem_type"

end program
