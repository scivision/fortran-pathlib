#include <iostream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}

int main()
{

std::string smiley = "😀";
std::string wink = "😉";
std::string hello = "你好";

// test allocation
auto u1 = fs_canonical(".", true, false);
if(!u1)
  return EXIT_FAILURE;

std::cout << "canonical(.): " << u1.value() << "\n";

for ( const auto &u : {smiley, wink, hello} ) {
  if (auto u2 = fs_file_name("./" + u); u2 != u)
    err("file_name(./" + smiley + ") != " + u2 + " " + u);

  u1 = fs_canonical(u, false, false);
  if(!u1)
    return EXIT_FAILURE;

  std::cout << "canonical(" + u + "): " << u1.value() << "\n";
  if (u1.value() != u)
    err("canonical UTF8: "  + u1.value() + " != " + u);
}

return EXIT_SUCCESS;
}
