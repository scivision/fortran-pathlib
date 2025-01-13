#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(int argc, char* argv[])
{

  if(argc < 2){
    std::cerr << "Please specify filename to remove\n";
    return EXIT_FAILURE;
  }

  const std::string j = argv[1];

  std::cout << "Test file " << j << "\n";

  if(!fs_is_file(j))
    skip("is_file(" + j + ") should be true for existing regular file");

  if(!fs_remove(j)){
    if(fs_exists(j))
      err("remove(" + j + ") failed--file still exists");
    err("remove(" + j + ") failed");
  }

  ok_msg("remove(" + j + ") C++");

  return EXIT_SUCCESS;
}
