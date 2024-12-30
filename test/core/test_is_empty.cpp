#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(){

std::string f;

if (fs_is_empty("."))
  err("is_empty(.) should be false generally");


f = "test_is_empty_cpp_dir";
if(!fs_mkdir(f))
  err("Failed mkdir " + f);

if (!fs_is_empty(f))
  err("is_empty(" + f + ") should be true");


f = "test_is_empty_cpp.txt";
if(!fs_touch(f))
  err("Failed to touch " + f);

if (!fs_is_empty(f))
  err("is_empty(" + f + ") should be true");


f = "not-exist-is-empty_cpp";
if (fs_is_empty(f))
  err("is_empty(" + f + ") should be false");

ok_msg("is_empty C++");

return EXIT_SUCCESS;

}
