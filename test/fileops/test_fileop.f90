program test_fileop

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none


valgrind : block

logical :: ok

character(:), allocatable :: old_cwd, cwd, req

integer :: i
character(:), allocatable :: buf

allocate(character(max_path()) :: buf)

if(command_argument_count() < 1) error stop "please specify path to chdir"
call get_command_argument(1, buf, status=i)
if(i/=0) error stop "failed to get command argument"


old_cwd = get_cwd()

print '(a)', 'current working directory: ', old_cwd

ok = set_cwd(buf)

if (.not. ok) error stop "chdir failed"

if(set_cwd("")) error stop "chdir should fail on empty string"

cwd = get_cwd()
print '(a)', 'New working directory: ', cwd

req = canonical(buf)

ok = set_cwd(old_cwd)
!! avoid messing up subsequent test location

if (cwd /= req) error stop "chdir failed: " // req // " != " // cwd

end block valgrind

print '(a)', "OK: chdir"

end program
