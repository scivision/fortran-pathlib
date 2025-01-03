#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"

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

  std::string_view c1 = (argc > 1) ? argv[1] : argv[0];

  std::cout << "own command: " << c1 << "\n";
  const std::string cwd = fs_get_cwd();
  std::cout << "current directory: " << cwd << "\n";

  std::string s1 = fs_file_name(c1);
  std::string s2 = "./" + s1;

  if (!fs_is_file(s2))
    err("file name: " + s2 + " is not a file");

  if (!fs_is_file(s1))
    err("file name: " + s1 + " is not a file");

  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";

  if (!fs_equivalent(s1, s2))
    err("equivalent(" + s1 + " /= " + s2 + ")");

  s1 = fs_parent(s1);
  s2 = "./" + s1;

  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";

  if (!fs_equivalent(s1, s2))
    err("equivalent(" + s1 + " /= " + s2 + ")");


  s1 = "..";
  s2 = fs_parent(fs_get_cwd());
  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";
  if (!fs_equivalent(s1, s2))
    err("equivalent(" + s1 + " /= " + s2 + ")");

  if (!fs_equivalent(".", "./"))
    err("equivalent(., ./)");


  s1 = ".";
  s2 = fs_get_cwd();
  std::cout << "Testing equivalent(" << s1 << ", " << s2 << ")\n";
  if (!fs_equivalent(s1, s2))
    err("equivalent(" + s1 + " /= " + s2 + ")");

  fs_mkdir("test-equivalent/b/");

  s1 = "test-equivalent/c";
  fs_touch(s1);

  s2 = "test-equivalent/b/../c";

  if (!fs_equivalent(s1, s2))
    err("equivalent resolve");

  if (fs_equivalent("not-exist-same", "not-exist-same"))
    err("equivalent(not-exist-same, not-exist-same)");

  std::cout << "PASSED: equivalent(" << s1 << ", " << s2 << ")\n";

  // non-ASCII
  s1 = "日本語_equivalent";
  if(!fs_mkdir(s1))
    err("mkdir(" + s1 + ")");
  if(!fs_is_dir(s1))
    err("is_dir(" + s1 + ")");

  if(!fs_equivalent("./" + s1, s1))
    err("equivalent(./" + s1 + ", " + s1 + ")");

  ok_msg("equivalent C++");

  return EXIT_SUCCESS;
}
