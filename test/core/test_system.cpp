#include <cstdlib>
#include <string>
#include <iostream>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{
  std::size_t skip = 0;
  std::size_t fail = 0;

  const auto max_path = fs_get_max_path();

  std::string s = fs_compiler();
  if (s.empty()){
    std::cerr << "unknown compiler\n";
    skip++;
  }
  else if (s.length() == max_path){
    std::cerr << "FAILED: compiler has blank space\n";
    fail++;
  }
#ifdef __cpp_lib_string_contains
  else if (s.contains("%")){
    std::cerr << "FAILED: fs_compiler has formatting problem\n";
    fail++;
  }
#endif
  else
    std::cout << "OK: compiler: " << s << "\n";

  s = fs_get_username();
  if (s.empty()){
    std::cerr << "FAILED: failed to get username\n";
    fail++;
  }
  else if (s.length() == max_path){
    std::cerr << "FAILED: username has blank space\n";
    fail++;
  }
  else
    std::cout << "OK: username: " << s << "\n";

// -----------------

  std::cout << "skip: " << skip << " fail: " << fail << "\n";

  if(fail)
    return EXIT_FAILURE;
  if(skip)
    return 77;

  ok_msg("system C++");

  return EXIT_SUCCESS;
}
