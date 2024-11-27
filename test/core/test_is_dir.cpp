#include <iostream>
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
  if (fs_is_dir(""))
    err("is_dir empty should be false");

  std::string s1 = fs_get_cwd();
  if (!fs_is_dir(s1))
    err("is_dir(get_cwd()) failed: " + s1);

  if (!fs_is_dir("."))
    err("did not detect '.' as directory");

  if (fs_is_file("."))
    err("detected '.' as file");

  if(!fs_is_dir("."))
    err("is_dir failed on '.'");

  if (!fs_exists(".") || !fs_is_dir("."))
    err("is_readable failed on '.'");

  std::string_view s2(argv[0]);
  if (fs_is_dir(s2))
    err("detected file as directory");

  if (fs_is_dir("not-exist-dir"))
    err("not-exist-dir should not exist");

  std::string fn = "日本語_is_dir";

  if (!fs_mkdir(fn))
    err("fs_mkdir " + fn);

  if (!fs_exists(fn))
    err("fs_exists " + fn);

  if (!fs_is_dir(fn))
    err("fs_is_dir " + fn);

  ok_msg("is_dir C++");

  return EXIT_SUCCESS;
}
