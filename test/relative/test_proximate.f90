program test

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none

valgrind : block

type(path_t) :: p1
character(:), allocatable :: rel

rel = proximate_to("/", "")
if(rel /= "") error stop "empty other should be empty: " // rel

rel = proximate_to("", "")
if(rel /= "") error stop "empty path and base should be empty: " // rel

rel = proximate_to("", "/")
if(rel /= "/") error stop "empty base should be other: " // rel

print '(a)', "OK: proximate_to: empty"


rel = proximate_to("Hello", "Hello")
if(rel /= ".") error stop "same path relative exact should be . but got: "  // rel

rel = proximate_to("/", "/")
if(rel /= ".") error stop "same path '/' should be . but got: "  // rel

rel = proximate_to("Hello", "Hello/")
if(rel /= ".") error stop "same path 'Hello' vs. 'Hello/' should be . but got: "  // rel

rel = proximate_to("/dev/null", "/dev/null")
if(rel /= ".") error stop "same path '/dev/null' should be . but got: "  // rel

print '(a)', "OK: proximate_to: same"

if(is_windows()) then
    rel = proximate_to("c:\a\b", "x")
    if(rel /= "x") error stop "abs path with rel base should be other: " // rel

    rel = proximate_to("x", "c:/a/b")
    if(rel /= "c:/a/b") error stop "rel path with abs base should be other: " // rel

    rel = proximate_to("c:/a/b", "c:/a/b")
    if(rel /= ".") error stop "same path should be . but got: "  // rel

    rel = proximate_to("c:/a/b", "c:/a")
    if(rel /= "..") then
    write(stderr,*) "ERROR: rel to parent 1: " // rel
    ! error stop
    endif

    rel = proximate_to("c:\a/b\c/d", "c:/a\b")
    if(rel /= "../..") error stop "rel to parent 2: " // rel

    p1 = path_t("c:/a/b/c/d")
    if (p1%proximate_to("c:/a/b") /= rel) error stop " OO rel to parent"

    rel = proximate_to("C:/path", "D:/path")
    if (rel /= "D:/path") error stop "different drives should be other: " // rel
else
    rel = proximate_to("/a/b", "c")
    if(rel /= "c") error stop "abs base with rel other should be other: " // rel

    rel = proximate_to("c", "/a/b")
    if(rel /= "/a/b") error stop "rel base with abs other should be other: " // rel

    rel = proximate_to("/a/b", "/a/b")
    if(rel /= ".") error stop "same path should be . but got: "  // rel

    rel = proximate_to("/a/b", "/a")
    if(rel /= "..") error stop "rel to parent 1: " // rel

    rel = proximate_to("/a/b/c/d", "/a/b")
    if(rel /= "../..") error stop "rel to parent 2: " // rel

    p1 = path_t("/a/b/c/d")
    if (p1%proximate_to("/a/b") /= rel) error stop " OO rel to parent"
endif

rel = proximate_to("./this/is/path_one", "./this/is/path_two")
if(rel /= "../path_two") error stop "rel to parent dot leading: " // rel

rel = proximate_to("/this/is/path_one", "/this/is/path_two")
if(rel /= "../path_two") error stop "rel to parent slash leading: " // rel

rel = proximate_to("/path/same", "/path/same/ho/..")
if(rel /= ".") error stop "rel to parent double dot trail: " // rel

end block valgrind

end program
