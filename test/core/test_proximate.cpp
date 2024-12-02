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

std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tests;

if(fs_is_windows()){
tests = {
    {"", "", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"c:/a/b", "c:/", "../.."},
    {"c:\\a\\b", "c:/", "../.."},
    {"c:/", "c:/a/b", "a/b"},
    {"c:/a/b", "c:/a/b", "."},
    {"c:/a/b", "c:/a", ".."},
    {"c:/a/b/c/d", "c:/a/b", "../.."},
    {"c:/path", "d:/path", "d:/path"},
    {"c:/path", "d:/path/hi/", "d:/path/hi/"},
    {"c:/path", "d:/path/hi/.", "hi/."},
    {"c:/path", "d:/path/hi/..", "hi/.."}
};
} else {
tests = {
    {"", "", "."},
    {"", "/", "/"},
    {"/", "", ""},
    {"/", "/", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"/dev/null", "/dev/null", "."},
    {"/a/b", "c", "c"},
    {"c", "/a/b", "/a/b"},
    {"/a/b", "/a/b", "."},
    {"/a/b", "/a/b/", "."},
    {"/a/b/", "/a/b", "."},
    {"/a/b", "/a", ".."},
    {"/a/b", "/a/", "../"},
    {"/a/b/c/d", "/a/b", "../.."},
    {"./this/one", "./this/two", "../two"},
    {"/this/one", "/this/two", "../two"},
    {"/path/same", "/path/same/hi/", "hi/"},
    {"/path/same", "/path/same/hi/.", "hi/."},
    {"/path/same", "/path/same/hi/..", "hi/.."}
};
}

for (const auto& [a, b, expected] : tests) {
    std::string result = fs_proximate_to(a, b);
    if (result != expected) {
        fail++;
        std::cerr << "FAIL: proximate_to(" << a << ", " << b << ") -> "  << result  << " (expected: "  << expected << ")\n";
    }
}

if(fail){
    std::cerr << "FAIL: proximate " << fail << " tests failed.  backend: " << fs_backend() << "\n";
    return EXIT_FAILURE;
}

  ok_msg("proximate_to C++");

  return EXIT_SUCCESS;
}
