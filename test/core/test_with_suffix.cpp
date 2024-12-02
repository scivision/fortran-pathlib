#include <cstdlib>
#include <iostream>
#include <vector>
#include <string_view>
#include <tuple>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

std::string r;
std::string ref;

int fail = 0;

const std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tests = {
  {"", ".h5", ".h5"},
  {"foo.h5", "", "foo"},
  {".foo.h5", ".txt", ".foo.txt"},
  {".h5", "", ".h5"},
  {".h5", ".h5", ".h5.h5"},
  {"a//b///c/", ".h5", "a//b///c/.h5"},
  {"c:/a/hi.nc", ".h5", "c:/a/hi.h5"},
  {"my/file.h5", ".hdf5", "my/file.hdf5"},
  {"a/boo", ".h5", "a/boo.h5"},
  {"boo", ".h5", "boo.h5"},
  {"a/b/c.d/", ".hdf5", "a/b/c.d/.hdf5"},
  {"dir.h5/", ".hdf5", "dir.h5/.hdf5"}
};

for (const auto& [input, suffix, expected] : tests) {
  r = fs_with_suffix(input, suffix);

  if (r != expected) {
    std::cerr << "FAIL: with_suffix(" << input << ", " << suffix << " = " + r + "   expected: " << expected << "\n";
    fail++;
  } else
    std::cout << "PASS: with_suffix(" << input << ", " << suffix << " = " + r + "\n";
}

  if(fail){
    std::cerr << "Failed: " << fail << " tests  backend: " << fs_backend() << "\n";
    return EXIT_FAILURE;
  }

  ok_msg("with_suffix C++");

  return EXIT_SUCCESS;
}
