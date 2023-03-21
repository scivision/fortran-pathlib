#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "ffilesystem.h"

void test_filename()
{

std::string p1, p2;

if(fs_file_name("") != "")
  throw std::runtime_error("filename empty: " + fs_file_name(""));

std::cout << "PASS:filename:empty\n";

if (fs_file_name("a/b/c") != "c")
  throw std::runtime_error("file_name failed: " + fs_file_name("a/b/c"));

if (fs_file_name("a") != "a")
 throw std::runtime_error("file_name idempotent failed: " + fs_file_name("a"));

if(fs_file_name("file_name") != "file_name")
  throw std::runtime_error("file_name plain filename: " + fs_file_name("file_name"));

std::string nr = fs_file_name(fs_root(fs_get_cwd()));
if(!nr.empty())
  throw std::runtime_error("file_name root: " + nr);

if(fs_file_name(".file_name") != ".file_name")
  throw std::runtime_error("file_name leading dot filename: " + fs_file_name(".file_name"));

if(fs_file_name("./file_name") != "file_name")
  throw std::runtime_error("file_name leading dot filename cwd: " + fs_file_name("./file_name"));

if(fs_file_name("file_name.txt") != "file_name.txt")
  throw std::runtime_error("file_name leading dot filename w/ext");

if(fs_file_name("./file_name.txt") != "file_name.txt")
  throw std::runtime_error("file_name leading dot filename w/ext and cwd");

if(fs_file_name("../file_name.txt") != "file_name.txt")
  throw std::runtime_error("file_name leading dot filename w/ext up " + fs_file_name("../file_name.txt"));

if(fs_is_windows() && fs_file_name("c:\\my\\path") != "path")
  throw std::runtime_error("file_name windows: " + fs_file_name("c:\\my\\path"));

}


int main() {
  test_filename();

  return EXIT_SUCCESS;
}
