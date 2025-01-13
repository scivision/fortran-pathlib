#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

  std::string s = fs_hostname();
  if (s.empty())
    err("failed to get hostname");

  if (s.length() == fs_get_max_path())
    err("hostname has blank space");

  ok_msg("hostname: " + s);

  return EXIT_SUCCESS;

}
