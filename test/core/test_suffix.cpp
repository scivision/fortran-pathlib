#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

if(fs_suffix("") != "")
  err("suffix empty");

std::string s1 = fs_suffix("suffix_name.a.b");

if (s1 != ".b")
  err("suffix failed: " + s1);

s1 = fs_suffix(s1);
if (s1 != "")
  err("suffix recursive failed: " + s1);

s1 = fs_suffix(".suffix");
if (!s1.empty())
  err("suffix leading dot failed: " + s1);

s1 = fs_suffix("./.suffix");
if (!s1.empty())
  err("suffix leading dot filename cwd failed: " + s1);

s1 = fs_suffix(".suffix.txt");
if (s1 != ".txt")
  err("suffix leading dot filename w/ext failed: " + s1);

s1 = fs_suffix("./.suffix.txt");
if (s1 != ".txt")
  err("suffix leading dot filename w/ext and cwd failed: " + s1);

s1 = fs_suffix("../.suffix.txt");
if (s1 != ".txt")
  err("suffix leading dot filename w/ext up failed: " + s1);

ok_msg("suffix");

return EXIT_SUCCESS;
}
