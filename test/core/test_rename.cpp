#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int
main()
{
  constexpr std::string_view f1 = "test_Ffs_rename.txt";
  constexpr std::string_view f2 = "test_Ffs_rename2.txt";

  if(fs_exists(f2) && !fs_remove(f2))
    err("remove() failed");

  if(!fs_touch(f1))
    err("touch() failed");

  if(!fs_is_file(f1))
    err("is_file() failed");

  if(!fs_rename(f1, f2))
    err("rename() failed");

  if(!fs_is_file(f2))
    err("is_file() failed after rename()");

  if(!fs_remove(f2))
    err("remove() failed");

  ok_msg("rename C++");

  return EXIT_SUCCESS;
}
