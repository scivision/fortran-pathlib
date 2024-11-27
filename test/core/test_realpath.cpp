#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

auto cwd = fs_get_cwd();
if(cwd.empty())
  err("current working dir not found");


std::cout << "cwd: " << cwd << " length " << cwd.length() << "\n";

int e = 0;

const std::string r = fs_realpath(".");
if (r != cwd){
  std::cerr << "FAILED: fs_realpath(.) " << r << " != " << cwd << "\n";
  std::cerr << r << " length " << r.length() << "\n";
  e++;
} else
  std::cout << "fs_realpath(.) " << r << "\n";

const std::string name = "not-exist-realpath";
const std::string not_exist = cwd + "/" + name;
const std::string rt = fs_realpath(name);
if(rt != not_exist){
  if(fs_is_cygwin()){
    std::cerr << "On Cygwin, realpath() returns empty string for non-existing file\n";
  } else {
    std::cerr << "FAILED: fs_realpath(test) " << rt << " != " << not_exist << "\n";
    std::cerr << rt << " length " << rt.length() << "\n";
    e++;
  }
} else
  std::cout << "fs_realpath(test) " << rt << "\n";

const std::string p = fs_realpath("..");
if (p.length() >= r.length()){
  std::cerr << "FAILED: fs_realpath(..) " << p << " >= " << r << "\n";
  std::cerr << p << " length " << p.length() << "\n";
  std::cerr << r << " length " << r.length() << "\n";
  e++;
} else
  std::cout << "fs_realpath(..) " << p << " < " << r << "\n";

if(e)
  err("fs_realpath failed");

ok_msg("fs_realpath C++");

return EXIT_SUCCESS;

}
