// tests for fs_is_appexec_alias()

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(int argc, char* argv[])
{

if (!fs_is_windows())
  skip("This test is only for Windows");

std::string path;
// not string_view, it garbles on some platforms

if (argc > 1)
  path = argv[1];
else {
  const std::string appdir = fs_user_config_dir() + "/Microsoft";
  std::cout << "App Execution Alias directory: " << appdir << "\n";

  std::string s = fs_which("wt.exe");
  if (!s.empty() && fs_is_subdir(s, appdir))
    path = s;

  std::cout << "which: " << s << "\n";

  if(path.empty())
    skip("didn't find any App Execution Alias to test");
}

std::cout << "Testing fs_is_appexec_alias(" << path << ")\n";

if (!fs_is_appexec_alias(path))
  err("fs_is_appexec_alias(" + std::string(path) + ") was not an App Execution Alias");

ok_msg("fs_is_appexec_alias(" + std::string(path) + ")");

return EXIT_SUCCESS;

}
