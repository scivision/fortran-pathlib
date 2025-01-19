#include <iostream>
#include <cstdlib>
#include <algorithm>  // for std::move
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <iterator> // for std::back_inserter

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

int fail = 0;

std::vector<std::tuple<std::string_view, std::string_view, std::string_view>>
tests = {{"", "", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"a/b", "a/b", "."},
    {"a///b", "a/b", "."},
    {"a/b", "a///b", "."},
    {"a/b", "a/b/", "."},
    {"a/b/", "a/b", "."},
    {"a/b", "a", ".."},
    {"a/b", "a/", ".."},
    {"a", "a/b/", "b"},
    {"a", "a/b/.", "b"},
    {"a", "a/b/..", "."},
    {"a/b/c/d", "a/b", "../.."},
    {"a/b/c/d", "a/b/", "../.."},
    {"./a/b", "./a/c", "../c"},
};

std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tos;

if(fs_is_windows()){
tos = {
    {"C:/", "C:/a/b", "a/b"},
    {"C:/a/b", "C:/a/b", "."},
    {"C:/a/b", "C:/a", ".."},
    {"C:/a", "D:/a", "D:/a"}
};
// NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.

} else {
tos = {
    {"", "a", "a"},
    {"/", "/", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"/dev/null", "/dev/null", "."},
    {"a/b", "c", "../../c"},
    {"c", "a/b", "../a/b"},
    {"a/b", "a/c", "../c"}
};
}
// NOTE: use relative non-existing paths, as on macOS AppleClang, the <filesystem> gives incorrect results on non-existing absolute paths,
// Which don't make sense anyway.

std::move(tos.begin(), tos.end(), std::back_inserter(tests));

for (const auto& [a, b, expected] : tests) {
    std::string result = fs_proximate_to(a, b);
    if (result != expected) {
        fail++;
        std::cerr << "FAIL: proximate_to(" << a << ", " << b << ") -> "  << result  << " (expected: "  << expected << ")\n";
    } else {
        std::cout << "PASS: proximate_to(" << a << ", " << b << ") -> "  << result  << "\n";
    }
}

if(fail){
    std::cerr << "FAIL: proximate " << fail << " tests failed.  backend: " << fs_backend() << "\n";
    return EXIT_FAILURE;
}

  ok_msg("proximate_to C++");

  return EXIT_SUCCESS;
}
