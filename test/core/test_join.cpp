#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <tuple>

#include "ffilesystem.h"


int main() {

    const std::vector<std::tuple<std::string_view, std::string_view, std::string_view>> tests = {
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
            std::cerr << "FAILED: join(" << a << ", " << b << ") -> "  << result  << " (expected: "  << expected << ")\n";
            return EXIT_FAILURE;
        }
    }

  std::cout << "OK: join\n";
  return EXIT_SUCCESS;
}
