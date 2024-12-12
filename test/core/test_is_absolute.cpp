#include <iostream>
#include <cstdlib>

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

if (fs_is_absolute("")) {
  std::cerr << "blank is not absolute\n";
  return EXIT_FAILURE;
}

if(fs_is_windows()){
  if (!fs_is_absolute("J:/"))
    err("is_absolute(J:/) on Windows should be true");

  if (!fs_is_absolute("j:/"))
    err("is_absolute(j:/) on Windows should be true");

  if (fs_is_absolute("j:"))
    err("is_absolute(j:) on Windows should be false");

  if (fs_is_absolute("/"))
    err("is_absolute(/) on Windows should be false");

  if (fs_is_absolute("/日本語"))
    err("is_absolute(/日本語) on Windows should be false");

} else {
  if (!fs_is_absolute("/"))
    err("is_absolute(/) on Unix should be true");

  if (!fs_is_absolute("/日本語"))
    err("is_absolute(/日本語) should be true");

  if (fs_is_absolute("j:/"))
    err("is_absolute(j:/) on Unix should be false");
}

  if (fs_is_absolute("日本語"))
    err("is_absolute(日本語) should be false");

  if (fs_is_absolute("some space here"))
    err("is_absolute(some space here) should be false");

  ok_msg("is_absolute C++");

  return EXIT_SUCCESS;
}
