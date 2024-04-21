program test_suffix

use filesystem

implicit none

valgrind : block

character(:), allocatable :: s1

if(suffix("") /= "") error stop "suffix empty"

s1 = suffix("suffix_name.a.b")

if (s1 /= ".b") error stop "%suffix failed: " // s1

s1 = suffix(s1)
if (s1 /= "") error stop "suffix recursive failed: " // s1

if(len_trim(suffix(".suffix")) /= 0) error stop "suffix leading dot filename: " // suffix(".suffix")
if(len_trim(suffix("./.suffix")) /= 0) error stop "suffix leading dot filename cwd: " // suffix("./.suffix")
if(suffix(".suffix.txt") /= ".txt") error stop "suffix leading dot filename w/ext"
if(suffix("./.suffix.txt") /= ".txt") error stop "suffix leading dot filename w/ext and cwd"
if(suffix("../.suffix.txt") /= ".txt") error stop "suffix leading dot filename w/ext up"

end block valgrind

end program
