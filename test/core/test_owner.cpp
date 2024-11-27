#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
int argc, char *argv[]) {

  std::string_view exe = argv[0];
  std::cout << "Executable: " << exe << "\n";

  const std::string user = fs_get_username();
  std::cout << "User: " << user << "\n";;

  const std::string name = fs_get_owner_name(exe);
  std::cout << "Owner name: " << name << "\n";;
  if (name.empty())
    err("No owner information");

  const std::string group = fs_get_owner_group(exe);
  std::cout << "Owner group: " << group << std::endl;
  if (group.empty())
    err("No owner group information");

  if (user == name) {
    std::cout << "OK: User and owner match\n";
    return EXIT_SUCCESS;
  }

  std::cerr << "User and owner didn't match\n";

  if (fs_getenv("CI") == "true")
    std::cerr << "mismatched username and owner can happen on CI systems\n";

  ok_msg("get_owner C++");

  return EXIT_SUCCESS;
}
