
#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"

int main() {
  const std::string t = fs_filesystem_type(fs_is_windows() ? std::getenv("SYSTEMDRIVE") : "/");

  if (t.empty()) {
    std::cerr << "Unknown filesystem type, see type ID in stderr to update fs_get_type()\n";
    return 77;
  }

  std::cout << "OK: filesystem_type: " << t << "\n";

  return EXIT_SUCCESS;
}
