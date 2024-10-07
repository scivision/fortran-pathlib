program test_join

use filesystem

valgrind : block

character(:), allocatable :: p

p = join("", "")
if(p /= "") error stop "join empty: " // p

p = join("a", "")
if(p /= "a") error stop "join a: " // p

p = join("", "b")
if(p /= "b") error stop "join b: " // p

p = join("a/b/" , "c/")
if(p /= "a/b/c") error stop "join(a/b, c/): " // p

p = join("/", "")
if(p /= "/") error stop "join(/,): " // p

p = join("", "/")
if(p /= "/") error stop "join('',/): " // p

p = join("a", "b//")
if(p /= "a/b") error stop "join(a, b//): " // p

p = join("a//", "b//")
if(p /= "a/b") error stop "join(a//, b//): " // p

p = join("a/b/../", "c/d/../")
if(p /= "a/c") error stop "join(a/b/../, c/d/../): " // p

p = join("a/b", "..")
if(p /= "a") error stop "join(a/b, ..): " // p

p = join("a/b", "c/d")
if (p /= "a/b/c/d") error stop "join(c/d): " // p

p = join("ab/cd", "/ef")
if (p /= "/ef") error stop "join(ab/cd, /ef): " // p

end block valgrind

print '(a)', "OK: join"

end program
