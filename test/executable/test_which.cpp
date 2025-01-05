#include <iostream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(int argc, char* argv[]) {
  int noenv = (argc > 1) ? std::stoi(argv[1]) : 0;

  if (noenv)
    fs_setenv("PATH", "");

  // Pick an executable name for test
  const std::string testExe = fs_is_windows() ? "cmake.exe" : "ls";
  std::string found = fs_which(testExe);
  if (found.empty())
    err("which: not found " + testExe);

  // Check absolute "which" consistency
  if (fs_which(found) != found)
    err("which: absolute path " + found);

  // Various other checks
  if (!fs_which("/not/a/path").empty())
    err("which: unexpected path match");

  if (!fs_which("").empty())
    err("which: empty name check");

  // Build a relative path from the running program
  std::string name = fs_file_name(argv[0]);
  std::string rel = std::string("./") + name;
  if (!fs_is_file(rel) || fs_which(rel).empty())
    err("which: relative path " + rel);

  // non-existent subdir
  std::string badRel = std::string("not-exist/") + name;
  if (!fs_which(badRel).empty())
    err("which: non-existent relative path check " + badRel);

  // Extra check for Windows-only scenario
  // Check that local dir is preferred
  if (fs_is_windows() && (!fs_is_file(name) || fs_which(name).empty()))
    err("which: local dir check " + name);

  ok_msg("which");
  return EXIT_SUCCESS;
}
