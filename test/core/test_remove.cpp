#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int
#ifdef _WIN32
wmain(int argc, wchar_t* argv[])
#else
main(int argc, char* argv[])
#endif
{

  if(argc < 2){
    std::cerr << "Please specify filename to remove\n";
    return EXIT_FAILURE;
  }

  const std::string j =
#ifdef _WIN32
    fs_to_narrow(argv[1]);
#else
  argv[1];
#endif

  std::cout << "Test file " << j << "\n";

  if(!fs_is_file(j)){
    std::cerr << "is_file(" << j << ") should be true for existing regular file\n";
    return 77;
  }

  if(!fs_remove(j)){
    if(fs_exists(j))
      err("remove(" + j + ") failed--file still exists");
    err("remove(" + j + ") failed");
  }

  ok_msg("remove(" + j + ") C++");

  return EXIT_SUCCESS;
}
