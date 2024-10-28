#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"


int main() {

std::vector<std::tuple<std::string_view, std::string_view>> tests = {
  {"", ""},
  {"a/b", ""},
  {"./a/b", ""},
  {"../a/b", ""}
};

if (fs_is_windows()) {
  tests.emplace_back("c:/a/b", "c:/");
  tests.emplace_back("/etc", "");
  tests.emplace_back("c:\\", "c:/");
} else {
  tests.emplace_back("/a/b", "/");
  tests.emplace_back("c:/etc", "");
}

for (const auto& [input, expected] : tests) {
  if (fs_root(std::string(input)) != expected) {
    std::cerr << "FAILED: root(" << input << ") " << fs_root(input) << "  expected: " << expected << "\n";
    return EXIT_FAILURE;
  }
}

// test root_name
tests = {
  {"", ""},
  {"a/b", ""},
  {"./a/b", ""},
  {"../a/b", ""}
};

if(fs_is_windows()){
  tests.emplace_back("c:/a/b", "c:");
  tests.emplace_back("/etc", "");
  tests.emplace_back("c:\\", "c:");
} else {
  tests.emplace_back("/a/b", "");
  tests.emplace_back("c:/etc", "");
}

for(const auto& [input, expected] : tests){
  if (fs_root_name(std::string(input)) != expected) {
    std::cerr << "FAILED: root_name(" << input << ") " << fs_root_name(input) << "  expected: " << expected << "\n";
    return EXIT_FAILURE;
  }
}


  std::cout << "OK: root(), root_name(): C++\n";
  return EXIT_SUCCESS;
}
