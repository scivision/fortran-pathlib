#include <iostream>
#include "ffilesystem.h"
#include <cstdlib>
#include <string_view>

int main(int argc, char* argv[]) {

if (argc < 3) {
    std::cerr << "please give test link file and link dir\n";
    return EXIT_FAILURE;
}

std::string_view tfile = argv[1];
std::string_view tdir = argv[2];

if (fs_is_symlink("not-exist-file")) {
  std::cerr << "is_symlink() should be false for non-existent file\n";
  return EXIT_FAILURE;
}

if(fs_is_symlink("")) {
  std::cerr << "is_symlink() should be false for empty file\n";
  return EXIT_FAILURE;
}

if(!fs_is_file(tfile)){
    std::cerr << "is_file(" << tfile << ") should be true for symlink file\n";
    return EXIT_FAILURE;
}

if(!fs_is_symlink(tfile)) {
  std::cerr << "is_symlink(" << tfile << ") should be true for symlink file\n";
  return EXIT_FAILURE;
}

std::cout << "PASS: file is_symlink(" << tfile << ")\n";

if (!fs_is_dir(tdir)){
    std::cerr << "is_dir(" << tdir << ") should be true for symlink directory\n";
    return EXIT_FAILURE;
}
if (!fs_is_symlink(tdir)){
    std::cerr << "is_symlink(" << tdir << ") should be true for symlink directory\n";
    return EXIT_FAILURE;
}

std::cout << "PASS: directory is_symlink(" << tdir << ")\n";

std::cout << "OK: is_symlink\n";
return EXIT_SUCCESS;
}
