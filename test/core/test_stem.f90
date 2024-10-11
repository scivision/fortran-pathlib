program test_stem

use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem

implicit none


valgrind : block
character(:), allocatable :: s, p

s = stem("")
if(s /= "") error stop "stem empty: " // s

s = stem("stem.a.b")
if (s /= "stem.a") error stop "%stem failed: " // s
s = stem(s)
if (s /= "stem") error stop "stem nest failed: " // s

s = stem("stem")
if (s /= "stem") error stop "stem idempotent failed: " // s

s = stem(".stem")
if(s /= ".stem") error stop "stem leading dot filename idempotent: " // s

s = stem("./.stem")
if(s /= ".stem") error stop "stem leading dot filename cwd: " // s

s = stem(".stem.txt")
if(s /= ".stem") error stop "stem leading dot filename w/ext " // s

s = stem("./.stem.txt")
if(s /= ".stem") error stop "stem leading dot filename w/ext and cwd " // s

s = stem("../.stem.txt")
if(s /= ".stem") then
    write(stderr, '(a)') "stem leading dot filename w/ext up " // s
    error stop
end if

s = stem("stem.")
if (s /= "stem") error stop "stem trailing dot filename idempotent: " // s

s = stem("a/..")
if (s /= "..") error stop "stem parent directory: " // s

s = stem("a/../")
if (s /= "") error stop "stem parent directory trailing slash: " // s

s = stem("a/.")
if (s /= ".") error stop "stem parent directory ." // s

if(is_windows()) then
  p = "C:\\a\\ball.text"
  s = stem(p)
  if (s /= "ball") error stop "stem(" // p // ") windows path: " // s // " != ball"

endif

end block valgrind

print '(a)', "OK: stem"

end program
