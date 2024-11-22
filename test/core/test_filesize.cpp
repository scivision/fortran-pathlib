#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
int argc,char *argv[]){

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  std::string d(argv[0]);

  if(!fs_file_size(d))
    err("failed to get own file size");

  auto avail = fs_space_available(d);
  if(!avail)
    err("failed to get space available of own drive " + d);

  float avail_GB = (float) avail / 1073741824;
  std::cout << "OK space available on drive of " << d <<  " (GB) " <<  avail_GB << "\n";

  return EXIT_SUCCESS;
}
