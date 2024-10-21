#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>

#include <exception>
#include <stdexcept>

#include "ffilesystem.h"


void test_normal() {
    std::string p;

    std::vector<std::tuple<std::string_view, std::string_view>> test_cases = {
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
            throw std::runtime_error("normal(" + std::string(input) + ") " + std::string(p));
        }
    }

    std::cout << "OK: normal" << std::endl;
}

int main() {
    try {
        test_normal();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
