#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"

#if __has_include(<format>)
#include <format>
#endif

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

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

auto fpath = fs_get_cwd();
if(fpath.empty())
  return EXIT_FAILURE;

std::cout << "current working dir " << fpath << "\n";

if(!fs_exists(fpath))
  err("current working dir " + fpath + " does not exist");

if(!fs_is_dir(fpath))
  err("current working dir " + fpath + " is not a directory");

const auto cpath = fs_get_cwd();
if(cpath.empty())
  return EXIT_FAILURE;

if (std::string s = fs_normal(cpath); fpath != s)
  err("C cwd " + s + " != Fortran cwd " + fpath);

std::cout << "PASS: C++ current working directory\n";

// --- profile dir
std::string pdir = fs_get_profile_dir();
std::cout << "Profile directory " << pdir << "\n";

// --- homedir
std::string p = fs_get_homedir();
std::cout << "Home directory " << p << "\n";

if (p.length() != pdir.length())
#ifdef __cpp_lib_format
  std::cerr << std::format("ERROR: length of profile dir {} != home dir {}\n", pdir.length(), p.length());
#else
  std::cerr << "length of profile dir " << pdir.length() << " != home dir " << p.length() << "\n";
#endif

if (p != pdir)
  err("profile dir " + pdir + " != home dir " + p);

if (p != fs_expanduser("~"))
  err("home dir " + p + " != expanduser('~') " + fs_expanduser("~"));

std::cout << "PASS: C++ home directory\n";

// --- tempdir
auto t = fs_get_tempdir();
if(t.empty())
  err("failed to get_tempdir()");

std::cout << "Temp directory " << t << "\n";
if (!fs_exists(t))
  err("Fortran: temp dir " + t + " does not exist");

std::cout << "PASS: C++ temp directory\n";

// --- setenv
std::string k = "FORTtest";
std::string v = "FORTvalue";
fs_setenv(k, v);

if (std::string e = fs_getenv(k); e != v)
  err("Fortran: getenv " + k + "=" + e + " != " + v);

std::cout << "OK: C++ environment\n";

return EXIT_SUCCESS;
}
