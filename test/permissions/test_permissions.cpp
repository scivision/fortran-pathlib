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

if(auto p = fs_get_permissions(""); p)
    err("get_permissions('') should fail, but got: " + p.value());

std::string read = "readable.txt";
std::string noread = "nonreadable.txt";
std::string nowrite = "nonwritable.txt";

Ffs::touch(read);
fs_set_permissions(read, 1, 0, 0);

auto p = fs_get_permissions(read);
if(!p)
    err("get_permissions('" + read + "') failed");

std::cout << "Permissions for " << read << ": " << p.value() << "\n";

if(!fs_is_readable(read))
    err(read + " should be readable");

if(!fs_exists(read))
    err(read + " should exist");

if(!fs_is_file(read))
    err(read + " should be a file");

// for Ffilesystem, even non-readable files "exist" and are "is_file"
Ffs::touch(noread);
fs_set_permissions(noread, -1, 0, 0);

p = fs_get_permissions(noread);
if(!p)
    err("get_permissions('" + noread + "') failed");

std::cout << "Permissions for " << noread << ": " << p.value() << "\n";

#ifdef __cpp_lib_string_contains
if(!p.value().contains("r") && fs_is_readable(noread)){

if(!fs_exists(noread))
    err(noread + " should exist");

if(!fs_is_file(noread))
    err(noread + " should be a file");
}
#else
std::cerr << "SKIP: due to not having C++23 string contains support\n";
#endif

// writable
if(!fs_is_file(nowrite))
  Ffs::touch(nowrite);
fs_set_permissions(nowrite, 0, -1, 0);

p = fs_get_permissions(nowrite);
if(!p)
    err("get_permissions('" + nowrite + "') failed");
std::cout << "Permissions for " << nowrite << " " << p.value() << "\n";

#ifdef __cpp_lib_string_contains
if(!p.value().contains("w") && fs_is_writable(nowrite)){
  std::cerr << "ERROR:  " << nowrite << " should not be writable\n";
  if(!fs_is_windows())
    return EXIT_FAILURE;
}
#else
std::cerr << "SKIP: due to not having C++23 string contains support\n";
#endif

if(!fs_exists(nowrite))
    err(nowrite + " should exist");

if(!fs_is_file(nowrite))
    err(nowrite + " should be a file");

return EXIT_SUCCESS;
}
