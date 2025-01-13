
#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main() {
  const std::string t = fs_filesystem_type(fs_is_windows() ? fs_getenv("SYSTEMDRIVE") : "/");

  if (t.empty())
    skip("Unknown filesystem type, see type ID in stderr to update fs_get_type()");

  std::cout << "OK: filesystem_type: " << t << "\n";

  return EXIT_SUCCESS;
}
