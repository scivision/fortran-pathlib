#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


int main(int argc, char* argv[])
{
#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  int shared = 0;
  if(argc > 1){
    shared = std::atoi(argv[1]);
  }

  std::string path = Ffs::lib_path();

  if(path.empty()){
    std::cerr << "test_binpath: lib_path should be non-empty: " << path << "\n";
    return EXIT_FAILURE;
  }

  auto s = std::filesystem::status(path);

  if(!std::filesystem::exists(s)){
    std::cerr << "test_binpath: lib_path should exist: " << path << "\n";
    return EXIT_FAILURE;
  }

  if(std::filesystem::is_directory(s)){
    if(shared){
      std::cerr << "test_binpath: lib_path should not be a directory: " << path << "\n";
      return EXIT_FAILURE;
    } else {
      std::cout << path << "\n";
      return EXIT_SUCCESS;
    }
  }

  if(std::filesystem::is_regular_file(s)){
    std::cout << path << "\n";
    return EXIT_SUCCESS;
  }

  std::cerr << "test_binpath: lib_path should be a regular file: " << path << "\n";
  return EXIT_FAILURE;
}
