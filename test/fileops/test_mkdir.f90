program test

use filesystem

implicit none

call test_mkdir()
print '(a)', "OK: mkdir"

contains

subroutine test_mkdir()

character(:), allocatable :: pwd, p2, p1
logical :: ok

! call mkdir("")  !< error stops

pwd = get_cwd()
if(len_trim(pwd) == 0) error stop "get_cwd: " // pwd

print '(a)', "test_mkdir: pwd " // pwd

p1 = join(pwd, "test-filesystem-dir1")
print '(a)', "test_mkdir: testing " // p1
call mkdir(p1)
if(.not. is_dir(p1)) error stop "mkdir: full: " // p1

print '(a)', "test_mkdir: check existing dir doesn't fail"
call mkdir(p1, ok=ok)
if(.not. ok) error stop "mkdir: full: ok false despite success: " // p1
call remove(p1)

print '(a)', "test_mkdir: test relative path"
p1 = "test-filesystem-dir/hello"
call mkdir(p1, ok=ok)
if(.not. is_dir(p1)) error stop "ERROR:test_mkdir: relative: " // p1
if (.not. ok) error stop "ERROR:test_mkdir: relative: ok false despite success: " // p1
call remove(p1)

p2 = join(pwd, "test-filesystem-dir2/hello_posix")
print '(a)', "mkdir: testing " // p2
call mkdir(p2)
if(.not. is_dir(p2)) error stop "mkdir: full_posix: " // p2
call remove(p2)

end subroutine test_mkdir

end program
