program test_filesize

use filesystem

implicit none

call test_file_size()
print '(a)', "OK: file_size"

call test_space_available()
print '(a)', "OK: space_available"

contains

subroutine test_space_available()

integer :: ierr
character(:), allocatable :: buf
allocate(character(len=max_path()) :: buf)


if(command_argument_count() > 0) then
  call get_command_argument(1, buf, status=ierr)
  if (ierr /= 0) error stop "failed to get command line argument for test_space_available"
else
  buf = "."
end if

print '(a,f7.3)', "space_available (GB): ", real(space_available(buf)) / 1024**3

! if(space_available("not-exist-file") /= 0) error stop "space_available /= 0 for not existing file"
! if(space_available("") /= 0) error stop "space_available /= 0 for empty file"
! that's how windows/mingw defines it.

end subroutine


subroutine test_file_size()

integer :: u, d(10), ierr

type(path_t) :: p1

character(:), allocatable :: fn
allocate(character(len=max_path()) :: fn)


if(command_argument_count() > 0) then
  call get_command_argument(1, fn, status=ierr)
  if(ierr /= 0) error stop "failed to get path from command line argument"
else
  fn = "test_filesize.dat"
endif

print '(a)', "file_size path: ", trim(fn)

d = 0

p1 = path_t(fn)

open(newunit=u, file=fn, status="replace", action="write", access="stream")
! writing text made OS-specific newlines that could not be suppressed
write(u) d
close(u)

if (p1%file_size() /= size(d)*storage_size(d)/8) error stop "size mismatch OO"
if (p1%file_size() /= file_size(p1%path())) error stop "size mismatch functional"
print '(a, i0)', "PASSED: filesize as expected: ", p1%file_size()

!> shaky platform

!> not exist no size
if (file_size("not-existing-file") > 0) error stop "size of non-existing file"

!! directory might have file size (Windows oneapi), but it is meaningless so don't test

if(file_size("") > 0) error stop "size of empty file"

end subroutine

end program
