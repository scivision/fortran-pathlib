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
    {"a/b", "a/", "../"},
    {"a", "a/hi/", "hi/"},
    {"a", "a/hi/.", "hi/."},
    {"a", "a/hi/..", "hi/.."},
    {"a/b/c/d", "a/b", "../.."},
    {"a/b/c/d", "a/b/", "../../"},
    {"./this/one", "./this/two", "../two"},
};

std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tos;

if(fs_is_windows()){
tos = {
    {"c:/", "c:/a/b", "a/b"},
    {"c:/a/b", "c:/a/b", "."},
    {"c:/a/b", "c:/a", ".."},
    {"c:/a", "d:/a", "d:/a"}
};
} else {
tos = {
    {"", "/", "/"},
    {"/", "", ""},
    {"/", "/", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"/dev/null", "/dev/null", "."},
    {"/a/b", "c", "c"},
    {"c", "/a/b", "/a/b"},
    {"/this/one", "/this/two", "../two"}
};
}

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
