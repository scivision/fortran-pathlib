program test_exe

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none


call test_not_exist()
print '(a)', "PASSED: test_not_exist"

call test_exist()
print '(a)', "PASSED: test_exist"

call test_set_permissions()
print '(a)', "PASSED: test_set_permissions"

contains

subroutine test_not_exist()

type(path_t) :: p1

!> empty file
if(is_exe("")) error stop "ERROR:test_exe: is_exe('') should be false"
if(len_trim(get_permissions("")) /= 0) error stop "ERROR:test_exe: get_permissions('') should be empty"

!> not exist file
p1 = path_t("not-exist-file")
if (p1%is_file()) error stop "ERROR:test_exe: not-exist-file should not exist."
if (p1%is_exe()) error stop "ERROR:test_exe: non-exist-file cannot be executable"
if(len_trim(get_permissions("not-exist-file")) /= 0) error stop "ERROR:test_exe: get_permissions('not-exist') should be empty"

end subroutine test_not_exist


subroutine test_exist()

character(:), allocatable :: exe, noexe

exe = "test_exe"
noexe = "test_noexe"

call touch(exe)
call touch(noexe)

call set_permissions(exe, executable=.true.)
call set_permissions(noexe, executable=.false.)

if(is_exe(parent(exe))) then
  write(stderr, '(a)') "ERROR:test_exe: directory" // parent(exe) // " should not be executable"
  error stop
endif

print '(a)', "permissions: " // trim(exe) // " = " // get_permissions(exe)

if (.not. is_exe(exe)) then
  write(stderr,'(a)') "ERROR:test_exe: " // trim(exe) // " is not executable."
  error stop
endif

print '(a)', "permissions: " // trim(noexe) // " = " // get_permissions(noexe)

if (is_exe(noexe)) then
  if(is_windows()) then
    write(stderr,'(a)') "XFAIL:test_exe: " // trim(noexe) // " is executable on Windows."
  else
    write(stderr,'(a)') "ERROR:test_exe: " // trim(noexe) // " is executable and should not be."
    error stop
  endif
endif

call remove(exe)
call remove(noexe)

end subroutine test_exist


subroutine test_set_permissions()

type(path_t) :: p1, p2

character(9) :: perm
logical :: ok

character(:), allocatable :: exe, noexe

exe = join(get_tempdir(), "yes_exe")
noexe = join(get_tempdir(), "no_exe")

!> chmod(.true.)

p1 = path_t(exe)
p1 = p1%canonical()
call p1%touch()
if(.not. p1%is_file()) error stop "ERROR:test_exe: " // p1%path() // " is not a file."

perm = get_permissions(exe)
print '(a)', "permissions before set_permissions(exe=true) " // p1%path() // " = " // perm

call set_permissions(p1%path(), executable=.true.)

call set_permissions(p1%path(), executable=.false., ok=ok)
if (.not. ok) error stop "ERROR:test_exe: set_permissions(exe=.false.) failed"

call p1%set_permissions(executable=.true., ok=ok)

perm = get_permissions(exe)
print '(a)', "permissions after set_permissions(exe=true): " // p1%path() // " = " // perm

if (.not. p1%is_exe()) then
  write(stderr,'(a)') "ERROR:test_exe: %is_exe() did not detect executable file " // trim(exe)
  if(.not. is_windows()) error stop
endif

if (.not. is_exe(p1%path())) then
  write(stderr, '(a)') "ERROR:test_exe: is_exe(path) did not detect executable file " // trim(exe)
  if(.not. is_windows()) error stop
endif

if(.not. is_windows()) then
if(perm(3:3) /= "x") then
  write(stderr,'(a)') "ERROR:test_exe: get_permissions() " // trim(exe) // " is not executable"
  error stop
endif
endif

!> chmod(.false.)

p2 = path_t(noexe)
call p2%touch()
if(.not. p2%is_file()) then
  write(stderr,'(a)') "ERROR:test_exe: " // trim(noexe) // " is not a file."
  error stop
endif

perm = get_permissions(noexe)
print '(a)', "permissions: " // trim(noexe) // " = " // perm

call p2%set_permissions(executable=.false., ok=ok)
if (.not. ok) error stop "ERROR:test_exe: set_permissions(exe=.false.) failed"

if(.not. is_windows()) then
!~ Windows file system is always executable to stdlib.

  if (p2%is_exe()) error stop "ERROR:test_exe: did not detect non-executable file."

  if(perm(3:3) /= "-") then
    write(stderr,'(a)') "ERROR:test_exe: get_permissions() " // trim(noexe) // " is executable"
    error stop
  endif

endif

end subroutine test_set_permissions

end program
