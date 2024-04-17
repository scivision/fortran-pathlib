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

std::string exe = "test_exe";
std::string noexe = "test_noexe";

// Empty string
if(Ffs::is_exe(""))
    err("test_exe: is_exe('') should be false");

// Non-existent file
if (Ffs::is_file("not-exist"))
    err("test_exe: not-exist-file should not exist.");
if (Ffs::is_exe("not-exist"))
    err("test_exe: not-exist-file cannot be executable");
if(Ffs::get_permissions("not-exist").length() != 0)
    err("test_exe: get_permissions('not-exist') should be empty");

Ffs::touch(exe);
Ffs::touch(noexe);

Ffs::set_permissions(exe, 0, 0, 1);
Ffs::set_permissions(noexe, 0, 0, -1);

if(Ffs::is_exe(Ffs::parent(exe)))
    err("test_exe: is_exe() should not detect directory " + Ffs::parent(exe));

std::string p;

std::cout << "permissions: " << exe << " = " << Ffs::get_permissions(exe) << "\n";

if (!Ffs::is_exe(exe))
  err("test_exe: " + exe + " is not executable and should be.");

std::cout << "permissions: " << noexe << " = " << Ffs::get_permissions(noexe) << "\n";

if (Ffs::is_exe(noexe)){
  if(fs_is_windows()){
    std::cerr << "XFAIL:Windows: test_exe: is_exe() did not detect non-executable file " << noexe << " on Windows\n";
  }
  else{
   err("test_exe: " + noexe + " is executable and should not be.");
  }
}

// chmod setup

Ffs::remove(exe);
Ffs::remove(noexe);

// chmod(true)
Ffs::touch(exe);
if (!Ffs::is_file(exe))
    err("test_exe: " + exe + " is not a file.");

std::cout << "permissions before chmod(" << exe << ", true)  = " << Ffs::get_permissions(exe) << "\n";

Ffs::set_permissions(exe, 0, 0, 1);

p = Ffs::get_permissions(exe);
std::cout << "permissions after chmod(" << exe << ", true) = " << p << "\n";

if (!Ffs::is_exe(exe)){
  if(fs_is_windows()){
    std::cerr << "XFAIL:Windows: test_exe: is_exe() did not detect executable file " << exe << " on Windows\n";
  }
  else{
    err("test_exe: is_exe() did not detect executable file " + exe);
  }
}

if (!fs_is_windows() && p[2] != 'x'){
  err("test_exe: expected POSIX perms for " + exe + " to be 'x' in index 2");
}

// chmod(false)
Ffs::touch(noexe);
if (!Ffs::is_file(noexe))
    err("test_exe: " + noexe + " is not a file.");

std::cout << "permissions before chmod(" << noexe << ", false)  = " << Ffs::get_permissions(noexe) << "\n";

Ffs::set_permissions(noexe, 0, 0, 0);

p = Ffs::get_permissions(noexe);
std::cout << "permissions after chmod(" << noexe << ",false) = " << p << "\n";

if(!fs_is_windows())
{
  if (Ffs::is_exe(noexe))
    err("test_exe: did not detect non-executable file.");

  if (p[2] != '-')
    err("test_exe: expected POSIX perms for " + noexe + " to be '-' in index 2");
}

// test Ffs::which
if(fs_is_windows()){
    std::string which = Ffs::which("cmd.exe");
    if(which.length() == 0)
        err("test_exe: Ffs::which('cmd.exe') should return a path");
    std::cout << "Ffs::which('cmd.exe') = " << which << "\n";
    }
else{
    std::string which = Ffs::which("ls");
    if(which.length() == 0)
        err("test_exe: Ffs::which('ls') should return a path");
    std::cout << "Ffs::which('ls') = " << which << "\n";
}

Ffs::remove(exe);
Ffs::remove(noexe);

std::cout << "OK: c++ test_exe\n";

return EXIT_SUCCESS;
}
