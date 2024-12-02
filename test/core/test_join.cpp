#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

  int fail = 0;

  const std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tests = {
    {"", "", ""},
    {"a", "", "a"},
    {"", "b", "b"},
    {"a//b/", "c/", "a//b/c"},
    {"/", "", "/"},
    {"", "/", "/"},
    {"a", "b//", "a/b"},
    {"a//", "b//", "a/b"},
    {"a/b/../", "c/d/../", "a/b/../c/d/.."},
    {"a/b", "..", "a/b/.."},
    {"./a/b", ".", "./a/b/."},
    {"a/b", "c/d", "a/b/c/d"},
    {"ab/cd", "/ef", "/ef"}
  };

  for (const auto& [a, b, expected] : tests) {
    std::string result = fs_join(a, b);
    if (result != expected) {
      std::cerr << "FAIL: join(" << a << ", " << b << ") -> "  << result  << "  expected: "  << expected << "\n";
      fail++;
    } else
      std::cout << "PASS: join(" << a << ", " << b << ") -> "  << result << "\n";
  }

  if(fail){
    std::cerr << "Failed: " << fail << " tests  backend: " << fs_backend() << "\n";
    return EXIT_FAILURE;
  }

  ok_msg("join C++");

  return EXIT_SUCCESS;
}
