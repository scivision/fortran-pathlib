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

auto fpath = Ffs::get_cwd();
if(!fpath)
  return EXIT_FAILURE;

std::cout << "current working dir " << fpath.value() << "\n";

if(!Ffs::exists(fpath.value()))
  err("current working dir " + fpath.value() + " does not exist");

if(!Ffs::is_dir(fpath.value()))
  err("current working dir " + fpath.value() + " is not a directory");

std::string cpath = std::filesystem::current_path().string();

if (std::string s = Ffs::normal(cpath); fpath != s)
  err("C cwd " + s + " != Fortran cwd " + fpath.value());

std::cout << "PASS: C++ current working directory\n";

// --- profile dir
std::string pdir = Ffs::get_profile_dir();
std::cout << "Profile directory " << pdir << "\n";

// --- homedir
std::string p = Ffs::get_homedir();
std::cout << "Home directory " << p << "\n";

if (p.length() != pdir.length())
#ifdef __cpp_lib_format
  std::cerr << std::format("ERROR: length of profile dir {} != home dir {}\n", pdir.length(), p.length());
#else
  std::cerr << "length of profile dir " << pdir.length() << " != home dir " << p.length() << "\n";
#endif

if (p != pdir)
  err("profile dir " + pdir + " != home dir " + p);

if (p != Ffs::expanduser("~"))
  err("home dir " + p + " != expanduser('~') " + Ffs::expanduser("~"));

std::cout << "PASS: C++ home directory\n";

// --- tempdir
auto t = Ffs::get_tempdir();
if(!t)
  err("failed to get_tempdir()");

std::cout << "Temp directory " << t.value() << "\n";
if (!Ffs::exists(t.value()))
  err("Fortran: temp dir " + t.value() + " does not exist");

std::cout << "PASS: C++ temp directory\n";

// --- setenv
std::string k = "FORTtest";
std::string v = "FORTvalue";
Ffs::set_env(k, v);

if (std::string e = Ffs::get_env(k); e != v)
  err("Fortran: getenv " + k + "=" + e + " != " + v);

std::cout << "OK: C++ environment\n";

return EXIT_SUCCESS;
}
