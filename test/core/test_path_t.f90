program path_methods

use, intrinsic :: iso_fortran_env, only : stderr => ERROR_UNIT, int64
use filesystem, only : path_t, get_cwd, is_windows, max_path, get_homedir

implicit none

call test_setter_getter()
print '(a)', "OK: setter/getter"

valgrind : block

type(path_t) :: p1, p2
integer :: L1, i
integer(int64) :: i64
character(:), allocatable :: s1, s2
character(9) :: c9
logical :: b

!> as_posix
if(is_windows()) then
  p1 = path_t("C:\a")
  p1 = p1%as_posix()
  if(p1%path() /= "C:/a") error stop "as_posix"
else
  p1 = path_t("/a")
  p1 = p1%as_posix()
  if(p1%path() /= "/a") error stop "as_posix"
endif

!> absolute
if(is_windows()) then
  p1 = path_t("C:/a")
else
  p1 = path_t("/a")
endif
p2 = path_t("a/b")
if(.not. p1%is_absolute()) error stop "is_absolute(abs)"
if(p2%is_absolute()) error stop "is_absolute(rel)"

!> expanduser
p1 = path_t("~")
p1 = p1%expanduser()
if (p1%path() /= get_homedir()) error stop "expanduser"

!> canonical
p1 = path_t(".")
p1 = p1%canonical()
L1 = p1%length()
if (L1 == 0) then
  write(stderr,*) "ERROR: canonical(.) " // p1%path()
  error stop
endif

if (p1%path() /= get_cwd()) then
  write(stderr,*) "ERROR: canonical(.) " // p1%path() // " /= get_cwd: " // get_cwd()
  error stop
endif

!> resolve
p1 = path_t(".")
p1 = p1%resolve()
L1 = p1%length()
if (L1 == 0) then
  write(stderr,*) "ERROR: resolve(.) " // p1%path()
  error stop
endif

if (p1%path() /= get_cwd()) then
  write(stderr,*) "ERROR: resolve(.) " // p1%path() // " /= get_cwd: " // get_cwd()
  error stop
endif

!> filename
p1 = path_t("a/b/c")
if (p1%file_name() /= "c") error stop "filename"

!> is_dir, is_file, exists
allocate(character(max_path()) :: s2)
call get_command_argument(0, s2, status=i)
if (i /= 0) error stop "ERROR: get_command_argument"

p1 = path_t(s2)
if (.not. p1%exists()) error stop "exists(file)"
if (p1%is_dir()) error stop "is_dir(file)"
if (.not. p1%is_file()) error stop "is_file(file)"
p1 = path_t(p1%parent())
if (.not. p1%is_dir()) error stop "is_dir(dir)"

!> is_exe
p1 = path_t(s2)
if (.not. p1%is_exe()) error stop "is_exe(exe)"
p1 = path_t(p1%parent())
if (p1%is_exe()) error stop "is_exe(dir)"

!> file_size
p1 = path_t(s2)
if (p1%file_size() == 0) error stop "file_size"

!> space_available
p1 = path_t(s2)
if (p1%space_available() == 0) error stop "space_available"

!> modtime
i64 = p1%modtime()
if (i64 == 0) error stop "modtime"

!> get_permissions
c9 = p1%get_permissions()
print '(a)', "get_permissions: " // c9
if(c9(1:3) /= "rwx") error stop "get_permissions expected user rwx"

!> is_subdir
p1 = path_t("a/b")
if (.not.p1%is_subdir("a")) error stop "is_subdir(a)"
if (p1%is_subdir("a/b/c")) error stop "is_subdir(a/b/c)"

!> join
p1 = path_t("a")
p1 = p1%join("b")
if (p1%path() /= "a/b") error stop "join"

!> normal
p1 = path_t("a/b/../c")
p1 = p1%normal()
if (p1%path() /= "a/c") error stop "normal"

!> parent
p1 = path_t("a/b/c")
if (p1%parent() /= "a/b") error stop "parent"

!> proximate
if(is_windows()) then
  p1 = path_t("X:/a/b")
else
  p1 = path_t("/a/b")
endif
s1 = p1%proximate_to("c")
if (s1 /= "c") error stop "proximate_to: " // s1

!> relative
s1 = p1%relative_to("c")
if (s1 /= "") error stop "relative_to: " //s1

!> root
p1 = path_t("/c/b")
if (p1%root() /= "/") error stop "root: " // p1%root()

!> stem
p1 = path_t("a/b.txt")
if (p1%stem() /= "b") error stop "stem: " // p1%stem()

!> suffix
s1 = p1%suffix()
if (s1 /= ".txt") then
  write(stderr,'(a)') "ERROR: suffix: " // s1 // " /= .txt"
  error stop
endif
print '(a)', "PASS: %suffix: " // s1

!> touch
p1 = path_t("touch.txt")
call p1%touch()
if (.not. p1%is_file()) error stop "touch"

!> copy_file, with_suffix
p2 = p1%with_suffix(".dat")
if(p2%path() /= "touch.dat") error stop "with_suffix: " // p2%path()
call p1%copy_file(p2%path(), overwrite=.true.)
if (.not. p2%is_file()) error stop "copy_file"

!> same_file, remove
p2 = path_t("./touch.txt")
if (.not. p1%same_file(p2)) error stop "same_file"
call p2%remove()

!> is_reserved, is_char_device
p1 = path_t("/dev/null")
b = p1%is_reserved()
b = p1%is_char_device()

!> mkdir
p1 = path_t("test_dir")
call p1%mkdir(ok=b)
if (.not. b) error stop "mkdir"

!> is_symlink, create_symlink, read_symlink
p1 = path_t("touch.txt")
call p1%touch()

p2 = path_t("touch.link")
if(p2%is_file()) call p2%remove()

call p1%create_symlink(p2%path())
if (.not. p2%is_symlink()) error stop "create_symlink: " // p2%path()

if (.not. is_windows()) then
p2 = p2%read_symlink()
if (p2%path() /= p1%path()) error stop "read_symlink " // p2%path()
endif

!> cleanup
call p2%remove()


end block valgrind

print '(a)', "OK: path methods"

contains

subroutine test_setter_getter()

type(path_t) :: p1

p1 = path_t("a/b/c")

if (p1%path(2,3) /= "/b") error stop "getter start,end"
if (p1%path(3,3) /= "b") error stop "getter same"
if (p1%path(2) /= "/b/c") error stop "getter start only"

end subroutine

end program
