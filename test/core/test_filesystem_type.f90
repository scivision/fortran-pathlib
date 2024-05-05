program test

use filesystem

implicit none

valgrind : block

character(:), allocatable :: fsname

fsname = filesystem_type(".")

print '(a)', fsname

if(is_windows()) then
  if (.not. any(fsname == [character(5) :: "NTFS", "EXFAT"])) error stop "expected NTFS or EXFAT"
elseif (is_wsl() > 0) then
  if (.not. any(fsname == ["ext4", "v9fs"])) error stop "expected ext4 or v9fs"
elseif (is_linux()) then
  if (.not. any(fsname == [character(5) :: "ext4", "tmpfs", "xfs"])) error stop "expected ext4 or tmpfs or xfs"
elseif (is_macos()) then
  if (.not. any(fsname == [character(4) :: "apfs", "hfs"])) error stop "expected apfs or hfs"
else
  error stop 77
end if

end block valgrind

print '(a)', "OK: filesystem_type"

end program
