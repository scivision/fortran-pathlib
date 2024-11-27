#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){
#ifdef _MSC_VER
_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
/// is safe name expects ONLY the filename, not the path

std::string s;

s = "test/re/";

if(fs_is_safe_name(s))
  err(s);

s = "test/re";
if(fs_is_safe_name(s))
  err(s);

s = "hi.";
bool ok = fs_is_safe_name(s);
if(fs_is_windows() && ok)
  err(s);
if(!fs_is_windows() && !ok)
  err(s);

s = "hi there";
if(fs_is_safe_name(s))
  err(s);

s = "日本語";
if(fs_is_safe_name(s))
  err(s);

ok_msg("safe C++");

return EXIT_SUCCESS;
}
