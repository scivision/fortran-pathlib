#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

  std::string fn = "日本語_is_file.txt";

  if(!fs_touch(fn))
    err("fs_touch " + fn);

  if(!fs_exists(fn)){
    if(fs_is_mingw() && fs_backend() == "<filesystem>")
      std::cerr << "MinGW: fs_exists on MinGW <filesystem> has bugs\n";
    else
      err("fs_exists " + fn);
  }

  if(!fs_is_file(fn)){
    if(fs_is_mingw() && fs_backend() == "<filesystem>")
      std::cerr << "MinGW: fs_is_file on MinGW <filesystem> has bugs\n";
    else
      err("fs_is_file " + fn);
  }

  if(fs_is_file("not-exist"))
    err("fs_is_file not-exist");

  if(fs_is_file("."))
    err("fs_is_file .");

  ok_msg("is_file C++");

  return EXIT_SUCCESS;
}
