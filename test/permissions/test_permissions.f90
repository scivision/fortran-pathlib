program main

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

character(9) :: p


character(*), parameter :: reada="readable.txt", noread="not-readable.txt", nowrite="not-writable.txt"

p = get_permissions("")
if(len_trim(p) /= 0) then
    write(stderr, '(a)') "get_permissions('') should be empty: " // p
    error stop
endif

!> readable
call touch(reada)
call set_permissions(reada, readable=.true.)

p = get_permissions(reada)
print '(a)', "Permissions for " // trim(reada)// ": "// p

if (len_trim(p) == 0) error stop "get_permissions('"//trim(reada)//"') should not be empty"

if (p(1:1) /= "r") then
    write(stderr, '(a)') "ERROR: test_exe: "//trim(reada)//" should be readable"
    error stop
endif
if(.not. is_readable(reada)) error stop "test_exe: "//trim(reada)//" should be readable"

if(.not. exists(reada)) error stop "test_exe: "//trim(reada)//" should exist"

if(.not. is_file(reada)) error stop "test_exe: "//trim(reada)//" should be a file"

!! for Ffilesystem, even non-readable files "exist" and are "is_file"

call touch(noread)
call set_permissions(noread, readable=.false.)

p = get_permissions(noread);
print '(a)', "Permissions for " // trim(noread)// ": "// p

if (len_trim(p) == 0) error stop "get_permissions('"//trim(noread)//"') should not be empty"

if (index(p, "r") /= 0) then
    write(stderr, '(a)') "XFAIL:test_exe: "//trim(noread)//" should not be readable"
else
if (is_readable(noread)) error stop "test_exe: "//trim(noread)//" should not be readable"

if (.not. exists(noread)) error stop "test_exe: "//trim(noread)//" should exist"

if (.not. is_file(noread)) error stop "test_exe: "//trim(noread)//" should be a file"
endif

!> writable
if(.not. is_file(nowrite)) call touch(nowrite)
call set_permissions(nowrite, writable=.false.)

if (.not. is_writable(".")) error stop "test_exe: . should be writable"

p = get_permissions(nowrite);
print '(a)', "Permissions for " // trim(nowrite)// ": "// p

if (len_trim(p) == 0) error stop "get_permissions('"//trim(nowrite)//"') should not be empty"

if (index(p, "w") /= 0) then
    write(stderr, '(a)') "test_exe: "//trim(nowrite)//" should not be writable"
    if(.not. is_windows()) error stop
endif
if(is_writable(nowrite)) error stop "test_exe: "//trim(nowrite)//" should not be writable"

if (.not. exists(nowrite)) error stop "test_exe: "//trim(nowrite)//" should exist"

if (.not. is_file(nowrite)) error stop "test_exe: "//trim(nowrite)//" should be a file"

call remove(reada)
call remove(noread)
call remove(nowrite)

end program
