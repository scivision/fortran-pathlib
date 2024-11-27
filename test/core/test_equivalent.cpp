#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
int argc,
char* argv[])
{
  std::string_view c1 = argv[0];
  std::cout << "own command: " << c1 << "\n";
  const std::string cwd = fs_get_cwd();
  std::cout << "current directory: " << cwd << "\n";

  std::string s1 = fs_file_name(c1);
  std::string s2 = "./" + s1;

  if (!fs_is_file(s2))
    err("ERROR: file name: " + s2 + " is not a file");

  if (!fs_is_file(s1))
    err("ERROR: file name: " + s1 + " is not a file");

  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";

  if (!fs_equivalent(s1, s2))
    err("ERROR: equivalent(" + s1 + " /= " + s2 + ")");

  s1 = fs_parent(s1);
  s2 = "./" + s1;

  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";

  if (!fs_equivalent(s1, s2))
    err("ERROR: equivalent(" + s1 + " /= " + s2 + ")");


  s1 = "..";
  s2 = fs_parent(fs_get_cwd());
  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";
  if (!fs_equivalent(s1, s2))
    err("ERROR: equivalent(" + s1 + " /= " + s2 + ")");

  if (!fs_equivalent(".", "./"))
    err("ERROR: equivalent(., ./)");


  s1 = ".";
  s2 = fs_get_cwd();
  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";
  if (!fs_equivalent(s1, s2))
    err("ERROR: equivalent(" + s1 + " /= " + s2 + ")");

  fs_mkdir("test-a/b/");

  s1 = "test-a/c";
  fs_touch(s1);

  s2 = "test-a/b/../c";

  if (!fs_equivalent(s1, s2))
    err("ERROR: equivalent resolve");

  if (fs_equivalent("not-exist-same", "not-exist-same"))
    err("ERROR: equivalent(not-exist-same, not-exist-same)");

  // non-ASCII
  s1 = "日本語_equivalent";
  if(!fs_mkdir(s1))
    err("ERROR: mkdir(" + s1 + ")");


  if(!fs_equivalent("./" + s1, s1))
    err("ERROR: equivalent(./" + s1 + ", " + s1 + ")");

  ok_msg("equivalent C++");

  return EXIT_SUCCESS;
}
