#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(){

  std::string s = fs_get_shell();
  if (s.empty())
    skip("shell: unknown value");

  if (s.length() == fs_get_max_path())
    err("shell has blank space");

  ok_msg("shell: " + s);

  return EXIT_SUCCESS;
}
