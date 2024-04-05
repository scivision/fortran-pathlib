program test_binpath

use filesystem

implicit none

valgrind : block

character(:), allocatable :: path
integer :: i, L
character :: s
logical :: shared

shared = .false.
if(command_argument_count() > 0) then
  call get_command_argument(1, s, length=L, status=i)
  if(i/=0) error stop "ERROR:test_binpath:test_lib_path: get_command_argument failed"
  if(L/=1) error stop "ERROR:test_binpath: expected argument 0 for static or 1 for shared"
  shared = s == '1'
endif

path = lib_path()

if(.not. exists(path)) error stop path // " does not exist"

if(is_dir(path)) then
  if(shared) error stop "test_libpath: for shared libraries a full path to the library file is expected: " // path
  print '(a)', path
  stop
endif

if (.not. is_file(path)) error stop path // " is neither a file nor a directory"

print '(a)', path

end block valgrind

end program
