program test_normal

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p

p = normal("")
if(p /= ".") error stop "normal() " // p

p = normal("/")
if(p /= "/") error stop "normal(/) " // p

p = normal("//")
if(p /= "/") error stop "normal(//) " // p

p = normal(".")
if(p /= ".") error stop "normal(.) " // p

p = normal("./")
if(p /= ".") error stop "normal(./) " // p

p = normal("./.")
if(p /= ".") error stop "normal(./.) " // p

p = normal("..")
if(p /= "..") error stop "normal(..) " // p

p = normal("a/..")
if(p /= ".") error stop "normal(a/..) " // p

p = normal("../..")
if(p /= "../..") error stop "normal(../..) " // p

p = normal("/a")
if(p /= "/a") error stop "normal(/a) " // p

p = normal("a")
if(p /= "a") error stop "normal(a) " // p

p = normal(".a")
if(p /= ".a") error stop "normal(.a) " // p

p = normal("a.")
if(p /= "a.") error stop "normal(a.) " // p

p = normal("a./")
if(p /= "a.") error stop "normal(a./) " // p

p = normal("..a")
if(p /= "..a") error stop "normal(..a) " // p

p = normal("a..")
if(p /= "a..") error stop "normal(a..) " // p

p = normal("a../")
if(p /= "a..") error stop "normal(a../) " // p

p = normal("a/")
if(p /= "a") error stop "normal(a/) " // p

p = normal("a//")
if(p /= "a") error stop "normal(a//) " // p

p = normal("./a")
if(p /= "a") error stop "normal(./a) " // p

p = normal("./a/")
if(p /= "a") error stop "normal(./a/) " // p

p = normal("./a/.")
if (p /= "a") error stop "normal(./a/.) " // p

p = normal("../a")
if(p /= "../a") error stop "normal(../a) " // p

p = normal("../a/b/..")
if(p /= "../a") error stop "normal(../a/b/..) " // p

p = normal("a/b/")
if(p /= "a/b") error stop "normal(a/b/) " // p

p = normal("a/b/.")
if(p /= "a/b") error stop "normal(a/b/.) " // p

p = normal("a/b/..")
if(p /= "a") error stop "normal(a/b/..) " // p

p = normal("a/b/../")
if(p /= "a") error stop "normal(a/b/../) " // p

p = normal("a/b/../c")
if(p /= "a/c") error stop "normal(a/b/../c) " // p

p = normal("a/b/../c/d")
if(p /= "a/c/d") error stop "normal(a/b/../c/d) " // p

p = normal("a/b/../../c/d")
if(p /= "c/d") error stop "normal(a/b/../../c/d) " // p

p = normal("././a/./b/././c/./.")
if(p /= "a/b/c") error stop "normal(././a/./b/././c/./.) " // p

p = normal("a/b/../../c/../..")
if(p /= "..") error stop "normal(a/b/../../c/../..) " // p

p = normal("a/b/../../../c/../..")
if(p /= "../..") error stop "normal(a/b/../../../c/../..) " // p

end block valgrind

print '(a)', "OK: normal"

end program
