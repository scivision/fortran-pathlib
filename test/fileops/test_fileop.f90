program test_fileop

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

integer :: i
character(:), allocatable :: buf

allocate(character(max_path()) :: buf)

if(command_argument_count() < 1) error stop "please specify path to chdir"
call get_command_argument(1, buf, status=i)
if(i/=0) error stop "failed to get command argument"

call test_chdir(buf)
print '(a)', "OK: chdir set_cwd"

call test_touch()
print '(a)', "OK: touch"

deallocate(buf) !< valgrind

contains


subroutine test_chdir(path)
character(*), intent(in) :: path

logical :: ok

character(:), allocatable :: old_cwd, cwd, req

old_cwd = get_cwd()

print '(a)', 'current working directory: ', old_cwd

ok = set_cwd(path)

if (.not. ok) error stop "chdir failed"

cwd = get_cwd()
print '(a)', 'New working directory: ', cwd

req = canonical(path)

ok = set_cwd(old_cwd)
!! avoid messing up subsequent test location

if (cwd /= req) error stop "chdir failed: " // req // " != " // cwd

end subroutine


subroutine test_touch

type(path_t) :: p

! call touch("")  !< error stops

call touch("test_fileop.h5")

p = path_t("test_fileop.empty")
call p%touch()
if(.not. p%is_file()) error stop "touch failed"
call assert_is_file(p%path())

end subroutine test_touch


end program
