#include "ffilesystem.h"

#include <cstdlib>
#include <string>
#include <iostream>

int main(){

  size_t skip = 0;
  size_t fail = 0;

  double load = fs_cpu_loadavg();
  if (load <= 0.0){
    std::cerr << "CPU load average not implemented on this platform\n";
    skip++;
  }
  else
    std::cout << "OK: CPU load average: " << load << "\n";

  const auto max_path = fs_get_max_path();

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

  return EXIT_SUCCESS;
}
