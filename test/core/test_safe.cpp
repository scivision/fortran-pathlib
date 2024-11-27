#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

/// is safe name expects ONLY the filename, not the path

std::string s;

s = "test/re/";

if(fs_is_safe_name(s))
  err(s);

s = "test/re";
if(fs_is_safe_name(s))
  err(s);

s = "hi.";
bool ok = fs_is_safe_name(s);
if(fs_is_windows() && ok)
  err(s);
if(!fs_is_windows() && !ok)
  err(s);

s = "hi there";
if(fs_is_safe_name(s))
  err(s);

ok_msg("safe C++");

return EXIT_SUCCESS;
}
