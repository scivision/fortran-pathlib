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

// space

  auto avail = fs_space_available(d);
  if(!avail)
    err("failed to get space available of own drive " + d);

  float avail_GB = (float) avail / 1073741824;
  std::cout << "OK space available on drive of " << d <<  " (GB) " <<  avail_GB << "\n";

// UTF-8 space
  if(fs_is_mingw() && fs_backend() == "<filesystem>")
    std::cerr << "MINGW with <filesystem> backend has a bug in fs_space_available 0xc0000409\n";
  else {
    fn = fs_canonical(fn, true, false);
    avail = fs_space_available(fn);
    if(!avail)
      err("failed to get space available of file " + fn);
  }

  ok_msg("file_size, space_available C++");

  return EXIT_SUCCESS;
}
