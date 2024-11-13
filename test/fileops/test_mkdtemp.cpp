#include <iostream>
#include <cstdlib>

#include "ffilesystem.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

int main()
{
#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

const std::string tdir = fs_mkdtemp("tmp.XXXXXX");

if(!fs_is_dir(tdir)){
  std::cerr << "FAILED: test_mkdtemp: temp dir not created " << tdir << "\n";
  return EXIT_FAILURE;
}

std::cout << "C++ mkdtemp: " << tdir << "\n";

std::cout << "OK: C++ mkdtemp\n";

return EXIT_SUCCESS;

}
