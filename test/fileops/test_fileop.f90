program test_fileop

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


valgrind : block

logical :: ok

character(:), allocatable :: old_cwd, cwd

integer :: i
character(:), allocatable :: s1, s2

allocate(character(max_path()) :: s1)

call get_command_argument(0, s1, status=i)
if(i/=0) error stop "failed to get command argument"

s2 = parent(parent(s1))

old_cwd = get_cwd()

print '(a)', 'current working directory: ', old_cwd

ok = set_cwd(s2)

if (.not. ok) error stop "chdir failed"

if(set_cwd("")) error stop "chdir should fail on empty string"

cwd = get_cwd()
print '(a)', 'New working directory: ', cwd

ok = set_cwd(old_cwd)
!! avoid messing up subsequent test location

if (.not. same_file(cwd, s2)) error stop "chdir failed: " // s2 // " /= " // cwd

end block valgrind

print '(a)', "OK: chdir"

end program
