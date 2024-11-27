#include <iostream>
#include <cstdlib>


#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

if (fs_is_absolute("")) {
  std::cerr << "blank is not absolute\n";
  return EXIT_FAILURE;
}

if(fs_is_windows()){
  if (!fs_is_absolute("J:/"))
    err("is_absolute(J:/) on Windows should be true");

  if (!fs_is_absolute("j:/"))
    err("is_absolute(j:/) on Windows should be true");

  if (fs_is_absolute("/"))
    err("is_absolute(/) on Windows should be false");

} else {
  if (!fs_is_absolute("/"))
    err("is_absolute(/) on Unix should be true");

  if (fs_is_absolute("j:/"))
    err("is_absolute(j:/) on Unix should be false");
}

  ok_msg("is_absolute C++");

  return EXIT_SUCCESS;
}
