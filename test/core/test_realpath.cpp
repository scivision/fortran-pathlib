#include "ffilesystem.h"
#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem_test.h"


int main()
{

auto cwd = fs_get_cwd();
if(cwd.empty())
  err("current working dir not found");

std::string r = fs_realpath(".");
if (r != cwd){
  std::cerr << "FAILED: fs_realpath(.) " << r << " != " << cwd << "\n";
  std::cerr << "r: " << r << " length " << r.length() << "\n";
  std::cerr << "cwd: " << cwd << " length " << cwd.length() << "\n";
  err("fs_realpath(.)");
}
std::cout << "fs_realpath(.) " << r << "\n";


r = fs_realpath("test");
if(r != cwd + "/test"){
  std::cerr << "FAILED: fs_realpath(test) " << r << " != " << cwd << "/test\n";
  std::cerr << "CWD: " << cwd << "\n";
  err("fs_realpath(test)");
}

std::string p = fs_realpath("..");
if (p.length() >= r.length())
  err("fs_realpath(..) " + p + " >= " + r);
std::cout << "fs_realpath(..) " << p << " < " << r << "\n";

std::cout << "OK: C++ realpath\n";

return EXIT_SUCCESS;

}
