program test_normal

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p

p = normal("")
if(p /= "") error stop "normal empty: " // p

p = normal("a")
if(p /= "a") error stop "normal a: " // p

p = normal("a/")
if(p /= "a") error stop "normal a/: " // p

p = normal("a//")
if(p /= "a") error stop "normal a//: " // p

p = normal("./a")
if(p /= "a") error stop "normal ./a: " // p

p = normal("./a/")
if(p /= "a") error stop "normal ./a/: " // p

p = normal("../a")
if(p /= "../a") error stop "normal ../a: " // p

p = normal("a/b/")
if(p /= "a/b") error stop "normal(a/b/): " // p

p = normal("a/b/.")
if(p /= "a/b") error stop "normal(a/b/.): " // p

p = normal("a/b/..")
if(p /= "a") error stop "normal(a/b/..): " // p

p = normal("a/b/../")
if(p /= "a") error stop "normal(a/b/../): " // p

end block valgrind

end program
