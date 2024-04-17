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

std::string p = Ffs::get_permissions("");
if(p.length() != 0)
    err("get_permissions('') should be empty, got: " + p);

std::string read = "readable.txt";
std::string noread = "nonreadable.txt";
std::string nowrite = "nonwritable.txt";

Ffs::touch(read);
Ffs::set_permissions(read, 1, 0, 0);

p = Ffs::get_permissions(read);
std::cout << "Permissions for " << read << ": " << p << "\n";

if(p.length() == 0)
    err("get_permissions('" + read + "') should not be empty");

if(!Ffs::is_readable(read))
    err(read + " should be readable");

if(!Ffs::exists(read))
    err(read + " should exist");

if(!Ffs::is_file(read))
    err(read + " should be a file");

// for Ffilesystem, even non-readable files "exist" and are "is_file"
Ffs::touch(noread);
Ffs::set_permissions(noread, -1, 0, 0);

p = Ffs::get_permissions(noread);
std::cout << "Permissions for " << noread << ": " << p << "\n";

if(p.length() == 0)
    err(noread + "') should not be empty");

if(Ffs::is_readable(noread)){
    std::cerr << "XFAIL: test_exe: " << noread << " should not be readable\n";
} else {

if(!Ffs::exists(noread))
    err(noread + " should exist");

if(!Ffs::is_file(noread))
    err(noread + " should be a file");
}
// writable
if(!Ffs::is_file(nowrite))
  Ffs::touch(nowrite);
Ffs::set_permissions(nowrite, 0, -1, 0);

std::cout << "Permissions for " << nowrite << ": " << Ffs::get_permissions(nowrite) << "\n";

if(Ffs::is_writable(nowrite)){
  std::cerr << "ERROR: test_exe: " << nowrite << " should not be writable\n";
  if(!fs_is_windows())
    return EXIT_FAILURE;
}

if(!Ffs::exists(nowrite))
    err(nowrite + " should exist");

if(!Ffs::is_file(nowrite))
    err(nowrite + " should be a file");

return EXIT_SUCCESS;
}
