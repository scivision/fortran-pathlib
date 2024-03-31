program test

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

valgrind : block

type(path_t) :: p1
character(:), allocatable :: rel

rel = relative_to("/", "")
if(rel /= "") error stop "empty base should be empty: " // rel

rel = relative_to("", "")
if(rel /= "") error stop "empty path and base should be empty: " // rel

rel = relative_to("", "/")
if(rel /= "") error stop "empty path should be empty: " // rel

print '(a)', "OK: relative_to: empty"

rel = relative_to("/", "/")
if(rel /= ".") error stop "same path '/' should be . but got: "  // rel

rel = relative_to("Hello", "Hello")
if(rel /= ".") error stop "same path 'Hello'should be . but got: "  // rel

rel = relative_to("Hello", "Hello/")
if(rel /= ".") error stop "same path 'Hello' vs. 'Hello/' should be . but got: "  // rel

rel = relative_to("/dev/null", "/dev/null")
if(rel /= ".") error stop "same path '/dev/null' should be . but got: "  // rel

print '(a)', "OK: relative_to: same"

if(is_windows()) then
    rel  = relative_to("c:\a\b", "c")
    if(rel /= "") error stop "abs path with rel base should be empty: " // rel

    rel = relative_to("c", "c:/a/b")
    if(rel /= "") error stop "rel path with abs base should be empty: " // rel

    rel = relative_to("c:/a/b", "c:/a/b")
    if(rel /= ".") error stop "same path should be . but got: "  // rel

    rel = relative_to("c:/a/b", "c:/a")
    if(rel /= "..") then
    write(stderr,*) "ERROR: rel to parent 1: " // rel
    ! error stop
    endif

    rel = relative_to("c:\a/b\c/d", "c:/a\b")
    if(rel /= "../..") error stop "rel to parent 2: " // rel

    p1 = path_t("c:/a/b/c/d")
    if (p1%relative_to("c:/a/b") /= rel) error stop " OO rel to parent"

    rel = relative_to("C:/path", "D:/path")
    if (rel /= "") error stop "different drives should be empty: " // rel
else
    rel = relative_to("/a/b", "c")
    if(rel /= "") error stop "abs path with rel base should be empty: " // rel

    rel = relative_to("c", "/a/b")
    if(rel /= "") error stop "rel path with abs base should be empty: " // rel

    rel = relative_to("/a/b", "/a/b")
    if(rel /= ".") error stop "same path should be . but got: "  // rel

    rel = relative_to("/a/b", "/a")
    if(rel /= "..") error stop "rel to parent 1: " // rel

    rel = relative_to("/a/b/c/d", "/a/b")
    if(rel /= "../..") error stop "rel to parent 2: " // rel

    p1 = path_t("/a/b/c/d")
    if (p1%relative_to("/a/b") /= rel) error stop " OO rel to parent"
endif

rel = relative_to("./this/is/path_one", "./this/is/path_two")
if(rel /= "../path_two") error stop "rel to parent dot leading: " // rel

rel = relative_to("/this/is/path_one", "/this/is/path_two")
if(rel /= "../path_two") error stop "rel to parent slash leading: " // rel

rel = relative_to("/path/same", "/path/same/ho/..")
if(rel /= ".") error stop "rel to parent double dot trail: " // rel

end block valgrind

end program
