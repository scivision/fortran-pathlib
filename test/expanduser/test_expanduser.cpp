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

int main(void){

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  std::string r;
  std::string h;

  r = Ffs::expanduser("");

  if(!r.empty())
    err("expanduser('') != ''  " + r);

  r = Ffs::expanduser(".");
  if (r != ".")
    err("expanduser('.') != '.'" + r);

  r = Ffs::expanduser("~");
  h = Ffs::get_homedir();
  if(r != h)
    err("expanduser('~') != homedir  => " + r + " != " + h);

  std::cout << "~: " << h << "\n";

  r = Ffs::expanduser("~/");
  h = Ffs::get_homedir();
  if (r != h)
    err("expanduser('~/') != homedir + '/'  =>  " + r + " != " + h);

  std::cout << "~/: " << h << "\n";

  r = Ffs::expanduser("~//");
  if (r != h)
    err("expanduser('~//') != homedir");

  std::cout << "OK: Cpp expanduser\n";

  return EXIT_SUCCESS;
}
