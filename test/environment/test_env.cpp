#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}


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

std::string fpath = Ffs::get_cwd();
std::cout << "current working dir " << fpath << "\n";

if(!Ffs::exists(fpath))
  err("current working dir " + fpath + " does not exist");

if(!Ffs::is_dir(fpath))
  err("current working dir " + fpath + " is not a directory");

std::string cpath = std::filesystem::current_path().string();

if (std::string s = Ffs::normal(cpath); fpath != s)
  err("C cwd " + s + " != Fortran cwd " + fpath);

// --- homedir
std::string p = Ffs::get_homedir();
std::cout << "Home directory " << p << "\n";
if (p != Ffs::expanduser("~"))
  err("home dir " + p + " != expanduser('~') " + Ffs::expanduser("~"));

// --- tempdir
std::string t = Ffs::get_tempdir();
std::cout << "Temp directory " << t << "\n";
if (!Ffs::exists(t))
  err("Fortran: temp dir " + t + " does not exist");

// --- setenv
std::string k = "FORTtest";
std::string v = "FORTvalue";
Ffs::set_env(k, v);

if (std::string e = Ffs::get_env(k); e != v)
  err("Fortran: getenv " + k + "=" + e + " != " + v);

std::cout << "OK: C++ environment\n";

return EXIT_SUCCESS;
}
