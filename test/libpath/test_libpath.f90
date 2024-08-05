program test_binpath

use filesystem

implicit none

valgrind : block

character(:), allocatable :: path
logical :: shared

shared = getarg(1) == '1'

path = lib_path()

if(.not. exists(path)) error stop path // " does not exist"

if(is_dir(path)) then
  if(shared) error stop "test_libpath: for shared libraries a full path to the library file is expected: " // path
  print '(a)', path
  stop
end if

if (.not. is_file(path)) error stop path // " is neither a file nor a directory"

print '(a)', path

end block valgrind

end program
