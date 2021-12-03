submodule (pathlib) path_canon
!! For Windows systems.
!! path need not exist.
!!
!! https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fullpath-wfullpath?view=vs-2019


use, intrinsic :: iso_c_binding, only: c_long, c_char, c_null_char

implicit none (type, external)


interface !< Windows C runtime library
subroutine fullpath_c(absPath, relPath, maxLength) bind(c, name='_fullpath')

!! char *_fullpath(char *absPath, const char *relPath, size_t maxLength)
!! https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fullpath-wfullpath?view=vs-2019

import c_char, c_long
character(kind=c_char), intent(in) :: relPath(*)
character(kind=c_char), intent(out) :: absPath(*)
integer(c_long), intent(in) :: maxLength
end subroutine fullpath_c
end interface

contains


module procedure canonical

type(path_t) :: p
integer(c_long), parameter :: N = 4096
character(kind=c_char):: c_buf(N)
character(N) :: buf
integer :: i

if(len_trim(path) == 0) error stop "cannot canonicalize empty path"

p = path_t(path)
p = p%expanduser()

!! some systems e.g. old MacOS can't handle leading "." or ".."
!! so manually resolve this part with CWD, which is implicit.

if (p%path_str(1:1) == ".") p%path_str = cwd() // "/" // p%path_str

if(len(p%path_str) > N) error stop "path too long"

call fullpath_c(c_buf, p%path_str // c_null_char, N)

do i = 1,N
  if (c_buf(i) == c_null_char) exit
  buf(i:i) = c_buf(i)
enddo

canonical = trim(buf(:i-1))

end procedure canonical


end submodule path_canon
