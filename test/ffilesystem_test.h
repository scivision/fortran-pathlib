#include <cstdlib>
#include <iostream>
#include <string_view>


[[noreturn]]
void err(std::string_view m){
  std::cerr << "FAILED: " << m << " backend: " << fs_backend() << "\n";
  std::exit(EXIT_FAILURE);
}


void ok_msg(std::string_view m){
  std::cout << "OK: " << m << " backend: " << fs_backend() << "\n";
}
