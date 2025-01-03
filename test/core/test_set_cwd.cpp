#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(int argc, char* argv[])
{

std::string_view s1 = (argc > 1) ? argv[1] : argv[0];

std::cout << "file name: " << s1 << "\n";

std::string s2 = fs_parent(fs_parent(s1));
std::cout << "parent(parent(' << s1 << ')) = " << s2 << "\n";

std::string old_cwd = fs_get_cwd();

std::cout << "current working directory: " << old_cwd << "\n";

bool ok = fs_set_cwd(s2);

if (!ok)
  err("chdir failed");

if(fs_set_cwd(""))
  err("chdir should fail on empty string");

std::string cwd = fs_get_cwd();
std::cout << "New working directory: " << cwd << "\n";

if (!fs_equivalent(cwd, s2))
  err("chdir failed: " + s2 + " /= " + cwd);

ok_msg("set_cwd C++");

return EXIT_SUCCESS;
}
