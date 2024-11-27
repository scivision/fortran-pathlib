#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(){

if (fs_is_empty("."))
  err("is_empty(.) should be false generally");

if(!fs_mkdir("test-empty-dir"))
  err("Failed to create test-empty-dir");

if (!fs_is_empty("test-empty-dir"))
  err("is_empty(test-empty-dir) should be true");

if(!fs_touch("test-empty-file"))
  err("Failed to create test-empty-file");

if (!fs_is_empty("test-empty-file"))
  err("is_empty(test-empty-file) should be true");

ok_msg("is_empty C++");

return EXIT_SUCCESS;

}
