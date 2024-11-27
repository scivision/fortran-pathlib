#include <iostream>
#include <cstdlib>

#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(
#if __has_cpp_attribute(maybe_unused)
  [[maybe_unused]]
#endif
    int argc, char* argv[])
{

const std::string s1(argv[0]);

std::uintmax_t i64 = fs_space_available(s1);

if (i64 == 0)
  err("space_available(" + s1 + ") " + std::to_string(i64));

ok_msg("space_available (GB): " + std::to_string(static_cast<double>(i64) / static_cast<double>(1024^3)));
}
