#include <cstdlib>
#include <iostream>
#include <string_view>

#include <cerrno>


[[noreturn]]
void err(std::string_view m){
  std::cerr << "FAILED: " << m << " backend: " << fs_backend();

  if(errno){
  auto econd = std::generic_category().default_error_condition(errno);

  std::cerr << " " << econd.message();
  }

  std::cerr << std::endl;

  std::exit(EXIT_FAILURE);
}


[[noreturn]]
void skip(std::string_view m){
  std::cout << "SKIP: " << m << " backend: " << fs_backend() << "\n";
  std::exit(77);
}


void ok_msg(std::string_view m){
  std::cout << "OK: " << m << " backend: " << fs_backend() << "\n";
}
