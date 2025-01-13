#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

  const auto max_component = fs_max_component("/");
  const std::string s = std::to_string(max_component);
  if(max_component < 1)
    err("max_component: " + s);

  ok_msg("max_component: " + s);

  return EXIT_SUCCESS;
}
