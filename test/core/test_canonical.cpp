#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

// -- home directory
std::string home = fs_get_homedir();
if(home.empty())
  err("get_homedir() failed");

if (auto h = fs_canonical("~", true, true); home != h)
  err("fs_canonical(~) " + h + " != get_homedir() " + home);

std::string homep = fs_parent(home);
if(homep.empty())
  err("parent(get_homedir()) failed");

// -- relative dir

if(auto h = fs_canonical("~/..", true, true); homep != h)
  err("fs_canonical(~/..) != parent(get_homedir()) " + h + " != " + homep);

// -- relative file
if(fs_is_cygwin())
  // Cygwin can't handle non-existing canonical paths
  return EXIT_SUCCESS;

auto h = fs_canonical("~/../not-exist.txt", false, true);
if(h.empty())
  err("fs_canonical(\"~/../not-exist.txt\") failed weakly canonical");

if (h.length() <= 13)
  err("fs_canonical(\"~/../not-exist.txt\") didn't expand ~  " + h);

return EXIT_SUCCESS;
}
