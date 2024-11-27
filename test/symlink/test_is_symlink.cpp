#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(int argc, char* argv[]) {

if (argc < 3) {
  std::cerr << "please give test link file and link dir\n";
  return EXIT_FAILURE;
}

const std::string tfile = argv[1];
const std::string tdir = argv[2];

if (fs_is_symlink("not-exist-file"))
  err("is_symlink() should be false for non-existent file");

if(fs_is_symlink(""))
  err("is_symlink() should be false for empty file");

if(!fs_is_file(tfile))
  err("is_file(" + tfile + ") should be true for symlink file");

if(!fs_is_symlink(tfile))
  err("is_symlink(" + tfile + ") should be true for symlink file");

std::cout << "PASS: file is_symlink(" << tfile << ")\n";

if (!fs_is_dir(tdir))
  err("is_dir(" + tdir + ") should be true for symlink directory");

if (!fs_is_symlink(tdir))
  err("is_symlink(" + tdir + ") should be true for symlink directory");

std::cout << "PASS: directory is_symlink(" << tdir << ")\n";

ok_msg("is_symlink C++");

return EXIT_SUCCESS;
}
