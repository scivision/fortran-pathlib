program test_filesize

use, intrinsic :: iso_fortran_env, only: int64, stderr=>error_unit

use filesystem


implicit none

call test_file_size()
print '(a)', "OK: file_size"

call test_space_available()
print '(a)', "OK: space_available"

contains

subroutine test_space_available()

character, parameter :: s1 = "/"
integer(int64) :: i64

i64 = space_available(s1)

if (i64 == 0) then
  write(stderr, '(a,i0)') "space_available(" // s1 // ") failed: ", i64
  error stop
end if

print *, "space_available (GB): ", real(i64) / 1024**3

! if(space_available("not-exist-file") /= 0) error stop "space_available /= 0 for not existing file"
! if(space_available("") /= 0) error stop "space_available /= 0 for empty file"
! that's how windows/mingw defines it.

end subroutine


subroutine test_file_size()

integer :: u, d(10), ierr
integer(int64) :: i64

character(:), allocatable :: s1
allocate(character(len=max_path()) :: s1)

call get_command_argument(0, s1, status=ierr)
if (ierr /= 0) error stop "failed to get command line argument for test_file_size"

s1 = join(parent(s1), "test_filesize.dat")

print '(a)', "file_size path: ", trim(s1)

d = 0

open(newunit=u, file=s1, status="replace", action="write", access="stream")
! writing text made OS-specific newlines that could not be suppressed
write(u) d
close(u)

i64 = file_size(s1)
print '(a, i0)', "filesize (bytes): ", i64
if (i64 /= size(d)*storage_size(d)/8) error stop "file_size() mismatch"
if (i64 /= file_size(s1)) error stop "file_size() mismatch"

!> shaky platform

!> not exist no size
if (file_size("not-existing-file") > 0) error stop "size of non-existing file"

!! directory might have file size (Windows oneapi), but it is meaningless so don't test

if(file_size("") > 0) error stop "size of empty file"

end subroutine

end program
