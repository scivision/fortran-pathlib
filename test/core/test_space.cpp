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

const std::string s1 = fs_parent(argv[0]);

std::uintmax_t avail = fs_space_available(s1);
std::uintmax_t cap = fs_space_capacity(s1);

if (avail == 0)
  err("space_available(" + s1 + ") " + std::to_string(avail));
if (cap == 0)
  err("space_capacity(" + s1 + ") " + std::to_string(cap));


ok_msg("space_available (GB): " + std::to_string(static_cast<double>(avail) / static_cast<double>(1024^3)));
ok_msg("space_capacity (GB): " + std::to_string(static_cast<double>(cap) / static_cast<double>(1024^3)));

return EXIT_SUCCESS;
}
