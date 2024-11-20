#include <cstdlib>
#include <iostream>
#include <string>

#include "ffilesystem.h"


int main(){
  const std::string loc = fs_get_locale_name();

  std::cout << "locale: " << loc << "\n";
  // macOS and MSVC have empty locale. Don't fail

  return EXIT_SUCCESS;
}
