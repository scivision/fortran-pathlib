#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(){

  std::string s = fs_get_terminal();

  if (s.empty())
    skip("unknown terminal");

  if (s.length() == fs_get_max_path())
    err("terminal has blank space");

  ok_msg("terminal: " + s);

  return EXIT_SUCCESS;
}
