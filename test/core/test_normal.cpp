#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "ffilesystem.h"


int main() {

    std::string p;

    int fail = 0;

    std::vector<std::tuple<std::string_view, std::string_view>> test_cases = {
        {"", "."},
        {"/", "/"},
        {"//", "/"},
        {"/////", "/"},
        {".", "."},
        {"./", "."},
        {"./.", "."},
        {"..", ".."},
        {"../", ".."},
        {"a/..", "."},
        {"../..", "../.."},
        {"/a", "/a"},
        {"a", "a"},
        {".a", ".a"},
        {"a.", "a."},
        {"a./", "a."},
        {"..a", "..a"},
        {"a..", "a.."},
        {"a../", "a.."},
        {"a/", "a"},
        {"a//", "a"},
        {"./a", "a"},
        {"./a/", "a"},
        {"./a/.", "a"},
        {"../a", "../a"},
        {"../a/b/..", "../a"},
        {"a/b/", "a/b"},
        {"a/b/.", "a/b"},
        {"a/b/..", "a"},
        {"a/b/../", "a"},
        {"a/b/../c", "a/c"},
        {"a/b/../c/d", "a/c/d"},
        {"a/b/../../c/d", "c/d"},
        {"././a/./b/././c/./.", "a/b/c"},
        {"a/b/../../c/../..", ".."},
        {"a/b/../../../c/../..", "../.."},
        {"a/./b/..", "a"},
        {"a/.///b/../", "a"},
        {"/a/../..", "/"},
        {"/a/../../../../", "/"}
    };
// some tests from https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/test/filesystem_test.cpp#L950

if(fs_is_windows()){
    test_cases.emplace_back("\\/\\///\\/", "/");
    test_cases.emplace_back("a/b/..\\//..///\\/../c\\\\/", "../c");
    test_cases.emplace_back("..a/b/..\\//..///\\/../c\\\\/", "../c");
    test_cases.emplace_back("..\\" , "..");
    test_cases.emplace_back("c:\\", "c:/");
    test_cases.emplace_back("c:\\\\", "c:/");
    test_cases.emplace_back("c:\\\\a/b/../", "c:/a");
}

    for (const auto& [input, expected] : test_cases) {
        p = fs_normal(input);
        if (p != expected) {
            std::cerr << "FAIL: normal(" << input << ") " << p << " expected " << expected << "\n";
            fail++;
        }
    }

    if (fail) {
        std::cerr << "FAILED " << fail << " normal tests\n";
        return EXIT_FAILURE;
    }

    std::cout << "OK: normal\n";
    return EXIT_SUCCESS;
}
