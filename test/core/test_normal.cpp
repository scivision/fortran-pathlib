#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "ffilesystem.h"


int main() {

    std::string p;

    const std::vector<std::tuple<std::string_view, std::string_view>> test_cases = {
        {"", "."},
        {"/", "/"},
        {"//", "/"},
        {".", "."},
        {"./", "."},
        {"./.", "."},
        {"..", ".."},
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
        {"a/b/../../../c/../..", "../.."}
    };

    for (const auto& [input, expected] : test_cases) {
        p = fs_normal(input);
        if (p != expected) {
            std::cerr << "normal(" << input << ") " << p << "\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "OK: normal\n";
    return EXIT_SUCCESS;
}
