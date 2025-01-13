#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){
  const std::string loc = fs_get_locale_name();

  std::cout << "locale: " << loc << "\n";
  // macOS and MSVC have empty locale. Don't fail

  ok_msg("locale C++");

  return EXIT_SUCCESS;
}
