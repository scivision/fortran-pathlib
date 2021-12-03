program demo

use, intrinsic :: iso_c_binding, only : c_null_char
use pathlib, only : path

implicit none (type, external)

type(path) :: cwd, parent, file, p1, p2
character(*), parameter :: dummy = "nobody.txt"

integer :: L1, L2, L3

! -- current directory
cwd%path = "."
cwd = cwd%resolve()
L1 = cwd%length()
if (L1 < 3) error stop "ERROR canonical '.' " // cwd%path

print *, "OK: current dir = ", cwd%path
! -- home directory
p1%path = "~"
p1 = p1%resolve()
if (p1%path(1:1) == "~") error stop "resolve ~" // p1%path
print *, "OK: home dir = ", p1%path

p2%path = p1%parent()
if (p2%length() >= p1%length()) error stop "parent home " // p2%path

! -- relative dir
parent%path = ".."
parent = parent%resolve()

L2 = parent%length()
if (L2 >= L1+2) error stop 'ERROR: directory was not canonicalized: ' // parent%path

print *, 'OK: parent = ', parent%path
! -- relative file
file%path = '../' // dummy
file = file%resolve()
L3 = file%length()
if (L3 - L2 /= len(dummy) + 1) error stop 'ERROR: file was not canonicalized: ' // file%path

print *, 'OK: canon_file = ', file%path


print *, "OK: canonical"
end program