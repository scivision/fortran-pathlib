submodule (filesystem) fort2c_ifc

use, intrinsic:: iso_fortran_env, only: stderr=>error_unit
use, intrinsic :: iso_c_binding, only : C_NULL_CHAR, C_SIZE_T

implicit none

interface

integer(C_INT) function max_path() bind(C, name="fs_get_max_path")
import
end function

subroutine fs_as_posix(path) bind(C)
import
character(kind=C_CHAR), intent(inout) :: path(*)
end subroutine

integer(C_SIZE_T) function fs_canonical(path, strict, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
logical(C_BOOL), intent(in), value :: strict
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_resolve(path, strict, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
logical(C_BOOL), intent(in), value :: strict
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_set_permissions(path, readable, writable, executable) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
integer(C_INT), intent(in), value :: readable, writable, executable
end function

integer(C_SIZE_T) function fs_get_permissions(path, perms, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: perms(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_copy_file(source, dest, overwrite) bind(C)
import
character(kind=c_char), intent(in) :: source(*), dest(*)
logical(C_BOOL), intent(in), value :: overwrite
end function


logical(C_BOOL) function fs_equivalent(path1, path2) bind(C)
import C_BOOL, C_CHAR
character(kind=C_CHAR), intent(in) :: path1(*), path2(*)
end function

logical(C_BOOL) function fs_is_symlink(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_compiler(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_read_symlink(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_create_symlink(target, link) bind(C)
import
character(kind=C_CHAR), intent(in) :: target(*), link(*)
end function

logical(C_BOOL) function fs_mkdir(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_remove(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_exists(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_expanduser(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_file_name(path, filename, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: filename(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_is_safe_name(filename) bind(C)
import
character(kind=C_CHAR), intent(in) :: filename(*)
end function

integer(C_SIZE_T) function fs_file_size(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_space_available(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_get_cwd(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_set_cwd(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_get_homedir(path, buffer_size) bind(C)
import
character(kind=c_char), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_tempdir(path, buffer_size) bind(C)
import
character(kind=c_char), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(c_bool) function fs_is_absolute(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_char_device(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_file(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_dir(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_exe(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_readable(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_writable(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_reserved(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_join(path, other, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*), other(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_is_subdir(subdir, dir) bind(C)
import
character(kind=C_CHAR), intent(in) :: subdir(*), dir(*)
end function

integer(C_SIZE_T) function fs_make_absolute(path, base, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*), base(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_normal(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_parent(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_relative_to(path, base, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*), base(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_which(name, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: name(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_root(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_stem(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_suffix(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(c_bool) function fs_touch(path) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_with_suffix(path, new_suffix, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*), new_suffix
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_make_tempdir(result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_shortname(in, out, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: in(*)
character(kind=C_CHAR), intent(out) :: out(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_longname(in, out, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: in(*)
character(kind=C_CHAR), intent(out) :: out(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_lib_path(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_exe_path(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_setenv(name, val) bind(C)
import
character(kind=C_CHAR), intent(in) :: name(*), val(*)
end function

end interface

contains

module procedure get_max_path
get_max_path = int(max_path())
end procedure

module procedure as_posix
character(kind=c_char, len=:), allocatable :: cbuf
integer :: N
N = len_trim(path)
allocate(character(N+1) :: cbuf)
cbuf = trim(path) // C_NULL_CHAR
call fs_as_posix(cbuf)
allocate(character(N) :: r)
r = cbuf(:N)
end procedure

module procedure compiler_c
include "ifc0a.inc"
associate (fun => fs_compiler(cbuf, N))
include "ifc0b.inc"
end procedure

module procedure canonical
include "ifc1a.inc"
associate (fun => fs_canonical(trim(path) // C_NULL_CHAR, s, cbuf, N))
include "ifc0b.inc"
end procedure canonical

module procedure resolve
include "ifc1a.inc"
associate (fun => fs_resolve(trim(path) // C_NULL_CHAR, s, cbuf, N))
include "ifc0b.inc"
end procedure resolve

module procedure set_permissions
logical(C_BOOL) :: s

integer(C_INT) :: r, w, e

r = 0
w = 0
e = 0

if(present(readable)) then
  r = -1
  if(readable) r = 1
endif
if(present(writable)) then
  w = -1
  if(writable) w = 1
endif
if(present(executable)) then
  e = -1
  if(executable) e = 1
endif

s = fs_set_permissions(trim(path) // C_NULL_CHAR, r, w, e)
if(present(ok)) then
  ok = s
elseif (.not. s) then
  write(stderr, '(/,A,L1,1x,L1,1x,L1,1x,A)') "ERROR: set_permissions: failed to set permission ", &
    readable,writable,executable, trim(path)
  error stop
endif
end procedure

module procedure get_permissions
character(kind=c_char, len=:), allocatable :: cbuf
integer(C_SIZE_T) :: N
allocate(character(10) :: cbuf)
N = fs_get_permissions(trim(path) // C_NULL_CHAR, cbuf, len(cbuf, kind=C_SIZE_T))
if(N > 9) error stop "filesystem:get_permissions: unexpected length /= 9"
get_permissions = cbuf(:N)
end procedure


module procedure copy_file
logical(c_bool) :: ow, s
ow = .false.
if(present(overwrite)) ow = overwrite
s = fs_copy_file(trim(src) // C_NULL_CHAR, trim(dest) // C_NULL_CHAR, ow)
if (present(ok)) then
  ok = s
elseif(.not. s) then
  error stop "ERROR:ffilesystem:copy_file: failed to copy file: " // trim(src) // " to " // trim(dest)
endif
end procedure


module procedure mkdir
logical(C_BOOL) :: s

s = fs_mkdir(trim(path) // C_NULL_CHAR)
if(present(ok)) then
  ok = s
elseif (.not. s) then
  write(stderr,'(a,i0)') "ERROR:filesystem:mkdir: failed to create directory: " // trim(path)
  error stop
endif
end procedure mkdir


module procedure read_symlink
include "ifc0a.inc"
associate (fun => fs_read_symlink(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure


module procedure create_symlink

logical(C_BOOL) :: s

s = fs_create_symlink(trim(tgt) // C_NULL_CHAR, trim(link) // C_NULL_CHAR)
if(present(ok)) then
  ok =s
elseif (.not. s) then
  write(stderr,'(a,1x,i0)') "ERROR:Ffilesystem:create_symlink: " // trim(link)
  error stop
endif
end procedure

module procedure exists
exists = fs_exists(trim(path) // C_NULL_CHAR)
end procedure

module procedure expanduser
include "ifc0a.inc"
associate (fun => fs_expanduser(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure file_name
include "ifc0a.inc"
associate (fun => fs_file_name(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure is_safe_name
is_safe_name = fs_is_safe_name(trim(filename) // C_NULL_CHAR)
end procedure

module procedure file_size
file_size = fs_file_size(trim(path) // C_NULL_CHAR)
end procedure

module procedure space_available
space_available = fs_space_available(trim(path) // C_NULL_CHAR)
end procedure

module procedure get_cwd
include "ifc0a.inc"
associate (fun => fs_get_cwd(cbuf, N))
include "ifc0b.inc"
end procedure

module procedure set_cwd
set_cwd = fs_set_cwd(trim(path) // C_NULL_CHAR)
end procedure

module procedure get_homedir
include "ifc0a.inc"
associate (fun => fs_get_homedir(cbuf, N))
include "ifc0b.inc"
end procedure

module procedure get_tempdir
include "ifc0a.inc"
associate (fun => fs_get_tempdir(cbuf, N))
include "ifc0b.inc"
end procedure

module procedure is_absolute
!! no expanduser to be consistent with Python filesystem etc.
is_absolute = fs_is_absolute(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_char_device
is_char_device = fs_is_char_device(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_dir
is_dir = fs_is_dir(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_exe
is_exe = fs_is_exe(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_readable
is_readable = fs_is_readable(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_writable
is_writable = fs_is_writable(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_file
is_file = fs_is_file(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_reserved
is_reserved = fs_is_reserved(trim(path) // C_NULL_CHAR)
end procedure

module procedure is_symlink
is_symlink = fs_is_symlink(trim(path) // C_NULL_CHAR)
end procedure

module procedure join
include "ifc0a.inc"
associate (fun => fs_join(trim(path) // C_NULL_CHAR, trim(other) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure is_subdir
is_subdir = fs_is_subdir(trim(subdir) // C_NULL_CHAR, trim(dir) // C_NULL_CHAR)
end procedure

module procedure make_absolute
include "ifc0a.inc"
associate (fun => fs_make_absolute(trim(path) // C_NULL_CHAR, trim(base) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure parent
include "ifc0a.inc"
associate (fun => fs_parent(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure normal
include "ifc0a.inc"
associate (fun => fs_normal(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure relative_to
include "ifc0a.inc"
associate (fun => fs_relative_to(trim(a) // C_NULL_CHAR, trim(b) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure which
include "ifc0a.inc"
associate (fun => fs_which(trim(name) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure remove
logical(c_bool) :: e
e = fs_remove(trim(path) // C_NULL_CHAR)
if (.not. e) write(stderr, '(a)') "ERROR:ffilesystem:remove: " // trim(path) // " may not have been deleted."
end procedure

module procedure root
include "ifc0a.inc"
associate (fun => fs_root(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure same_file
same_file = fs_equivalent(trim(path1) // C_NULL_CHAR, trim(path2) // C_NULL_CHAR)
end procedure

module procedure stem
include "ifc0a.inc"
associate (fun => fs_stem(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure suffix
include "ifc0a.inc"
associate (fun => fs_suffix(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure touch
if(.not. fs_touch(trim(path) // C_NULL_CHAR)) then
  error stop "filesystem:touch: " // trim(path)
end if
end procedure

module procedure with_suffix
include "ifc0a.inc"
associate (fun => fs_with_suffix(trim(path) // C_NULL_CHAR, trim(new) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure with_suffix

module procedure make_tempdir
include "ifc0a.inc"
associate (fun => fs_make_tempdir(cbuf, N))
include "ifc0b.inc"
end procedure


module procedure shortname
include "ifc0a.inc"
associate (fun => fs_shortname(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure

module procedure longname
include "ifc0a.inc"
associate (fun => fs_longname(trim(path) // C_NULL_CHAR, cbuf, N))
include "ifc0b.inc"
end procedure


module procedure setenv
logical(C_BOOL) :: s

s = fs_setenv(trim(name) // C_NULL_CHAR, trim(val) // C_NULL_CHAR)
if(present(ok)) then
  ok = s
elseif (.not. s) then
  write(stderr,'(a,1x,i0)') "ERROR:Ffilesystem:setenv: " // trim(name)
  error stop
endif
end procedure

module procedure exe_path
include "ifc0a.inc"
associate (fun => fs_exe_path(cbuf, N))
include "ifc0b.inc"
end procedure

module procedure lib_path
include "ifc0a.inc"
associate (fun => fs_lib_path(cbuf, N))
include "ifc0b.inc"
end procedure


end submodule fort2c_ifc
