#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream> // std::ofstream

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

// UTF-8 filesize
  auto s = fs_file_size(d);
  std::cout << "own file size " << d << " " << s << "\n";
  if(!s)
    err("failed to get own file size " + d);

  std::string fn = "日本語.txt";
  // write "hello" to file using std::ofstream
  std::ofstream of(fn);
  if(!of)
    err("failed to open file for writing " + fn);
  of << "hello";
  of.close();

  s = fs_file_size(fn);
  std::cout << "file size " << fn << " " << s << "\n";
  if(s != 5){
    if((fs_is_mingw() && fs_backend() == "<filesystem>"))
      std::cerr << "MINGW with <filesystem> backend has a bug in fs_file_size\n";
    else
      err("failed to get correct file size " + fn);
  }

  ok_msg("file_size C++");

  return EXIT_SUCCESS;
}
