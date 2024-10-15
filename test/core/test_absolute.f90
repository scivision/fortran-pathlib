program main

use filesystem
use, intrinsic :: iso_fortran_env, only: stderr=> error_unit

implicit none

valgrind : block

character(:), allocatable :: in, base, ref, out

in = "rel"

if (is_windows()) then
  base = "j:/foo"
  ref = "j:/foo/rel"
else
  base = "/foo"
  ref = "/foo/rel"
end if

out = absolute(in, base)
if(len_trim(out) == 0) error stop "absolute() has empty output"
if (out /= ref) then
  write(stderr, '(a)') "Mismatch: absolute(" // in //", " // base // ") " // out // " /= " // ref
  error stop
endif

ref = get_cwd() // "/rel"
out = absolute(in, "")
if(len_trim(out) == 0) error stop "absolute(" // in // ", '') has empty output"
if(out /= ref)  then
  write(stderr, '(a)') "absolute(" // in // ", '') =" // out // " /= " // ref
  error stop
endif

ref = get_cwd()
out = absolute("", "")
if(len_trim(out) == 0) error stop "absolute('', '') has empty output"
if(out /= ref) then
  write(stderr, '(a)') "absolute('', ''): " // out // " /= " // ref
  error stop
endif

ref = get_cwd() // "/rel"
out = absolute("", in)
if(len_trim(out) == 0) error stop "absolute('', path) has empty output"
if(out /= ref) then
  write(stderr,'(a)') "absolute('', " // in //"): " // out // " /= " // ref
  error stop
endif

end block valgrind

end program
