#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}


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
std::string home = Ffs::get_homedir();
if(home.empty())
  err("get_homedir() failed");

if (auto h = Ffs::canonical("~", true, true); !h || home != h)
  err("Ffs::canonical(~) " + h.value() + " != get_homedir() " + home);

std::string homep = Ffs::parent(home);
if(homep.empty())
  err("Ffs::parent(get_homedir()) failed");

// -- relative dir

if(auto h = Ffs::canonical("~/..", true, true); !h || homep != h)
  err("Ffs::canonical(~/..) != Ffs::parent(get_homedir()) " + h.value() + " != " + homep);

// -- relative file
if(fs_is_cygwin())
  // Cygwin can't handle non-existing canonical paths
  return EXIT_SUCCESS;

auto h = Ffs::canonical("~/../not-exist.txt", false, true);
if(!h)
  err("Ffs::canonical(\"~/../not-exist.txt\") failed weakly canonical");

if (h.value().length() <= 13)
  err("Ffs::canonical(\"~/../not-exist.txt\") didn't expand ~  " + h.value());

return EXIT_SUCCESS;
}
