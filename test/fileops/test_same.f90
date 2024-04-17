program test_canon

use, intrinsic :: iso_fortran_env, only : stderr => error_unit
use filesystem

implicit none

call test_same_file()
print *, "OK: same_file"

contains

subroutine test_same_file()

type(path_t) :: p1, p2
character(:), allocatable :: c1, s1, s2
integer :: i

allocate(character(max_path()) :: c1)

call get_command_argument(0, c1, status=i)
if (i /= 0) error stop 'ERROR: get_command_argument(0)'

print '(a)', "own command: " // trim(c1)

s1 = file_name(c1)
s2 = "./" // s1

if(.not. is_file(s1)) then
  write(stderr, '(a)') "ERROR: file name: " // s1 // " is not a file"
  error stop
endif

print '(a)', "Testing same_file(" // s1 // ", " // s2 // ")"

if(.not. same_file(s1, s2)) then
  write(stderr, '(a)') "ERROR: same_file(" // s1 // " /= " // s2 // ")"
  error stop
endif

s1 = parent(s1)
s2 = "./" // s1

print '(a)', "Testing same_file(" // s1 // ", " // s2 // ")"

if(.not. same_file(s1, s2)) then
  write(stderr, '(a)') "ERROR: same_file(" // s1 // " /= " // s2 // ")"
  error stop
endif

s1 = ".."
s2 = parent(get_cwd())
print '(a)', "Testing same_file(" // s1 // ", " // s2 // ")"
if (.not. same_file(s1, s2)) error stop "ERROR: same_file(" // s1 // " /= " // s2 // ")"

if (.not. same_file(".", "./")) error stop 'ERROR: same_file(., ./)'

!> hardlink resolves to the same file
s1 = "."
s2 = get_cwd()
print '(a)', "Testing same_file(" // s1 // ", " // s2 // ")"
if(.not. same_file(s1, s2)) error stop "ERROR: same_file(" // s1 // " /= " // s2 // ")"

call mkdir("test-a/b/")

p1 = path_t("test-a/c")
call p1%touch()
if(.not. is_file("test-a/c")) error stop "touch test-a/c failed"

p2 = path_t("test-a/b/../c")

if (.not. p1%same_file(p2)) error stop 'ERROR: %same_file'
if (.not. same_file(p1%path(), p2%path())) error stop 'ERROR: same_file()'

if(same_file("not-exist-same", "not-exist-same")) error stop 'ERROR: same_file(not-exist-same, not-exist-same)'


end subroutine test_same_file

end program
