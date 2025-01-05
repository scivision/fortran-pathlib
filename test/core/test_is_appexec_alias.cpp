// tests for fs_is_appexec_alias()

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(int argc, char* argv[])
{

if (!fs_is_windows()) {
  std::cerr << "This test is only for Windows\n";
  return 77;
}

std::string_view path;

if (argc > 1)
  path = argv[1];
else {
  const std::string appdir = fs_user_config_dir() + "/Microsoft";
  std::cout << "App Execution Alias directory: " << appdir << "\n";
  // try a few exe like bash.exe, wt.exe
  // notepad.exe is not always an alias, as it was a system program historically
  for (const auto& p : {"bash.exe", "wt.exe", "code.cmd", "ninja.exe", "notepad.exe", "pbrush.exe"}) {
    std::string s = fs_which(p);
    if (!s.empty() && fs_is_subdir(s, appdir)) {
      path = s;
      break;
    }
  }
}

if(path.empty()){
  std::cerr << "didn't find any App Execution Alias to test, skipping\n";
  return 77;
}

std::cout << "Testing fs_is_appexec_alias(" << path << ")\n";

if (!fs_is_appexec_alias(path))
  err("fs_is_appexec_alias(" + std::string(path) + ") was not an App Execution Alias");

ok_msg("fs_is_appexec_alias(" + std::string(path) + ")");

return EXIT_SUCCESS;

}
