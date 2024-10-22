#include <iostream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main()
{

constexpr std::string smiley = "😀";
constexpr std::string wink = "😉";
constexpr std::string hello = "你好";

auto u1 = fs_canonical(".", true, false);
if(u1.empty())
  return EXIT_FAILURE;

std::cout << "canonical(.): " << u1 << "\n";

for ( const auto &u : {smiley, wink, hello} ) {
  if (auto u2 = fs_file_name("./" + u); u2 != u)
    err("file_name(./" + smiley + ") != " + u2 + " " + u);

  u1 = fs_canonical(u, false, false);
  if(u1.empty())
    return EXIT_FAILURE;

  std::cout << "canonical(" + u + "): " << u1 << "\n";
  if (u1 != u)
    err("canonical UTF8: "  + u1 + " != " + u);
}

return EXIT_SUCCESS;
}
