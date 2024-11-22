#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"

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

  r = fs_expanduser("");

  if(!r.empty())
    err("expanduser('') != ''  " + r);

  r = fs_expanduser(".");
  if (r != ".")
    err("expanduser('.') != '.'" + r);

  r = fs_expanduser("~");
  h = fs_get_homedir();
  if(r != h)
    err("expanduser('~') != homedir  => " + r + " != " + h);

  std::cout << "~: " << h << "\n";

  r = fs_expanduser("~/");
  h = fs_get_homedir();
  if (r != h)
    err("expanduser('~/') != homedir + '/'  =>  " + r + " != " + h);

  std::cout << "~/: " << h << "\n";

  r = fs_expanduser("~//");
  if (r != h)
    err("expanduser('~//') != homedir");

  std::string j = "日本語";
  r = fs_expanduser(j);
  if (r != j)
    err("expanduser(" + j + ") != " + j);

  std::cout << "OK: Cpp expanduser\n";

  return EXIT_SUCCESS;
}
