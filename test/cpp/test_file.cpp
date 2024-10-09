// no need to duplicate this in test/cpp
#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  if(argc != 1){
    std::cerr << "Usage: " << argv[0] << "\n";
    return EXIT_FAILURE;
  }

  std::string drive(argv[0]);

  if(!Ffs::file_size(drive))
    err("failed to get own file size");

  auto avail = fs_space_available(drive);
  if(!avail)
    err("failed to get space available of own drive " + drive);

  float avail_GB = (float) avail.value() / 1073741824;
  std::cout << "OK space available on drive of " << drive <<  " (GB) " <<  avail_GB << "\n";

  return EXIT_SUCCESS;
}
