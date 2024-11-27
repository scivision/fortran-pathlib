#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


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

  if(path.empty())
    err("test_binpath: lib_path is empty: " + path);

  if(!fs_exists(path))
    err("test_binpath: lib_path should exist: " + path);

  if(fs_is_dir(path))
    err("test_binpath: lib_path should not be a directory: " + path);

  if(fs_is_file(path)){
    ok_msg("libpath: " + path);
    return EXIT_SUCCESS;
  }

  err("test_binpath: lib_path should be a regular file: " + path);

  return EXIT_FAILURE;
}
