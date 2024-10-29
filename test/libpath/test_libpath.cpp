#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


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

  std::string path = fs_lib_path();

  if(path.empty()){
    std::cerr << "test_binpath: lib_path is empty: " << path << "\n";
    return EXIT_FAILURE;
  }

  if(!fs_exists(path)){
    std::cerr << "test_binpath: lib_path should exist: " << path << "\n";
    return EXIT_FAILURE;
  }

  if(fs_is_dir(path)){
    std::cerr << "test_binpath: lib_path should not be a directory: " << path << "\n";
    return EXIT_FAILURE;
  }

  if(fs_is_file(path)){
    std::cout << path << "\n";
    return EXIT_SUCCESS;
  }

  std::cerr << "test_binpath: lib_path should be a regular file: " << path << "\n";
  return EXIT_FAILURE;
}
