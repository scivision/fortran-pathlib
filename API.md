# Fortran Filesystem API

Fortran filesystem modules contains numerous procedures and one (optional, default enabled) Fortran type "path_t" that contains properties and methods.

C++ stdlib `<filesystem>` is used extensively within Ffilesystem to implement functions in a platform-agnostic and robust way.
Fallback to C99 functions is available for compilers that do not support C++ [<filesystem>](https://en.cppreference.com/w/cpp/header/filesystem).
For the interchange of character strings between Fortran and C++ / C, the buffer length is determined at compile time and is available in `fs_get_max_path()` (C, C++) or `max_path()` (Fortran).

```fortran
integer :: m
m = max_path()
```

## path_t

`path_t` can be manually disabled with CMake by setting `cmake -DHAVE_F03TYPE=0`.

The "path_t" type uses getter and setter procedure to access the path as a string `character(:), allocatable`.

```fortran
use filesystem, only : path_t

type(path_t) :: p

p = path_t("my/path")  !< setter

print *, "path: ", p%path() !< getter
```

The retrieved path string may be indexed like:

```fortran
p%path(2,4)  !< character index 2:4

p%path(2) !< character index 2:end
```

In all the examples, we assume "p" is path_t.

## System capabilities

Character, allocatable: the Fortran compiler name and version

```sh
compiler()
```

Character, allocatable: the C/C++ compiler name and version.

* C++ backend: output is non-empty if c++20 std::format is available
* C backend: output is non-empty for known compilers

```sh
compiler_c()
```


Logical: ffilesystem is using C++ backend

```fortran
fs_cpp()
```

integer (long): the C `__STDC_VERSION__` or C++ level of macro `__cplusplus`

```fortran
fs_lang()
```

## subroutines

These subroutines are available in the "filesystem" module.

```fortran
call create_symlink("my/path", "my/symlink", ok)

logical, intent(out), optional :: ok !< true if succeeded
```

Copy source to destination.
Optionally, overwrite existing file.

```fortran
character(*) :: dest = "new/file.ext"

call p%copy_file(dest)
! or
call copy_file("original.txt", "acopy.txt")

! overwrite
call copy_file("original.txt", "acopy.txt", overwrite=.true.)

character(*), intent(in) :: source, dest
logical, intent(in), optional :: overwrite
logical, intent(out), optional :: ok !< true if successful
```

Make directory with parent directories if specified

```fortran
p = path_t("my/new/dir")
! suppose only directory "my" exists
call p%mkdir()
! now directory my/new/dir exists
! OR
call mkdir("my/new/dir")
```

Touch file (create empty file if not a file).
The directories containing the file must already exist.
Also updates the file access/modification times to current time.

```fortran
call p%touch()
! or
call touch("myfile.ext")
```

Get last modified time of path.

```fortran
use, intrinsic :: iso_c_binding, only : C_LONG
integer(C_LONG) :: mtime

mtime = get_modtime("my/file.txt")
```

Set modified time of path to current time.

```fortran
logical :: ok

ok = set_modtime("my/file.txt")
```


Delete file, empty directory, or symbolic link (the target of a symbolic link is not deleted).

```fortran
call p%remove()
! or
call remove("my/file.txt")
```

create symbolic link to file or directory:

```fortran
call p%create_symlink(link)
! or
call create_symlink(target, link)
```

## path_t

These methods emit a new "path_t" object.
It can be a new path_t object, or reassign to the existing path_t object.

On Windows, force file separators (if any) to Posix "/"

```fortran
p = path_t('my\path')
p = p%as_posix()

! my/path
```

Expand home directory, swapping file separators "\" for "/" and drop redundant file separators "//".

```fortran
! Fortran does not understand tilde "~"

p = path_t("~/my/path")
p = p%expanduser()
```

Read symlink target if path is a symbolic link--empty string if not a symlink.

```fortran
target = p%read_symlink()
! or
target = read_symlink("my/symlink")
```

Resolve path. This means to canonicalize the path, normalizing, resolving symbolic links, and resolving relative paths when the path exists.
This is distinct from canonical, which does not pin relative paths to a specific directory when the path does not exist.

```fortran
p = path_t("~/../b")
p = p%resolve()

p%path() == "<absolute path of user home directory>/b"

! --- relative path resolved to current working directory
p = path_t("../b")
p = p%resolve()

p%path() == "<absolute path of current working directory>/b"
```

Canoicalize path. This means to normalize, resolve symbolic links, and resolve relative paths when the path exists.
If the path doesn't exist and no absolute path is given, the path is resolved as far as possible with existing path components, and then ".", ".." are lexiographically resolved.

```fortran
p = path_t("~/../b")
p = p%canonical()

p%path() == "<absolute path of user home directory>/b"

! --- relative path resolved to current working directory
p = path_t("../b")
p = p%canonical()

p%path() == "../b"
```

Swap file suffix

```fortran
p = path_t("my/file.h5")

p = p%with_suffix(".hdf5")

! p%path() == "my/file.hdf5"
```

Normalize path, a lexical operation removing ".." and "." and duplicate file separators "//".
The path need not exist.
Trailing file separators are gobbled.

```fortran
p = p%normal()
! or
normal("./my//path/../b/")  !< "my/b"
```

Join path with other path string using posix separators.
The paths are treated like strings.
No path resolution is used, so non-sensical paths are possible for non-sensical input.

```fortran
p = path_t("a/b")

p = p%join("c/d")

! p%path == "a/b/c/d"
```

## integer(int64)

These procedures emit an 64-bit integer value.

len_trim() of p%path()

```fortran
p%length()
```

File size (bytes):

```fortran
p%file_size()
! or
file_size("my/file.txt")
```

Space available on drive containing path (bytes):

```fortran
p%space_available()
! or
space_available("my/file.txt")
```

## logical

These methods emit a logical value.

Does directory exist:

```fortran
p%is_dir()
! or
is_dir("my/dir")
```

Error stop if directory does not exist

```fortran
call assert_is_dir("my/dir")
```

Is path a subdirectory under (not just equal to) of "dir". this is a lexical operation.

```fortran
p%is_subdir(dir)

is_subdir("my/dir", "my")
```

Is filename "safe" (no path separators, no reserved names, no special characters, no white space):

```fortran
logical :: is_safe_name()

is_safe_name("my_file.txt")
```

---

Is "path" a file or directory (or a symbolic link to existing file or directory).
Like Python pathlib.Path.exists(), even if the path does not have read permission,
it still may exist.

```fortran
p%exists()
! or
exists("my/file.txt")
```

Does file exist (or a symbolic link to an existing file).
Like Python pathlib.Path.is_file(), even if the file does not have read permission,
it still may exist.

```fortran
p%is_file()
! or
is_file("my/file.txt")
```

Is the path a special character device (like a terminal or /dev/null)?

```fortran
p%is_char_device()
! of
is_char_device("/dev/null")
```

On Windows, is the path a reserved name (like "nul" on Windows)?

```fortran
p%is_reserved()
! or
is_reserved("nul")
```

Error stop if file does not exist

```fortran
call assert_is_file("my/dir")
```

Is path a symbolic link:

```fortran
p%is_symlink()
! or
is_symlink("my/path")
```

Is path absolute:

```fortran
p%is_absolute()
! or
is_absolute("my/path")
```

Does path "p" *resolve* to the same path as "other".
Does not expanduser().
To be true:

* path must exist
* path must be traversable  E.g. "a/b/../c" resolves to "a/c" iff a/b also exists.
* symlink resolves to its target

```fortran
p%same_file(other)
! or
same_file(path1, path2)
```

## file permissions

Is file executable by the user. Even if the file does not have read permission,
it still may be executable. False for directories.

```fortran
!! logical

p%is_exe()
! or
is_exe("my/file.exe")
```

---

Make regular file executable (or not) for owner.

Windows: set_permissions(path, executable=) does NOT work (MinGW, oneAPI, MSVC).

```fortran
!! subroutine

call p%set_permissions(readable, writable, executable=.true., ok)
! or
call set_permissions("my/file.exe", executable=.true.)

logical, intent(in), optional :: readable, writable, executable
logical, intent(out), optional :: ok  !< true if successful
```

---

Is path (file or directory) readable by the user.

```fortran
!! logical

p%is_readable()
! or
is_readable("my/file.txt")
is_readable("./")
```

Is path (file or directory) writable by the user.

```fortran
!! logical

p%is_writable()
! or
is_writable("my/file.txt")
is_writable("./")
```

## character(:), allocatable

These procedures emit a string.

---

Force file separators (if any) to Posix "/"

```fortran
as_posix('my\path')
! my/path
```

Join path_t with other path string using posix separators.
The paths are treated like strings.
No path resolution is used, so non-sensical paths are possible for non-sensical input.

```fortran
join("a/b", "c/d") ! "a/b/c/d"
```

---

Find executable file on environment variable PATH if present.
Windows must include the ".exe" suffix.
Windows prioritizes CWD.
Does not resolve path--if Windows CWD or relative path is in PATH, may output relative path.
Does not expanduser tilde.

```fortran
character(:), allocatable :: which("myprog")
```

---

Get file suffix: extracts path suffix, including the final "." dot

```fortran
p%suffix()
! or
suffix("my/file.txt")  !< ".txt"
```

Swap file suffix

```fortran
with_suffix("to/my.h5", ".hdf5")  !< "to/my.hdf5"
```

Get parent directory of path.
The parent of the top-most relative path is ".".
We define the parent of a path as the directory above the specified path.
Trailing slashes are gobbled.
The path is not normalized.

```fortran
p%parent()
! or
parent("my/file.txt")  !< "my"

parent("a") !< "."
```

Get file name without path:

```fortran
p%file_name()
! or
file_name("my/file.txt")  ! "file.txt"
```

Get file name without path and suffix:

```fortran
p%stem()
! or
stem("my/file.txt")  !< "file"
```

Get path separator: ":" for Unix, ";" for Windows

```fortran
character :: s = pathsep()
```

Get null path: "/dev/null" on Unix, "nul" on Windows

```fortran
character(:), allocatable :: n = devnull()
```

Get filesystem type of path.
E.g. "NTFS" or "ext4". WSL shows "v9fs" for Windows paths.

```fortran
character(:), allocatable :: p%filesystem_type()

! or

filesystem_type("my/file.txt")  !< "NTFS" or "ext4" or ...
```

Get drive root. E.g. Unix "/"  Windows "c:"
Requires absolute path or will return empty string.

```fortran
p%root()
! or
root("/a/b/c") !< "/" on Unix, "" on Windows

root ("c:/a/b/c") !< "c:" on Windows, "" on Unix
```

Expand user home directory.
With C++ backend, the path is not normalized.
With C backend, the path is normalized, to drastically simplify the code.

```fortran
expanduser("~/my/path")   !< "/home/user/my/path" on Unix, "<root>/Users/user/my/path" on Windows
```

Resolve (canonicalize) path.
First attempts to resolve an existing path.
If that fails, the path is resolved as far as possible with existing path components, and then ".", ".." are lexiographically resolved.

```fortran
resolve("~/../b")

! --- relative path resolved to current working directory
resolve("../b")
```

Windows: long to short path

```fortran
shortname("C:/Program Files")  !< "C:/PROGRA~1"
```

Windows: short to long path

```fortran
longname("C:/PROGRA~1")  !< "C:/Program Files"
```

Get path relative to other path. This is a purely lexical operation.

```fortran
relative_to(base, other)
character(*), intent(in) :: base, other


relative_to("/a/b/c", "/a/b")  !< "c"

p = path_t("/a/b/c")
p%relative_to("/a")  !< "b/c"

p%relative_to("d")  !< ""
```

Get path proximate to other path. This is a purely lexical operation.

```fortran
proximate_to(base, other)
character(*), intent(in) :: base, other

proximate_to("/a/b/c", "/a/b")  !< "c"

p = path_t("/a/b/c")
p%proximate_to("d")  !< "d"
```

## System

Is Ffilesystem using C or C++ filesystem backend:

```fortran
logical :: as_cpp()
```

Get home directory, or empty string if not found.

```fortran
character(:), allocatable :: get_homedir()
```

Get full path of main executable, regardless of current working directory

```fortran
character(:), allocatable :: exe_path()
```

Get full path of **SHARED LIBRARY**, regardless of current working directory.
If static library, returns empty string.
To use `lib_path()`, build Ffilesystem with `cmake -DBUILD_SHARED_LIBS=on`

```fortran
character(:), allocatable :: lib_path()
```

Get current working directory

```fortran
character(:), allocatable :: get_cwd()
```

Change current working directory (chdir):

```fortran
logical :: ok
ok = set_cwd("my/path")
```

Get environment variable (allocatable character function vs. Fortran 2003 subroutine get_environment variable()):

```fortran
character(:), allocatable :: getenv(name)
```

Set environment variable:

```fortran
logical, optional :: ok

call setenv(name, value, ok=ok)
```

Get system or user temporary directory:

```fortran
character(:), allocatable :: get_tempdir()
```

Create a (probably) unique temporary directory.
This directory is not deleted automatically, or secure.

```fortran
character(:), allocatable :: make_tempdir()
```

Make a path absolute if relative:

```fortran
function make_absolute(path, top_path)
!! if path is absolute, return expanded path
!! if path is relative, top_path / path
!!
!! idempotent iff top_path is absolute

character(:), allocatable :: make_absolute
character(*), intent(in) :: path, top_path
```

Tell characteristics of the computing platform such as operating system:

```fortran
! logical based on C preprocessor

is_admin()
is_bsd()
is_unix()
is_linux()
is_windows()
is_macos()
is_cygwin()
is_mingw()
```

logical: is the user running as admin / root / superuser:

```fortran
is_admin()
```

```fortran
C_INT  is_wsl()  !< Windows Subsystem for Linux > 0 if true
```
