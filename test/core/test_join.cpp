#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <tuple>

#include <exception>
#include <stdexcept>

#include "ffilesystem.h"


void test_join() {
    std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tests = {
        {"", "", ""},
        {"a", "", "a"},
        {"", "b", "b"},
        {"a/b/", "c/", "a/b/c"},
        {"/", "", "/"},
        {"", "/", "/"},
        {"a", "b//", "a/b"},
        {"a//", "b//", "a/b"},
        {"a/b/../", "c/d/../", "a/c"},
        {"a/b", "..", "a"},
        {"a/b", "c/d", "a/b/c/d"},
        {"ab/cd", "/ef", "/ef"}
    };

    for (const auto& [a, b, expected] : tests) {
        std::string result = fs_join(a, b);
        if (result != expected) {
            throw std::runtime_error("Test failed: join(" + std::string(a) + ", " + std::string(b) + ") -> " + result + " (expected: " + std::string(expected) + ")");
        }
    }

}

int main() {
    try {
        test_join();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OK: join" << std::endl;
    return EXIT_SUCCESS;
}
