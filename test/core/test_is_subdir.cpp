#include <iostream>
#include <cstdlib>
#include <vector>
#include <string_view>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

std::vector<std::tuple<std::string_view, std::string_view, bool>> test_cases = {
    {"a/b/c", "a/b", true}, {"a/b/c", "a/b/", true}, {"a/b/c", "a", true},
    {"a/b", "a/b", false}, {"a/b", "a/b/", false},
    {"a/b", "c", false}, {"b", "a/b", false}

};

int fail = 0;

    for (const auto& [s1, s2, expected] : test_cases) {
        if (fs_is_subdir(s1, s2) != expected) {
            std::cerr << "FAIL: " << s1 << " is " << (expected ? "not " : "") << "subdir of " << s2 << "\n";
            fail++;
        }
    }

    if(fail){
        std::cerr << "FAIL: fs_is_subdir " << fail << "failures.\n";
        return EXIT_FAILURE;
    }

    ok_msg("fs_is_subdir");

    return EXIT_SUCCESS;
}
