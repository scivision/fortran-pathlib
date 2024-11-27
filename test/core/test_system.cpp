#include <cstdlib>
#include <string>
#include <iostream>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

  const std::string ci = fs_getenv("CI");
  bool is_ci = ci == "true";

  size_t skip = 0;
  size_t fail = 0;

  const auto max_path = fs_get_max_path();
  const auto max_component = fs_max_component("/");
  if(max_component < 1){
    std::cerr << "FAILED: max_component: " << max_component << "\n";
    fail++;
  }
  else
    std::cout << "OK: max_component: " << max_component << "\n";


  std::string s = fs_hostname();
  if (s.empty()){
    std::cerr << "FAILED: failed to get hostname\n";
    fail++;
  }
  else if (s.length() == max_path){
    std::cerr << "FAILED: hostname has blank space\n";
    fail++;
  }
  else
    std::cout << "OK: hostname: " << s << "\n";

  s = fs_compiler();
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

  s = fs_get_shell();
  if (s.empty()){
    std::cerr << "unknown shell\n";
    skip++;
  }
  else if (s.length() == max_path){
    std::cerr << "FAILED: shell has blank space\n";
    fail++;
  }
  else
    std::cout << "OK: shell: " << s << "\n";

  s = fs_get_terminal();
  if (s.empty()){
    std::cerr << "unknown terminal\n";
    if(!is_ci)
      skip++;
  }
  else if (s.length() == max_path){
    std::cerr << "FAILED: terminal has blank space\n";
    fail++;
  }
  else
    std::cout << "OK: terminal: " << s << "\n";

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
