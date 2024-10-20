#include "ffilesystem.h"
#include <iostream>
#include <cstdlib>
#include <string>


int main()
{

auto cwd = fs_get_cwd();
if(!cwd){
  std::cerr << "ERROR: current working dir not found\n";
  return EXIT_FAILURE;
}

std::string r = fs_realpath("test");
if(r != cwd.value() + "/test")
  std::cerr << "ERROR: fs_realpath(test) " << r << " != " << cwd.value() << "/test\n";

r = fs_realpath(".");
if (r != cwd.value())
  std::cerr << "ERROR: fs_realpath(.) " << r << " != " << cwd.value() << "\n";

return EXIT_SUCCESS;

}
