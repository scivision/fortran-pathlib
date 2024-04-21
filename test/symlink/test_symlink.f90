program test_symlink

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

integer :: i, L
character :: s
logical :: win32_symlink

valgrind: block

logical :: ok

character(:), allocatable :: tgt, rtgt, link, linko, tgt_dir, link_dir

allocate(character(max_path()) :: tgt_dir)

call get_command_argument(0, tgt_dir, status=i, length=L)
if(i /= 0 .or. L == 0) error stop "could not get command line arg 0"

win32_symlink = .false.
if(command_argument_count() > 0) then
  call get_command_argument(1, s, status=i)
  if(i /= 0) error stop "could not get command line arg 1"
  if(s /= '0') win32_symlink = .true.
endif

tgt_dir = parent(tgt_dir)

tgt = join(tgt_dir, "test.txt")
call touch(tgt)

link = join(tgt_dir, "test.link")
linko = join(tgt_dir, "test_oo.link")
link_dir = join(tgt_dir, "my_link.dir")

! print *, "TRACE:test_symlink: target: " // tgt
! print *, "TRACE:test_symlink: link: " // link

call create_symlink(tgt, "", ok)
if (ok) error stop "ERROR: create_symlink() should fail with empty link"
print '(a)', "PASSED: create_symlink: empty link"
if(is_symlink(tgt)) then
  write(stderr, '(a)') "is_symlink() should be false for non-symlink file: " // tgt
  error stop
endif

call create_symlink("", link, ok)
if (ok) error stop "ERROR: create_symlink() should fail with empty target"
print '(a)', "PASSED: create_symlink: empty target"

if (is_symlink(link)) then
  print *, "deleting old symlink " // link
  call remove(link)
endif
call create_symlink(tgt, link)
print '(a)', "PASSED: create_symlink " // link

if(win32_symlink) then
  write(stderr,"(a)") "Skipping read_symlink() test on Windows"
else

  !> read_symlink
  rtgt = read_symlink(link)
  if(rtgt /= tgt) then
    write(stderr, '(a)') "read_symlink() failed: " // rtgt // " /= " // tgt
    error stop
  endif
  print '(a)', "PASSED: read_symlink " // rtgt // " == " // tgt

  !> read_symlink non-symlink
  rtgt = read_symlink(tgt)
  if (len_trim(rtgt) > 0) then
    write(stderr, '(a)') "read_symlink() should return empty string for non-symlink file: " // rtgt
    error stop
  endif

  !> read_symlink non-existent
  rtgt = read_symlink("not-exist-file")
  if (len_trim(rtgt) > 0) then
    write(stderr, '(a)') "read_symlink() should return empty string for non-existent file: " // rtgt
    error stop
  endif

endif


if (is_symlink(linko)) then
  print *, "deleting old symlink " // linko
  call remove(linko)
endif
call create_symlink(tgt, linko)
print '(a)', "PASSED: created symlink " // linko

!> directory symlinks
if (is_symlink(link_dir)) then
  print *, "deleting old symlink " // link_dir
  call remove(link_dir)
endif
call create_symlink(tgt_dir, link_dir)

!> checks
! call create_symlink("", "")  !< this error stops

!> file symlinks
if(is_symlink(tgt)) error stop "is_symlink() should be false for non-symlink file"
if(.not. is_file(link)) then
  write(stderr, "(a)") "is_file() should be true for existing regular file: " // link
  error stop
endif

if(.not. is_symlink(link)) then
  write(stderr, '(a)') "is_symlink() should be true for symlink file: " // link
  error stop
endif
if(.not. is_file(link)) then
  write(stderr, '(a)') "is_file() should be true for existing symlink file: " // link
  error stop
endif

print '(a)', "PASSED: test_symlink: file"

!> directory symlinks
if(is_symlink(tgt_dir)) error stop "is_symlink() should be false for non-symlink dir"
if(.not. is_dir(link_dir)) then
  write(stderr, '(a)') "is_dir() should be true for existing regular dir" // link_dir
  error stop
endif

if(.not. is_symlink(link_dir)) then
  write(stderr, '(a)') "is_symlink() should be true for symlink dir: " // link_dir
  error stop
endif
if(.not. is_dir(link_dir)) then
  write(stderr,'(a)') "is_dir() should be true for existing symlink dir: " // link_dir
  error stop
endif

end block valgrind

print *, "OK: filesystem symbolic links"

end program
