program main

use filesystem

implicit none

valgrind : block

type(path_t) :: p1

p1 = path_t("a/b/c")

if (.not. p1%is_subdir("a/b")) error stop "a/b/c is subdir of a/b"
if (.not. p1%is_subdir("a/b/")) error stop "a/b/c is subdir of a/b/"
if (.not. p1%is_subdir("a")) error stop "a/b/c is subdir of a"

if(p1%is_subdir("a/b/c")) error stop "a/b/c is not subdir of a/b/c"
if(p1%is_subdir("a/b/c/")) error stop "a/b/c/ is not subdir of a/b/c"

p1 = path_t("a/b/c/d/..")

if (.not. p1%is_subdir("a/b/")) error stop "a/b/c/d/.. is subdir of a/b/"

if (is_subdir("a/b", "c")) error stop "c is not subdir of a/b"
if (is_subdir("b", "a/b")) error stop "a/b is not subdir of b"

if(is_windows()) then

if(is_subdir("x:/a/b", "c")) error stop "c is not subdir of x:/a/b"
if(is_subdir("a/b", "x:/c")) error stop "x:/c is not subdir of a/b"
if(is_subdir("x:/a/b", "x:/c")) error stop "x:/c is not subdir of x:/a/b"
if(is_subdir("x:/a/b", "x:/a/b")) error stop "x:/a/b is not subdir of x:/a/b"
if(is_subdir("x:/a/b", "x:/a/b/")) error stop "x:/a/b/ is not subdir of x:/a/b"
if(is_subdir("x:/a/b/", "x:/a/b")) error stop "x:/a/b is not subdir of x:/a/b/"
if(is_subdir("x:/a/b", "b")) error stop "b is not subdir of x:/a/b"
if(is_subdir("x:/a/b", "a/b")) error stop "a/b is not subdir of x:/a/b"

else

if (is_subdir("/a/b", "c")) error stop "c is not subdir of /a/b"
if (is_subdir("a/b", "/c")) error stop "/c is not subdir of a/b"
if (is_subdir("/a/b", "/c")) error stop "/c is not subdir of /a/b"
if (is_subdir("/a/b", "/a/b")) error stop "/a/b is not subdir of /a/b"
if (is_subdir("/a/b", "/a/b/")) error stop "/a/b/ is not subdir of /a/b"
if (is_subdir("/a/b/", "/a/b")) error stop "/a/b is not subdir of /a/b/"
if (is_subdir("/a/b", "b")) error stop "b is not subdir of /a/b"
if (is_subdir("/a/b", "a/b")) error stop "a/b is not subdir of /a/b"

endif

end block valgrind

print '(a)', "OK: is_subdir()"

end program
